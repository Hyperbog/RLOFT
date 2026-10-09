using System.Buffers.Binary;

namespace DosDis.Core;

/// <summary>
/// Source-backed proof of a unique sequence of instructions that neither
/// alters registers nor flags: NOP, short JMP, or near16 JMP. Bounded
/// backwards traversal uses the already decoded graph and refuses joins,
/// dynamic calls, invalid file offsets and unaccounted graph-entry seeds.
///
/// This is a local candidate-flow proof, not a global dominance analysis.
/// </summary>
public static class Dos16UniqueTransparentPath
{
    public static bool Connects(Dos16CodeRegion region, Dos16FlowResult graph,
        Dos16Address predecessor, Dos16Address successor, string sourceEdgeKind,
        int maximumIntermediate = 4)
    {
        ArgumentNullException.ThrowIfNull(region);
        ArgumentNullException.ThrowIfNull(graph);
        if (maximumIntermediate is < 0 or > 16)
            throw new ArgumentOutOfRangeException(nameof(maximumIntermediate));
        if (predecessor.Segment != region.Segment ||
            successor.Segment != region.Segment ||
            graph.Entry.Segment != region.Segment ||
            graph.FileOffset != region.FileOffset ||
            graph.RegionByteLength != region.ByteLength ||
            !string.Equals(region.SourceSha256, graph.SourceSha256,
                StringComparison.OrdinalIgnoreCase) ||
            graph.MappingEvidence != region.MappingEvidence ||
            graph.Truncated)
            return false;

        var known = graph.Instructions.ToDictionary(i => i.Address);
        if (!known.ContainsKey(predecessor) || !known.ContainsKey(successor))
            return false;

        bool OriginalBytes(Dos16FlowInstruction insn, ReadOnlySpan<byte> encoded)
        {
            long physical = (long)region.FileOffset +
                insn.Address.Offset - region.Origin;
            return insn.Address.Segment == region.Segment &&
                insn.Address.Offset >= region.Origin &&
                insn.Address.Offset < region.EndExclusive &&
                insn.SourceFileOffset == physical &&
                insn.Length == encoded.Length &&
                physical >= region.FileOffset &&
                physical + encoded.Length <=
                    (long)region.FileOffset + region.ByteLength &&
                string.Equals(insn.OriginalBytes,
                    Convert.ToHexString(encoded), StringComparison.OrdinalIgnoreCase) &&
                region.Source.Data.Slice(checked((int)physical), encoded.Length)
                    .SequenceEqual(encoded);
        }

        bool Transparent(Dos16FlowInstruction insn, Dos16FlowEdge edge,
            Dos16Address target)
        {
            if (edge.Destination != target || insn.Address == graph.Entry)
                return false;
            if (insn.Length is not (1 or 2 or 3)) return false;
            long physical = insn.SourceFileOffset;
            if (physical < region.FileOffset ||
                physical + insn.Length >
                    (long)region.FileOffset + region.ByteLength)
                return false;
            var bytes = region.Source.Data.Slice(checked((int)physical), insn.Length);
            if (!OriginalBytes(insn, bytes)) return false;
            if (insn.Length == 1)
                return bytes[0] == 0x90 && edge.Kind == "fallthrough" &&
                    insn.Address.Offset + 1 == target.Offset;
            if (edge.Kind != "branch") return false;

            int signedDisplacement;
            if (insn.Length == 2 && bytes[0] == 0xEB)
                signedDisplacement = unchecked((sbyte)bytes[1]);
            else if (insn.Length == 3 && bytes[0] == 0xE9)
                signedDisplacement = BinaryPrimitives.ReadInt16LittleEndian(bytes.Slice(1, 2));
            else return false;
            int expectedTarget = insn.Address.Offset + insn.Length + signedDisplacement;
            // An IP-wrap dependence is out of scope for this strict proof.
            return expectedTarget >= region.Origin &&
                expectedTarget < region.EndExclusive &&
                expectedTarget == target.Offset;
        }

        var visited = new HashSet<Dos16Address> { successor };
        var cursor = successor;
        for (int i = 0; i <= maximumIntermediate; i++)
        {
            // Exactly one incoming graph edge is essential: any alternate
            // predecessor can bypass the register guard or stride.
            var incoming = graph.Edges.Where(e => e.Destination == cursor)
                .Take(2).ToArray();
            if (incoming.Length != 1) return false;
            var edge = incoming[0];
            if (edge.Source == predecessor)
                return edge.Kind == sourceEdgeKind;
            if (i == maximumIntermediate || !visited.Add(edge.Source) ||
                !known.TryGetValue(edge.Source, out var insn) ||
                !Transparent(insn, edge, cursor))
                return false;
            cursor = edge.Source;
        }
        return false;
    }
}
