using System.Buffers.Binary;
using Iced.Intel;

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
    /// <summary>
    /// Require the retained graph instruction to match the same original
    /// physical bytes, 16-bit decode length AND control-flow classification.
    /// Graph metadata alone is never permission to bypass a call, return,
    /// indirect jump or an invalid instruction. Does not assert execution.
    /// </summary>
    public static bool IsOriginalDecoded(Dos16CodeRegion region,
        Dos16FlowInstruction instruction)
    {
        ArgumentNullException.ThrowIfNull(region);
        ArgumentNullException.ThrowIfNull(instruction);
        if (instruction.Length is < 1 or > 15 ||
            instruction.Address.Segment != region.Segment ||
            instruction.Address.Offset < region.Origin ||
            (long)instruction.Address.Offset + instruction.Length >
                region.EndExclusive)
            return false;
        long physical = (long)region.FileOffset +
            instruction.Address.Offset - region.Origin;
        if (instruction.SourceFileOffset != physical ||
            physical < region.FileOffset ||
            physical + instruction.Length > (long)region.FileOffset + region.ByteLength)
            return false;
        var bytes = region.Source.Data.Slice(checked((int)physical),
            instruction.Length);
        if (!string.Equals(instruction.OriginalBytes,
                Convert.ToHexString(bytes), StringComparison.OrdinalIgnoreCase))
            return false;
        var reader = new ByteArrayCodeReader(bytes.ToArray());
        var decoder = Decoder.Create(16, reader);
        decoder.IP = instruction.Address.Offset;
        decoder.Decode(out var actual);
        return actual.Code != Code.INVALID &&
            actual.Length == instruction.Length &&
            string.Equals(instruction.FlowControl, actual.FlowControl.ToString(),
                StringComparison.Ordinal);
    }

    /// <summary>
    /// Enumerate the one and only already-decoded predecessor of a site,
    /// continuing backwards for at most a bounded number of checked NOP
    /// or direct unconditional jump bridges. A competing incoming edge
    /// terminates discovery rather than supplying a fabricated path.
    /// </summary>
    public static IReadOnlyList<Dos16FlowInstruction> UpstreamCandidates(
        Dos16CodeRegion region, Dos16FlowResult graph,
        Dos16Address site, int maximumTransparent = 4)
    {
        ArgumentNullException.ThrowIfNull(region);
        ArgumentNullException.ThrowIfNull(graph);
        if (maximumTransparent is < 0 or > 16)
            throw new ArgumentOutOfRangeException(nameof(maximumTransparent));
        if (!MatchesRegion(region, graph) || site.Segment != region.Segment)
            return Array.Empty<Dos16FlowInstruction>();
        var known = UniqueInstructions(graph);
        if (known is null || !known.ContainsKey(site))
            return Array.Empty<Dos16FlowInstruction>();
        var found = new List<Dos16FlowInstruction>();
        var visited = new HashSet<Dos16Address> { site };
        var cursor = site;
        for (int i = 0; i <= maximumTransparent; i++)
        {
            if (cursor == graph.Entry) break;
            var incoming = graph.Edges.Where(x => x.Destination == cursor)
                .Take(2).ToArray();
            if (incoming.Length != 1 || !visited.Add(incoming[0].Source) ||
                !known.TryGetValue(incoming[0].Source, out var prior))
                break;
            found.Add(prior);
            if (i == maximumTransparent || prior.Address == graph.Entry ||
                !Transparent(region, graph, prior, incoming[0], cursor))
                break;
            cursor = prior.Address;
        }
        return found;
    }

    /// <summary>
    /// A graph with two interpretations starting at the same address is
    /// ambiguous. Return an unresolved candidate instead of allowing
    /// ToDictionary to throw while processing externally edited snapshots.
    /// </summary>
    private static Dictionary<Dos16Address, Dos16FlowInstruction>? UniqueInstructions(
        Dos16FlowResult graph)
    {
        var found = new Dictionary<Dos16Address, Dos16FlowInstruction>();
        foreach (var instruction in graph.Instructions)
            if (!found.TryAdd(instruction.Address, instruction))
                return null;
        return found;
    }

    private static bool MatchesRegion(Dos16CodeRegion region, Dos16FlowResult graph) =>
        graph.Entry.Segment == region.Segment &&
        graph.FileOffset == region.FileOffset &&
        graph.RegionByteLength == region.ByteLength &&
        string.Equals(region.SourceSha256, graph.SourceSha256,
            StringComparison.OrdinalIgnoreCase) &&
        graph.MappingEvidence == region.MappingEvidence &&
        !graph.Truncated;

    private static bool Transparent(Dos16CodeRegion region, Dos16FlowResult graph,
        Dos16FlowInstruction insn, Dos16FlowEdge edge, Dos16Address target)
    {
        if (edge.Destination != target || insn.Address == graph.Entry ||
            insn.Length is not (1 or 2 or 3))
            return false;
        long at = (long)region.FileOffset + insn.Address.Offset - region.Origin;
        if (insn.Address.Segment != region.Segment ||
            insn.Address.Offset < region.Origin ||
            insn.Address.Offset >= region.EndExclusive ||
            insn.SourceFileOffset != at ||
            at < region.FileOffset ||
            at + insn.Length > (long)region.FileOffset + region.ByteLength)
            return false;
        var bytes = region.Source.Data.Slice(checked((int)at), insn.Length);
        if (!IsOriginalDecoded(region, insn))
            return false;
        if (insn.Length == 1)
            return bytes[0] == 0x90 && edge.Kind == "fallthrough" &&
                insn.Address.Offset + 1 == target.Offset;
        if (edge.Kind != "branch") return false;
        int displacement;
        if (insn.Length == 2 && bytes[0] == 0xEB)
            displacement = unchecked((sbyte)bytes[1]);
        else if (insn.Length == 3 && bytes[0] == 0xE9)
            displacement = BinaryPrimitives.ReadInt16LittleEndian(bytes.Slice(1, 2));
        else return false;
        int expected = insn.Address.Offset + insn.Length + displacement;
        return expected >= region.Origin &&
            expected < region.EndExclusive && expected == target.Offset;
    }

    /// <summary>
    /// Verify a direct edge from its original instruction bytes, not only
    /// the editable graph's Kind field. This also forbids treating an
    /// independently seeded graph entry as a uniquely reached successor.
    /// </summary>
    private static bool DirectEdgeMatches(Dos16CodeRegion region,
        Dos16FlowInstruction source, Dos16Address destination, string kind)
    {
        if (source.Address.Segment != region.Segment ||
            destination.Segment != region.Segment ||
            source.Address.Offset < region.Origin ||
            source.Address.Offset >= region.EndExclusive ||
            source.Length is < 1 or > 15)
            return false;
        long physical = (long)region.FileOffset +
            source.Address.Offset - region.Origin;
        if (source.SourceFileOffset != physical ||
            physical < region.FileOffset ||
            physical + source.Length > (long)region.FileOffset + region.ByteLength)
            return false;
        var raw = region.Source.Data.Slice(checked((int)physical), source.Length);
        if (!IsOriginalDecoded(region, source))
            return false;
        int next = source.Address.Offset + source.Length;
        if (kind == "fallthrough")
            return destination.Offset == next &&
                source.FlowControl == "Next";
        if (kind == "branch-not-taken")
            return source.FlowControl == "ConditionalBranch" &&
                destination.Offset == next;
        if (kind != "branch-taken" ||
            source.FlowControl != "ConditionalBranch") return false;

        int displacement;
        if (source.Length == 2 && raw[0] is >= 0x70 and <= 0x7F)
            displacement = unchecked((sbyte)raw[1]);
        else if (source.Length == 4 && raw[0] == 0x0F &&
                 raw[1] is >= 0x80 and <= 0x8F)
            displacement = BinaryPrimitives.ReadInt16LittleEndian(raw.Slice(2, 2));
        else return false;
        int expected = next + displacement;
        return expected >= region.Origin &&
            expected < region.EndExclusive && destination.Offset == expected;
    }

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
            !MatchesRegion(region, graph))
            return false;
        var known = UniqueInstructions(graph);
        if (known is null ||
            !known.ContainsKey(predecessor) || !known.ContainsKey(successor) ||
            (successor == graph.Entry && predecessor != successor))
            return false;

        var visited = new HashSet<Dos16Address> { successor };
        var cursor = successor;
        for (int i = 0; i <= maximumIntermediate; i++)
        {
            if (cursor == graph.Entry) return false;
            // Exactly one incoming graph edge is essential: any alternate
            // predecessor can bypass the register guard or stride.
            var incoming = graph.Edges.Where(e => e.Destination == cursor)
                .Take(2).ToArray();
            if (incoming.Length != 1) return false;
            var edge = incoming[0];
            if (edge.Source == predecessor)
                return edge.Kind == sourceEdgeKind &&
                    DirectEdgeMatches(region, known[predecessor],
                        cursor, sourceEdgeKind);
            if (i == maximumIntermediate || !visited.Add(edge.Source) ||
                !known.TryGetValue(edge.Source, out var insn) ||
                !Transparent(region, graph, insn, edge, cursor))
                return false;
            cursor = edge.Source;
        }
        return false;
    }
}
