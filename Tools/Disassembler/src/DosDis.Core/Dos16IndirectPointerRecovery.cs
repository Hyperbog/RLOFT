using System.Buffers.Binary;

namespace DosDis.Core;

public sealed record Dos16IndirectPointerCandidate(
    Dos16Address Source, Dos16Address Target, string TransferKind,
    long InstructionFileOffset, long PointerFileOffset, long TargetFileOffset,
    int PointerByteLength, int? SelectorFixupOrdinal, string Evidence);

public sealed record Dos16IndirectSite(
    Dos16Address Source, string TransferKind, string ResolutionStatus,
    long InstructionFileOffset, string Evidence);

public sealed record Dos16IndirectReport(
    string SourceSha256, string MappingEvidence, string AnalysisStatus,
    int IndirectSites, int CandidateSites, int UnresolvedSites,
    IReadOnlyList<Dos16IndirectPointerCandidate> Candidates,
    IReadOnlyList<Dos16IndirectSite> Sites);

/// <summary>
/// Small, provable subset of indirect x86-16 transfers: the unprefixed
/// CS override plus FF /2,/3,/4,/5, mod=00 r/m=110 disp16 addressing.
/// The CS selector must be explicit. Indexed, register and DS-dependent
/// operations remain unresolved. Nothing is injected into a CFG.
/// </summary>
public static class Dos16IndirectPointerRecovery
{
    public static Dos16IndirectReport Inspect(Dos16CodeRegion region,
        Dos16FlowResult graph, DosMzImage? mz = null, DosMzRebasedView? simulation = null)
    {
        ArgumentNullException.ThrowIfNull(region);
        ArgumentNullException.ThrowIfNull(graph);
        if (!string.Equals(region.SourceSha256, graph.SourceSha256, StringComparison.OrdinalIgnoreCase) ||
            graph.FileOffset != region.FileOffset ||
            graph.RegionByteLength != region.ByteLength ||
            graph.MappingEvidence != region.MappingEvidence ||
            graph.Entry.Segment != region.Segment)
            throw new InvalidDataException("Indirect recovery requires the exact source and decoded region.");
        if ((mz is null) != (simulation is null))
            throw new ArgumentException("MZ image and its relocated copy must be supplied together.");
        if (mz is not null &&
            (!string.Equals(region.SourceSha256, mz.SourceSha256, StringComparison.OrdinalIgnoreCase) ||
             !string.Equals(region.SourceSha256, simulation!.Summary.SourceSha256, StringComparison.OrdinalIgnoreCase) ||
             simulation.Summary.Writes.Count != mz.Relocations.Count))
            throw new InvalidDataException("MZ selector relocation evidence belongs to a different source.");

        var indexedRelocations = new Dictionary<long, List<int>>();
        if (mz is not null)
            for (int ordinal = 0; ordinal < mz.Relocations.Count; ordinal++)
            {
                long start = mz.Relocations[ordinal].SourceFileOffset;
                if (!indexedRelocations.TryGetValue(start, out var indices))
                {
                    indices = new List<int>();
                    indexedRelocations.Add(start, indices);
                }
                indices.Add(ordinal);
            }

        // A two-byte relocation word overlaps [physical,physical+size) if
        // it starts at physical-1 through physical+size-1 inclusive.
        List<int> Touching(long physical, int size)
        {
            var ordinals = new List<int>();
            for (long loc = physical - 1; loc < physical + size; loc++)
                if (indexedRelocations.TryGetValue(loc, out var matches))
                    ordinals.AddRange(matches);
            return ordinals;
        }

        var sites = new List<Dos16IndirectSite>();
        var candidates = new List<Dos16IndirectPointerCandidate>();
        foreach (var instruction in graph.Instructions)
        {
            if (instruction.FlowControl is not ("IndirectCall" or "IndirectBranch"))
                continue;
            string kind = instruction.FlowControl == "IndirectCall" ? "indirect-call" : "indirect-jump";
            string status = "unresolved-other-addressing";
            string evidence = "register-indexed-or-segment-state-not-statically-established";
            long physical = instruction.SourceFileOffset;
            if (instruction.Length == 5 &&
                (long)physical >= region.FileOffset &&
                physical + 5 <= (long)region.FileOffset + region.ByteLength)
            {
                var bytes = region.Source.Data.Slice(checked((int)physical), 5);
                // Accept only the precise encoded instruction form; neither
                // operand-size nor address-size prefixes are allowed.
                byte selector = bytes[2];
                bool near = selector is 0x16 or 0x26;
                bool far = selector is 0x1E or 0x2E;
                bool appropriate = kind == "indirect-call"
                    ? selector is 0x16 or 0x1E
                    : selector is 0x26 or 0x2E;
                if (bytes[0] == 0x2E && bytes[1] == 0xFF && appropriate)
                {
                    int pointerOffset = BinaryPrimitives.ReadUInt16LittleEndian(bytes.Slice(3, 2));
                    int pointerSize = near ? 2 : 4;
                    if (pointerOffset >= region.Origin &&
                        (long)pointerOffset + pointerSize <= region.EndExclusive)
                    {
                        long pointerFile = region.FileOffset + pointerOffset - region.Origin;
                        byte[] payload = region.Source.Data.Slice(
                            checked((int)pointerFile), pointerSize).ToArray();
                        var fixups = Touching(pointerFile, pointerSize);
                        ushort targetOffset = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2));
                        if (near && fixups.Count == 0 &&
                            targetOffset >= region.Origin && targetOffset < region.EndExclusive)
                        {
                            long targetFile = region.FileOffset + targetOffset - region.Origin;
                            candidates.Add(new Dos16IndirectPointerCandidate(
                                instruction.Address,
                                new Dos16Address(region.Segment, targetOffset),
                                kind + "-cs-near-pointer",
                                physical, pointerFile, targetFile, pointerSize, null,
                                "exact-CS-FF-absolute-disp16;unmodified-stored-near-word;" +
                                "same-source-backed-segment;runtime-pointer-not-verified"));
                            status = "candidate-cs-near-pointer";
                            evidence = "stored-word-candidate;runtime-writes-not-proven-absent";
                        }
                        else if (far && mz is not null &&
                            fixups.Count == 1 &&
                            mz.Relocations[fixups[0]].SourceFileOffset == pointerFile + 2)
                        {
                            int ordinal = fixups[0];
                            var applied = simulation!.Summary.Writes[ordinal];
                            ushort originalSelector = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(2, 2));
                            if (applied.Ordinal == ordinal &&
                                applied.ValueBeforeWrite == originalSelector)
                            {
                                var target = simulation.MapRuntimeFarPointer(
                                    applied.ValueAfterWrite, targetOffset);
                                if (target is not null)
                                {
                                    candidates.Add(new Dos16IndirectPointerCandidate(
                                        instruction.Address,
                                        new Dos16Address(target.RelativeSegment, targetOffset),
                                        kind + "-cs-far-pointer",
                                        physical, pointerFile, target.OriginalFileOffset,
                                        pointerSize, ordinal,
                                        "exact-CS-FF-absolute-disp16;unique-MZ-selector-fixup;" +
                                        "assumed-load-segment;source-backed-far-target;unverified"));
                                    status = "candidate-cs-far-pointer";
                                    evidence = "original-memory-pointer-and-unique-MZ-fixup";
                                }
                            }
                        }
                        if (status.StartsWith("unresolved-", StringComparison.Ordinal))
                        {
                            status = fixups.Count > 0 ? "unresolved-pointer-fixup-or-target"
                                : "unresolved-pointer-target-or-load-state";
                            evidence = "pointer is not source-backed under validated relocation/bounds rules";
                        }
                    }
                    else
                    {
                        status = "unresolved-pointer-outside-mapped-region";
                        evidence = "CS pointer storage has no validated source-backed region";
                    }
                }
            }
            sites.Add(new Dos16IndirectSite(instruction.Address, kind,
                status, physical, evidence));
        }
        return new Dos16IndirectReport(region.SourceSha256, region.MappingEvidence,
            "candidate-unverified;no-unresolved-CFG-edge-replacement",
            sites.Count, candidates.Count, sites.Count - candidates.Count,
            candidates, sites);
    }
}
