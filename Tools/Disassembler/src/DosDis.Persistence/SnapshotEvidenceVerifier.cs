using DosDis.Core;

namespace DosDis.Persistence;

/// <summary>
/// Checks a saved analysis against a separately opened, SHA-256-identified
/// original LE executable. This is an integrity check of cached evidence,
/// NOT proof that the candidate instruction classification is correct.
/// </summary>
public static class SnapshotEvidenceVerifier
{
    public static void Verify(FlowAnalysisResult graph, LeImage original)
    {
        ArgumentNullException.ThrowIfNull(graph);
        ArgumentNullException.ThrowIfNull(original);
        if (!string.Equals(graph.ArtifactSha256, original.ArtifactSha256,
            StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("Saved analysis belongs to a different original binary.");

        var starts = new HashSet<FlowAddress>();
        foreach (FlowInstruction? instruction in graph.Instructions)
        {
            if (instruction is null || instruction.Address.Object < 1 ||
                instruction.Address.Object > original.Objects.Count ||
                instruction.Length is < 1 or > 15)
                throw new InvalidDataException("Saved analysis has an invalid instruction location or length.");

            LeObject obj = original.Objects[instruction.Address.Object - 1];
            if ((obj.Flags & 4u) == 0 ||
                (ulong)instruction.Address.Offset + (uint)instruction.Length > obj.VirtualSize ||
                !starts.Add(instruction.Address))
                throw new InvalidDataException("Saved instruction overlaps object bounds, has invalid ownership, or repeats an address.");

            if (instruction.OriginalBytes is null ||
                instruction.OriginalBytes.Length != instruction.Length * 2)
                throw new InvalidDataException("Saved instruction has an invalid original-byte encoding.");

            byte[] originalBytes;
            try
            {
                originalBytes = original.ReadStoredBytes(instruction.Address.Object,
                    instruction.Address.Offset, instruction.Length);
            }
            catch (Exception error) when (error is ArgumentException or InvalidDataException or OverflowException)
            {
                throw new InvalidDataException("Saved instruction refers to unavailable original-file bytes.", error);
            }
            if (!string.Equals(instruction.OriginalBytes,
                Convert.ToHexString(originalBytes), StringComparison.OrdinalIgnoreCase))
                throw new InvalidDataException($"Saved instruction evidence differs from the original at " +
                    $"{instruction.Address.Object}:0x{instruction.Address.Offset:X}.");
        }
    }
}
