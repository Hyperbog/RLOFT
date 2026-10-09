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
/// Each link must have unique, source-backed predecessor evidence; a
/// bounded chain of NOP/near-JMP bridges is allowed. JA/JAE rejection
/// guards select the fallthrough, and JB/JBE acceptance guards select
/// the taken path. The opposite edge must remain a distinct mapped
/// candidate. Entries stay unverified because runtime pointer writes
/// and actual index values are not known.
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
                        foreach (var shift in Dos16UniqueTransparentPath.UpstreamCandidates(
                            region, graph, site.Address))
                        {
                            // Both ADD BX,BX register forms double BX
                            // modulo 16 bits. The guarded <=255 entry range
                            // precludes overflow for near 2-byte pointers.
                            // No flags are consumed after the scaling step.
                            bool exactStride = requiredShift == 1 &&
                                (FileMatches(shift, 0xD1, 0xE3) ||
                                 FileMatches(shift, 0x01, 0xDB) ||
                                 FileMatches(shift, 0x03, 0xDB));
                            exactStride |= FileMatches(shift, 0xC1, 0xE3,
                                (byte)requiredShift);
                            if (exactStride && Dos16UniqueTransparentPath.Connects(
                                region, graph, shift.Address, site.Address, "fallthrough"))
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
                            foreach (var copy in Dos16UniqueTransparentPath.UpstreamCandidates(
                                region, graph, shift.Address))
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
                                    if ((FileMatches(copy, 0x89, form89) ||
                                        FileMatches(copy, 0x8B, form8B)) &&
                                        Dos16UniqueTransparentPath.Connects(
                                            region, graph, copy.Address, shift.Address,
                                            "fallthrough"))
                                        transfers.Add((copy, (byte)(0xF8 + reg.Code), reg.Name));
                                }

                            foreach (var transfer in transfers)
                            {
                                if (validatedTable is not null) break;
                                ushort afterGuard = transfer.Copy?.Address.Offset ??
                                    shift.Address.Offset;
                                foreach (var guard in Dos16UniqueTransparentPath.UpstreamCandidates(
                                    region, graph, new Dos16Address(region.Segment, afterGuard)))
                                {
                                    if (validatedTable is not null) break;
                                    int branchBytes = guard.Length;
                                    if (branchBytes is not (2 or 4) ||
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
                                    // Reject-outside guards JA/JAE feed the
                                    // bounded fallthrough. Select-inside
                                    // guards JB/JBE feed the bounded taken
                                    // edge. These are not interchangeable:
                                    // unsigned less-than excludes the CMP
                                    // immediate, while below-or-equal includes
                                    // it. Signed conditions remain unsupported.
                                    bool ja = branchBytes == 2
                                        ? guardBytes[0] == 0x77
                                        : guardBytes[0] == 0x0F && guardBytes[1] == 0x87;
                                    bool jae = branchBytes == 2
                                        ? guardBytes[0] == 0x73
                                        : guardBytes[0] == 0x0F && guardBytes[1] == 0x83;
                                    bool jb = branchBytes == 2
                                        ? guardBytes[0] == 0x72
                                        : guardBytes[0] == 0x0F && guardBytes[1] == 0x82;
                                    bool jbe = branchBytes == 2
                                        ? guardBytes[0] == 0x76
                                        : guardBytes[0] == 0x0F && guardBytes[1] == 0x86;
                                    if ((!ja && !jae && !jb && !jbe) ||
                                        !FileMatches(guard, guardBytes))
                                        continue;
                                    bool insideTaken = jb || jbe;
                                    string acceptedKind = insideTaken
                                        ? "branch-taken" : "branch-not-taken";
                                    int nextIp = guard.Address.Offset + branchBytes;
                                    int delta = branchBytes == 2
                                        ? unchecked((sbyte)guardBytes[1])
                                        : BinaryPrimitives.ReadInt16LittleEndian(
                                            guardBytes.AsSpan(2, 2));
                                    int takenIp = nextIp + delta;
                                    // Prove both actual outgoing edges from
                                    // their stored relative displacement, not
                                    // merely by trusting a mutable CFG label.
                                    if (nextIp >= region.EndExclusive ||
                                        takenIp < region.Origin ||
                                        takenIp >= region.EndExclusive ||
                                        nextIp == takenIp)
                                        continue;
                                    var takenTarget = new Dos16Address(
                                        region.Segment, (ushort)takenIp);
                                    var notTakenTarget = new Dos16Address(
                                        region.Segment, (ushort)nextIp);
                                    var outgoing = graph.Edges.Where(e =>
                                        e.Source == guard.Address).ToArray();
                                    if (outgoing.Length != 2 ||
                                        outgoing.Count(e =>
                                            e.Kind == "branch-taken" &&
                                            e.Destination == takenTarget) != 1 ||
                                        outgoing.Count(e =>
                                            e.Kind == "branch-not-taken" &&
                                            e.Destination == notTakenTarget) != 1 ||
                                        !graph.Instructions.Any(i =>
                                            i.Address == (insideTaken
                                                ? notTakenTarget : takenTarget)))
                                        continue;
                                    var exit = insideTaken ? notTakenTarget : takenTarget;
                                    if (exit.Offset == afterGuard ||
                                        exit.Offset == site.Address.Offset ||
                                        !Dos16UniqueTransparentPath.Connects(
                                            region, graph, guard.Address,
                                            new Dos16Address(region.Segment, afterGuard),
                                            acceptedKind))
                                        continue;
                                    foreach (var compare in Dos16UniqueTransparentPath.UpstreamCandidates(
                                        region, graph, guard.Address))
                                    {
                                        int compareBytes = compare.Length;
                                        if (compareBytes is not (3 or 4))
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
                                        // 3D iw is the accumulator-specific
                                        // CMP AX,imm16 encoding. It must be
                                        // paired with the proven AX->BX copy;
                                        // no high-half or 8-bit register
                                        // truncation is accepted.
                                        if (compareBytes == 3 &&
                                            transfer.RegisterName == "AX" &&
                                            cmpRaw[0] == 0x3D)
                                            max = BinaryPrimitives.ReadUInt16LittleEndian(
                                                cmpRaw.AsSpan(1, 2));
                                        if (max < 0 || max > 255 ||
                                            !FileMatches(compare, cmpRaw) ||
                                            !Dos16UniqueTransparentPath.Connects(
                                                region, graph, compare.Address, guard.Address,
                                                "fallthrough"))
                                            continue;
                                        int count = (ja || jbe) ? max + 1 : max;
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
                                                ";unsigned-ja-jae-jb-jbe;optional-mov-bx;verified-bx-stride") });
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
                "bounded-unique-predecessors;transparent-NOP-or-near-JMP-bridges;" +
            "exact-source-instruction-bytes;" +
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
