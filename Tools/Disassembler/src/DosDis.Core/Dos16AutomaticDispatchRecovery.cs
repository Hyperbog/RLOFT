using System.Buffers.Binary;

namespace DosDis.Core;

public sealed record Dos16GuardedDispatchSite(
    Dos16Address Address, string Status, string? PointerKind, int? EntryCount,
    string Evidence, Dos16TableFinding? StoredTable);
public sealed record Dos16GuardedDispatchReport(
    string SourceSha256, string MappingEvidence, string AnalysisStatus,
    int IndirectSites, int EligibleIndexedSites, int GuardedTableCandidates,
    int UnresolvedSites, bool Truncated,
    IReadOnlyList<Dos16GuardedDispatchSite> Sites);

/// <summary>
/// Narrow, non-game-specific proof of an unsigned BX index bound and stride:
/// cmp bx,N; ja/jae default; shl bx,log2(stride);
/// call/jmp [cs:bx+positive_disp16].
///
/// All four instructions must be adjacent along the unique decoded CFG
/// predecessors. The unsigned guard's taken edge must be a separate,
/// file-backed destination. Entries remain candidates because runtime
/// memory writes and true index values are not known.
/// </summary>
public static class Dos16AutomaticDispatchRecovery
{
    public static Dos16GuardedDispatchReport Inspect(
        Dos16CodeRegion region, Dos16FlowResult graph,
        DosMzImage? mz = null, DosMzRebasedView? simulated = null)
    {
        ArgumentNullException.ThrowIfNull(region);
        ArgumentNullException.ThrowIfNull(graph);
        if (!string.Equals(region.SourceSha256, graph.SourceSha256,
                StringComparison.OrdinalIgnoreCase) ||
            graph.MappingEvidence != region.MappingEvidence ||
            graph.FileOffset != region.FileOffset ||
            graph.RegionByteLength != region.ByteLength ||
            graph.Entry.Segment != region.Segment)
            throw new InvalidDataException("Automatic dispatch requires the exact mapped source graph.");
        if (simulated is not null && mz is null)
            throw new ArgumentException("A simulated relocation view requires its original MZ image.");
        if (mz is not null &&
            (!string.Equals(region.SourceSha256, mz.SourceSha256, StringComparison.OrdinalIgnoreCase) ||
             (simulated is not null &&
                (!string.Equals(region.SourceSha256, simulated.Summary.SourceSha256,
                    StringComparison.OrdinalIgnoreCase) ||
                 simulated.Summary.Writes.Count != mz.Relocations.Count))))
            throw new InvalidDataException("Automatic dispatch cannot borrow another source's MZ relocations.");

        var byOffset = graph.Instructions.ToDictionary(x => x.Address.Offset);
        var results = new List<Dos16GuardedDispatchSite>();
        int eligible = 0, candidates = 0;
        // The 128-table limit is visible in the report, never silent truncation.
        const int maximumSites = 128;
        var indirect = graph.Instructions
            .Where(x => x.FlowControl is "IndirectBranch" or "IndirectCall")
            .OrderBy(x => x.Address.Offset).ToArray();
        bool truncated = indirect.Length > maximumSites || graph.Truncated;

        bool FileMatches(Dos16FlowInstruction insn, params byte[] expected)
        {
            long at = (long)region.FileOffset + insn.Address.Offset - region.Origin;
            return insn.Address.Segment == region.Segment &&
                insn.Address.Offset >= region.Origin &&
                insn.Address.Offset < region.EndExclusive &&
                insn.SourceFileOffset == at &&
                insn.Length == expected.Length &&
                at >= region.FileOffset &&
                at + expected.Length <= (long)region.FileOffset + region.ByteLength &&
                insn.OriginalBytes.Equals(Convert.ToHexString(expected),
                    StringComparison.OrdinalIgnoreCase) &&
                region.Source.Data.Slice(checked((int)at), expected.Length)
                    .SequenceEqual(expected);
        }

        bool SingleIncoming(ushort destination, ushort predecessor, string kind)
        {
            var incoming = graph.Edges.Where(e => e.Destination ==
                new Dos16Address(region.Segment, destination)).ToArray();
            return incoming.Length == 1 &&
                incoming[0].Source == new Dos16Address(region.Segment, predecessor) &&
                incoming[0].Kind == kind;
        }

        foreach (var site in indirect.Take(maximumSites))
        {
            string reason = "rejected-not-exact-CS-BX-indexed-dispatch";
            string? pointerKind = null;
            int? entryCount = null;
            Dos16TableFinding? validatedTable = null;
            // FF /2,/3,/4,/5 with mod=10 r/m=111: [BX+disp16].
            if (site.Length == 5 &&
                site.Address.Offset >= region.Origin &&
                site.Address.Offset < region.EndExclusive &&
                site.SourceFileOffset ==
                    (long)region.FileOffset + site.Address.Offset - region.Origin &&
                site.SourceFileOffset + 5 <= (long)region.FileOffset + region.ByteLength)
            {
                var bytes = region.Source.Data.Slice(checked((int)site.SourceFileOffset), 5);
                byte modrm = bytes[2];
                bool exact = bytes[0] == 0x2E && bytes[1] == 0xFF &&
                    modrm is (0x97 or 0x9F or 0xA7 or 0xAF) &&
                    site.OriginalBytes.Equals(Convert.ToHexString(bytes),
                        StringComparison.OrdinalIgnoreCase) &&
                    ((modrm is 0x97 or 0x9F) == (site.FlowControl == "IndirectCall"));
                if (exact)
                {
                    eligible++;
                    bool far = modrm is 0x9F or 0xAF;
                    pointerKind = far ? "far16" : "near16";
                    int requiredShift = far ? 2 : 1;
                    ushort table = BinaryPrimitives.ReadUInt16LittleEndian(bytes.Slice(3, 2));
                    reason = "rejected-no-unique-unsigned-BX-guard-and-stride";
                    if (table <= 0x7FFF)
                    {
                        // Either SHL BX,1 or the exact immediate shift.
                        var shifts = new List<Dos16FlowInstruction>();
                        foreach (int bytesBack in new[] { 2, 3 })
                        {
                            int at = site.Address.Offset - bytesBack;
                            if (at < region.Origin || !byOffset.TryGetValue((ushort)at, out var shift))
                                continue;
                            byte[] expected = bytesBack == 2
                                ? new byte[] { 0xD1, 0xE3 }
                                : new byte[] { 0xC1, 0xE3, (byte)requiredShift };
                            if (bytesBack == 2 && requiredShift != 1) continue;
                            if (FileMatches(shift, expected) &&
                                SingleIncoming(site.Address.Offset, shift.Address.Offset, "fallthrough"))
                                shifts.Add(shift);
                        }
                        foreach (var shift in shifts)
                        {
                            if (validatedTable is not null) break;
                            // Optional exact MOV BX,r16 propagates the already
                            // guarded register value without changing flags.
                            // 89 /r or 8B /r register-register forms only;
                            // no memory, partial registers or arithmetic.
                            var transfers = new List<(Dos16FlowInstruction? Copy,
                                byte CompareModRm, string RegisterName)>
                            {
                                (null, 0xFB, "BX")
                            };
                            int copyAt = shift.Address.Offset - 2;
                            if (copyAt >= region.Origin &&
                                byOffset.TryGetValue((ushort)copyAt, out var copy))
                                foreach (var reg in new[] {
                                    (Code: 0, Name: "AX"),
                                    (Code: 1, Name: "CX"),
                                    (Code: 2, Name: "DX"),
                                    (Code: 6, Name: "SI"),
                                    (Code: 7, Name: "DI")
                                })
                                {
                                    byte form89 = (byte)(0xC3 | (reg.Code << 3));
                                    byte form8B = (byte)(0xD8 | reg.Code);
                                    if (FileMatches(copy, 0x89, form89) ||
                                        FileMatches(copy, 0x8B, form8B))
                                        transfers.Add((copy, (byte)(0xF8 + reg.Code), reg.Name));
                                }

                            foreach (var transfer in transfers)
                            {
                                if (validatedTable is not null) break;
                                ushort afterGuard = transfer.Copy?.Address.Offset ??
                                    shift.Address.Offset;
                                if (transfer.Copy is not null &&
                                    !SingleIncoming(shift.Address.Offset,
                                        transfer.Copy.Address.Offset, "fallthrough"))
                                    continue;
                                foreach (int branchBytes in new[] { 2, 4 })
                                {
                                    if (validatedTable is not null) break;
                                    int at = afterGuard - branchBytes;
                                    if (at < region.Origin ||
                                        !byOffset.TryGetValue((ushort)at, out var guard) ||
                                        guard.FlowControl != "ConditionalBranch")
                                        continue;
                                    long guardAt = (long)region.FileOffset +
                                        guard.Address.Offset - region.Origin;
                                    if (guard.SourceFileOffset != guardAt ||
                                        guardAt < region.FileOffset ||
                                        guardAt + branchBytes >
                                            (long)region.FileOffset + region.ByteLength)
                                        continue;
                                    byte[] guardBytes = region.Source.Data.Slice(
                                        checked((int)guardAt), branchBytes).ToArray();
                                    bool ja = branchBytes == 2
                                        ? guardBytes[0] == 0x77
                                        : guardBytes[0] == 0x0F && guardBytes[1] == 0x87;
                                    bool jae = branchBytes == 2
                                        ? guardBytes[0] == 0x73
                                        : guardBytes[0] == 0x0F && guardBytes[1] == 0x83;
                                    if ((!ja && !jae) || !FileMatches(guard, guardBytes) ||
                                        !SingleIncoming(afterGuard, guard.Address.Offset,
                                            "branch-not-taken") ||
                                        !graph.Edges.Any(e =>
                                            e.Source == guard.Address &&
                                            e.Kind == "branch-taken" &&
                                            e.Destination is not null &&
                                            e.Destination.Value.Offset != afterGuard &&
                                            e.Destination.Value.Offset != site.Address.Offset))
                                        continue;
                                    foreach (int compareBytes in new[] { 3, 4 })
                                    {
                                        int cmpAt = guard.Address.Offset - compareBytes;
                                        if (cmpAt < region.Origin ||
                                            !byOffset.TryGetValue((ushort)cmpAt, out var compare))
                                            continue;
                                        long cmpPhysical = (long)region.FileOffset +
                                            compare.Address.Offset - region.Origin;
                                        if (compare.SourceFileOffset != cmpPhysical ||
                                            cmpPhysical < region.FileOffset ||
                                            cmpPhysical + compareBytes >
                                                (long)region.FileOffset + region.ByteLength)
                                            continue;
                                        var cmpRaw = region.Source.Data.Slice(
                                            checked((int)cmpPhysical), compareBytes).ToArray();
                                        int max = -1;
                                        if (compareBytes == 3 &&
                                            cmpRaw[0] == 0x83 &&
                                            cmpRaw[1] == transfer.CompareModRm &&
                                            cmpRaw[2] <= 0x7F)
                                            max = cmpRaw[2];
                                        if (compareBytes == 4 &&
                                            cmpRaw[0] == 0x81 &&
                                            cmpRaw[1] == transfer.CompareModRm)
                                            max = BinaryPrimitives.ReadUInt16LittleEndian(
                                                cmpRaw.AsSpan(2));
                                        if (max < 0 || max > 255 ||
                                            !FileMatches(compare, cmpRaw) ||
                                            !SingleIncoming(guard.Address.Offset,
                                                compare.Address.Offset, "fallthrough"))
                                            continue;
                                        int count = ja ? max + 1 : max;
                                        int stride = far ? 4 : 2;
                                        if (count is < 2 or > 256 ||
                                            (long)table + (long)count * stride >
                                                region.EndExclusive ||
                                            (long)(count - 1) * stride > ushort.MaxValue)
                                            continue;
                                        entryCount = count;
                                        var manifest = new Dos16TableManifest(1,
                                            region.SourceSha256,
                                            new[] { new Dos16TableRequest(
                                                site.Address.Offset, table, count,
                                                pointerKind!,
                                                "automatic:unsigned-cmp-" +
                                                transfer.RegisterName +
                                                ";ja-or-jae;optional-mov-bx;shl-bx-stride") });
                                        var evidence = Dos16DispatchTableRecovery.Inspect(
                                            region, graph, manifest, mz, simulated).Findings[0];
                                        reason = evidence.Status ==
                                            "candidate-entries-source-backed"
                                            ? "candidate-guarded-CS-BX-table"
                                            : "rejected-guarded-table-storage-or-fixups";
                                        if (evidence.Status ==
                                            "candidate-entries-source-backed")
                                        {
                                            validatedTable = evidence;
                                            candidates++;
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    else reason = "rejected-signed-negative-displacement-not-modeled";
                }
            }
            if (graph.Truncated)
            {
                reason = "rejected-truncated-graph-cannot-exclude-bypass-paths";
                validatedTable = null;
                entryCount = null;
            }
            results.Add(new Dos16GuardedDispatchSite(site.Address, reason,
                pointerKind, entryCount,
                "locally-unique-predecessors;exact-source-instruction-bytes;" +
                "index-value-and-runtime-pointer-mutation-unverified", validatedTable));
        }
        if (graph.Truncated) candidates = 0;
        return new Dos16GuardedDispatchReport(region.SourceSha256,
            region.MappingEvidence, "candidate-unverified;unsigned-BX-guard-stride-only;" +
            "no-runtime-execution-proof;no-CFG-edges-added",
            indirect.Length, eligible, candidates, indirect.Length - candidates,
            truncated, results);
    }
}
