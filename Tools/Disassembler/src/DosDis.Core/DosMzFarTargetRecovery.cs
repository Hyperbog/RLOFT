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

        var candidates = new List<DosMzFarCandidate>();
        foreach (var instruction in graph.Instructions)
        {
            if (instruction.Length != 5) continue;
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
                !string.Equals(instruction.OriginalBytes,
                    Convert.ToHexString(stored), StringComparison.OrdinalIgnoreCase))
                continue;
            ushort targetOffset = (ushort)(stored[1] | (stored[2] << 8));
            ushort targetSelectorStored = (ushort)(stored[3] | (stored[4] << 8));
            // Any unknown/overlapping relocation in the far immediate bytes
            // would invalidate the single-selector interpretation.
            var touching = image.Relocations.Select((fixup, ordinal) =>
                (fixup, ordinal)).Where(x =>
                x.fixup.SourceFileOffset < physical + 5 &&
                x.fixup.SourceFileOffset + 2 > physical).ToArray();
            if (touching.Length != 1 || touching[0].fixup.SourceFileOffset != physical + 3)
                continue;
            DosMzRelocationWrite? applied = simulation.Summary.Writes
                .SingleOrDefault(x => x.Ordinal == touching[0].ordinal);
            if (applied is null || applied.ValueBeforeWrite != targetSelectorStored)
                continue;
            var target = simulation.MapRuntimeFarPointer(applied.ValueAfterWrite, targetOffset);
            if (target is null) continue;
            candidates.Add(new DosMzFarCandidate(instruction.Address,
                new Dos16Address(target.RelativeSegment, targetOffset),
                stored[0] == 0x9A ? "far-immediate-call" : "far-immediate-jump",
                physical, target.OriginalFileOffset, targetSelectorStored,
                applied.ValueAfterWrite, targetOffset, touching[0].ordinal,
                "exact-9A-or-EA-original-bytes;unique-MZ-selector-fixup;" +
                "assumed-load-segment;nonwrapped-file-backed-target;candidate-unverified"));
        }
        return candidates;
    }
}
