namespace DosDis.Core;

/// <summary>
/// Conservative far immediate CALL/JMP target: exact original operand bytes,
/// exactly one MZ relocation at the segment word and an assumed load base.
/// This is a candidate annotation, never an automatically followed CFG edge.
/// </summary>
public sealed record DosMzFarCandidate(
    Dos16Address Source, Dos16Address RelativeTarget,
    string TransferKind, long SourceFileOffset, long TargetFileOffset,
    ushort OriginalTargetSegment, ushort SimulatedRuntimeTargetSegment,
    ushort TargetOffset, int SelectorFixupOrdinal, string Evidence);

public static class DosMzFarTargetRecovery
{
    public static IReadOnlyList<DosMzFarCandidate> Inspect(
        DosMzImage image, Dos16FlowResult graph, DosMzRebasedView simulation)
    {
        ArgumentNullException.ThrowIfNull(image);
        ArgumentNullException.ThrowIfNull(graph);
        ArgumentNullException.ThrowIfNull(simulation);
        if (!string.Equals(image.SourceSha256, graph.SourceSha256,
                StringComparison.OrdinalIgnoreCase) ||
            !string.Equals(image.SourceSha256, simulation.Summary.SourceSha256,
                StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("Cannot combine a foreign graph, MZ image or relocated view.");

        if (simulation.Summary.Writes.Count != image.Relocations.Count)
            throw new InvalidDataException("Relocation simulation and original MZ table have different record counts.");

        // The graph's SHA alone is insufficient: the same immutable MZ
        // bytes can be analyzed in several overlapping segment windows.
        // Preserve the exact relative CS and physical load-image mapping
        // before accepting any graph row as a source address.
        var region = image.CreateCodeRegion(graph.Entry.Segment);
        if (graph.FileOffset != region.FileOffset ||
            graph.RegionByteLength != region.ByteLength ||
            graph.MappingEvidence != region.MappingEvidence ||
            graph.Entry.Offset < region.Origin ||
            graph.Entry.Offset >= region.EndExclusive ||
            graph.AnalysisStatus?.StartsWith("candidate-unverified",
                StringComparison.Ordinal) != true ||
            graph.Truncated || graph.Instructions is null ||
            graph.Edges is null || graph.Instructions.Any(x => x is null) ||
            graph.Edges.Any(x => x is null))
            throw new InvalidDataException(
                "FAR candidate graph is stale, truncated, malformed or not from the requested MZ code window.");

        // Each instruction probes at most six possible overlapping fixup
        // source offsets. Pre-index once instead of scanning the full MZ
        // relocation table for every candidate far instruction.
        var relocationOrdinals = new Dictionary<long, List<int>>();
        for (int index = 0; index < image.Relocations.Count; index++)
        {
            long start = image.Relocations[index].SourceFileOffset;
            if (!relocationOrdinals.TryGetValue(start, out var locations))
            {
                locations = new List<int>();
                relocationOrdinals.Add(start, locations);
            }
            locations.Add(index);
        }
        var candidates = new List<DosMzFarCandidate>();
        foreach (var instruction in graph.Instructions)
        {
            if (instruction.Length != 5 ||
                instruction.FlowControl is not ("Call" or "UnconditionalBranch") ||
                !Dos16UniqueTransparentPath.IsOriginalDecoded(region, instruction))
                continue;
            // A far 16:16 pointer encoded by one unprefixed opcode:
            // 9A imm16 offset,imm16 selector or EA imm16 offset,imm16 selector.
            long physical = instruction.SourceFileOffset;
            long imageOffsetLong = physical - image.LoadImageFileOffset;
            if (imageOffsetLong < 0 ||
                imageOffsetLong + 5 > image.FileBackedImageLength)
                continue;
            int imageOffset = (int)imageOffsetLong;
            byte[] stored = image.ReadStoredImageBytes(imageOffset, 5);
            if (stored[0] is not (0x9A or 0xEA) ||
                (stored[0] == 0x9A && instruction.FlowControl != "Call") ||
                (stored[0] == 0xEA && instruction.FlowControl != "UnconditionalBranch") ||
                !string.Equals(instruction.OriginalBytes,
                    Convert.ToHexString(stored), StringComparison.OrdinalIgnoreCase))
                continue;

            // The raw far instruction is still an unresolved runtime
            // transfer. Require its exact original CFG edge(s), not a
            // caller-forged direct or decoded far target annotation.
            var outgoing = graph.Edges.Where(e =>
                e.Source == instruction.Address).ToArray();
            string unresolvedKind = stored[0] == 0x9A
                ? "unresolved-far-call" : "unresolved-far-or-dynamic-jump";
            if (outgoing.Count(e => e.Kind == unresolvedKind &&
                    e.Destination is null) != 1)
                continue;
            if (stored[0] == 0xEA)
            {
                if (outgoing.Length != 1) continue;
            }
            else
            {
                int next = instruction.Address.Offset + instruction.Length;
                bool inside = next < region.EndExclusive;
                string returnKind = inside
                    ? "call-return-candidate" : "out-of-region-call-return-candidate";
                var returnAddress = inside
                    ? (Dos16Address?)new Dos16Address(region.Segment, (ushort)next)
                    : null;
                if (outgoing.Length != 2 ||
                    outgoing.Count(e => e.Kind == returnKind &&
                        e.Destination == returnAddress) != 1)
                    continue;
            }
            ushort targetOffset = (ushort)(stored[1] | (stored[2] << 8));
            ushort targetSelectorStored = (ushort)(stored[3] | (stored[4] << 8));
            // Any unknown/overlapping relocation in the far immediate bytes
            // would invalidate the single-selector interpretation.
            var touchingOrdinals = new List<int>(2);
            for (long slot = physical - 1; slot < physical + 5; slot++)
                if (relocationOrdinals.TryGetValue(slot, out var entries))
                    touchingOrdinals.AddRange(entries);
            if (touchingOrdinals.Count != 1 ||
                image.Relocations[touchingOrdinals[0]].SourceFileOffset != physical + 3)
                continue;
            int ordinal = touchingOrdinals[0];
            DosMzRelocationWrite applied = simulation.Summary.Writes[ordinal];
            if (applied.Ordinal != ordinal ||
                applied.SourceSegment != image.Relocations[ordinal].Segment ||
                applied.SourceOffset != image.Relocations[ordinal].Offset ||
                applied.OriginalFileOffset != image.Relocations[ordinal].SourceFileOffset ||
                applied.OriginalStoredWord != targetSelectorStored ||
                applied.ValueBeforeWrite != targetSelectorStored)
                continue;
            var target = simulation.MapRuntimeFarPointer(applied.ValueAfterWrite, targetOffset);
            if (target is null) continue;
            candidates.Add(new DosMzFarCandidate(instruction.Address,
                new Dos16Address(target.RelativeSegment, targetOffset),
                stored[0] == 0x9A ? "far-immediate-call" : "far-immediate-jump",
                physical, target.OriginalFileOffset, targetSelectorStored,
                applied.ValueAfterWrite, targetOffset, ordinal,
                "exact-9A-or-EA-original-bytes;unique-MZ-selector-fixup;" +
                "assumed-load-segment;nonwrapped-file-backed-target;candidate-unverified"));
        }
        return candidates;
    }
}
