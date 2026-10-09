namespace DosDis.Core;

public sealed record DosMzPhysicalDecodeConflict(
    long OriginalFileOffset, Dos16Address Retained,
    Dos16Address Competing, string Status);

public sealed record DosMzFarTraversalEdge(
    Dos16Address Source, Dos16Address Destination,
    string TransferKind, string Evidence);

public sealed record DosMzCrossSegmentReview(
    string SourceSha256, ushort AssumedLoadSegment, string AnalysisStatus,
    bool Truncated, int RequestedEntryCount, int UniqueDecodedBytes,
    IReadOnlyList<Dos16FlowResult> IndependentEntryGraphs,
    IReadOnlyList<DosMzFarTraversalEdge> CrossSegmentCandidates,
    IReadOnlyList<DosMzPhysicalDecodeConflict> PhysicalConflicts);

/// <summary>
/// Performs independent, bounded candidate traversals from a resident MZ
/// entry, adding only FAR immediate targets supported by an exact 16:16
/// opcode, uniquely relocated selector and assumed loader base.
/// Never merges segment aliases or upgrades candidate edges to verified flow.
/// </summary>
public static class DosMzCrossSegmentAnalysis
{
    public static DosMzCrossSegmentReview Analyze(DosMzImage image,
        ushort assumedLoadSegment, int maxEntryGraphs = 16,
        int instructionsPerEntry = 1024)
    {
        ArgumentNullException.ThrowIfNull(image);
        if (maxEntryGraphs is < 1 or > 64)
            throw new ArgumentOutOfRangeException(nameof(maxEntryGraphs));
        if (instructionsPerEntry is < 1 or > 4096)
            throw new ArgumentOutOfRangeException(nameof(instructionsPerEntry));

        DosMzRebasedView model = image.SimulateRelocations(assumedLoadSegment);
        var initial = new Dos16Address(image.InitialCs, image.InitialIp);
        var pending = new Queue<Dos16Address>();
        var enqueued = new HashSet<Dos16Address>();
        pending.Enqueue(initial); enqueued.Add(initial);
        var graphs = new List<Dos16FlowResult>();
        var farEdges = new List<DosMzFarTraversalEdge>();
        var physicalOwners = new Dictionary<long, Dos16FlowInstruction>();
        var conflicts = new List<DosMzPhysicalDecodeConflict>();
        var conflictLocations = new HashSet<long>();
        bool truncated = false;

        while (pending.Count > 0)
        {
            if (graphs.Count >= maxEntryGraphs)
            {
                truncated = true;
                break;
            }
            Dos16Address address = pending.Dequeue();
            Dos16CodeRegion region = image.CreateCodeRegion(address.Segment);
            var graph = Dos16ControlFlowAnalysis.Analyze(region, address.Offset, instructionsPerEntry);
            graphs.Add(graph);
            truncated |= graph.Truncated;

            foreach (Dos16FlowInstruction instruction in graph.Instructions)
                for (int i = 0; i < instruction.Length; i++)
                {
                    long fileOffset = instruction.SourceFileOffset + i;
                    if (!physicalOwners.TryAdd(fileOffset, instruction))
                    {
                        var prior = physicalOwners[fileOffset];
                        bool same = prior.SourceFileOffset == instruction.SourceFileOffset &&
                            prior.Length == instruction.Length &&
                            prior.OriginalBytes == instruction.OriginalBytes;
                        if (!same && conflictLocations.Add(fileOffset) && conflicts.Count < 4096)
                            conflicts.Add(new DosMzPhysicalDecodeConflict(
                                fileOffset, prior.Address, instruction.Address,
                                "competing-original-byte-interpretations;neither-verified"));
                    }
                }

            foreach (DosMzFarCandidate candidate in
                DosMzFarTargetRecovery.Inspect(image, graph, model))
            {
                var edge = new DosMzFarTraversalEdge(candidate.Source,
                    candidate.RelativeTarget, candidate.TransferKind, candidate.Evidence);
                farEdges.Add(edge);
                // Relocated selectors and original target file backing were
                // already verified. The code there is still only a candidate.
                if (enqueued.Add(candidate.RelativeTarget))
                    pending.Enqueue(candidate.RelativeTarget);
            }
        }

        return new DosMzCrossSegmentReview(image.SourceSha256,
            assumedLoadSegment,
            "source-backed-far-candidates;independent-segment-graphs;" +
            "aliases-are-not-merged;runtime-and-relocations-assumed;unverified",
            truncated, enqueued.Count, physicalOwners.Count,
            graphs, farEdges, conflicts);
    }
}
