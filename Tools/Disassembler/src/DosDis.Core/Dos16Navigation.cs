namespace DosDis.Core;

/// <summary>
/// Safe UI navigation hint derived strictly from resolved, same-segment,
/// original-file-backed x86-16 candidate graph edges. No assembler-text
/// parsing or speculative dynamic/far target resolution.
/// </summary>
public static class Dos16Navigation
{
    public static Dos16Address? DirectTarget(
        IReadOnlyList<Dos16FlowEdge> edges, Dos16Address source)
    {
        ArgumentNullException.ThrowIfNull(edges);
        // Only explicit taken control transfers, never inferred
        // fallthrough / call-return / interrupt-return continuations.
        Dos16Address? chosen = null;
        foreach (var edge in edges)
        {
            if (edge.Source != source ||
                edge.Kind is not ("call" or "branch" or "branch-taken") ||
                edge.Destination is not { } target ||
                target.Segment != source.Segment)
                continue;
            if (chosen is { } old && old != target)
                return null; // ambiguous targets: fail closed
            chosen = target;
        }
        return chosen;
    }
}
