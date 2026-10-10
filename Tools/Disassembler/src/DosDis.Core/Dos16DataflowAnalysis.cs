using System.Buffers.Binary;
using Iced.Intel;

namespace DosDis.Core;

public sealed record Dos16DataflowState(Dos16Address Address,
    Dos16RegisterSnapshot Before, Dos16RegisterSnapshot After);

public sealed record Dos16DataflowReport(string SourceSha256,
    string MappingEvidence, string AnalysisStatus, bool Incomplete,
    int ProcessedSteps, int MergeChanges, int WidenedRegisterFacts,
    int UnresolvedEdges, IReadOnlyList<Dos16DataflowState> States);

/// <summary>
/// Bounded, source-backed x86-16 candidate register propagation.
/// Never changes the CFG, promotes table targets, infers branch flags,
/// or claims a candidate instruction actually executes.
/// </summary>
public static class Dos16DataflowAnalysis
{
    public static Dos16DataflowReport Analyze(Dos16CodeRegion region,
        Dos16FlowResult graph, int maximumInstructions = 20_000,
        int maximumSteps = 100_000, int maximumEdges = 80_000)
    {
        ArgumentNullException.ThrowIfNull(region);
        ArgumentNullException.ThrowIfNull(graph);
        if (maximumInstructions is < 1 or > 100_000 ||
            maximumSteps is < 1 or > 1_000_000 ||
            maximumEdges is < 1 or > 400_000)
            throw new ArgumentOutOfRangeException(nameof(maximumInstructions),
                "Dataflow budgets must be positive and bounded.");

        Dos16DataflowReport Incomplete(string status, int steps = 0,
            int merges = 0, int widened = 0, int unresolved = 0) =>
            new(region.SourceSha256, region.MappingEvidence, status, true,
                steps, merges, widened, unresolved, Array.Empty<Dos16DataflowState>());

        if (graph.Instructions is null || graph.Edges is null ||
            graph.Instructions.Any(x => x is null) ||
            graph.Edges.Any(x => x is null))
            return Incomplete("rejected-malformed-graph");
        if (graph.Truncated)
            return Incomplete("incomplete-input-graph-truncated");
        if (graph.Instructions.Count > maximumInstructions ||
            graph.Edges.Count > maximumEdges)
            return Incomplete("incomplete-input-graph-budget");
        if (!string.Equals(graph.SourceSha256, region.SourceSha256,
                StringComparison.OrdinalIgnoreCase) ||
            graph.MappingEvidence != region.MappingEvidence ||
            graph.FileOffset != region.FileOffset ||
            graph.RegionByteLength != region.ByteLength ||
            graph.Entry.Segment != region.Segment ||
            graph.Entry.Offset < region.Origin ||
            graph.Entry.Offset >= region.EndExclusive ||
            graph.AnalysisStatus?.StartsWith("candidate-unverified",
                StringComparison.Ordinal) != true)
            return Incomplete("rejected-source-or-mapping-mismatch");

        var instructions = new Dictionary<Dos16Address, Dos16FlowInstruction>();
        int previousEnd = region.Origin;
        foreach (var candidate in graph.Instructions.OrderBy(x => x.Address.Offset))
        {
            if (candidate.Address.Segment != region.Segment ||
                candidate.Address.Offset < previousEnd ||
                !instructions.TryAdd(candidate.Address, candidate) ||
                !Dos16UniqueTransparentPath.IsOriginalDecoded(region, candidate))
                return Incomplete("rejected-duplicate-overlap-or-stale-instruction");
            previousEnd = candidate.Address.Offset + candidate.Length;
        }
        // These cached coverage totals are descriptive rather than
        // executable proof, but they must agree with the independently
        // checked nonoverlapping decoded byte spans. A stale summary
        // cannot accompany a trusted register-state report.
        long decodedBytes = 0;
        foreach (var instruction in instructions.Values)
            decodedBytes += instruction.Length;
        if (decodedBytes != graph.CandidateInstructionBytes ||
            (long)region.ByteLength - decodedBytes != graph.UnknownFileBackedBytes)
            return Incomplete("rejected-inconsistent-byte-coverage");
        if (!instructions.ContainsKey(graph.Entry))
            return Incomplete("rejected-graph-entry-not-decoded");
        if (graph.Edges.Any(edge => !instructions.ContainsKey(edge.Source)))
            return Incomplete("rejected-orphan-edge");

        // Independently decode every source instruction to reconstruct all
        // mandatory edges, including candidate fallthrough and unresolved
        // out-of-region edges. Missing, duplicated or forged graph edges
        // invalidate the whole value report, never only one predecessor.
        var edgesBySource = graph.Edges.GroupBy(x => x.Source)
            .ToDictionary(x => x.Key, x => x.ToArray());
        var outgoing = new Dictionary<Dos16Address, Dos16Address[]>();
        int unresolvedEdges = 0;
        foreach (var candidate in instructions.Values)
        {
            byte[] raw = region.Source.Data.Slice(
                checked((int)candidate.SourceFileOffset), candidate.Length).ToArray();
            var decoder = Decoder.Create(16, new ByteArrayCodeReader(raw));
            decoder.IP = candidate.Address.Offset;
            decoder.Decode(out var decoded);
            if (decoded.Code == Code.INVALID || decoded.Length != candidate.Length)
                return Incomplete("rejected-independent-decode-mismatch");

            // Iced expresses 16-bit near destinations after applying the
            // CPU's IP mask. Do not mistake a wrapped runtime branch for
            // a proven in-region source edge. Derive displacement again
            // from exact original bytes, independently of the CFG cache.
            int? displacement = null;
            if (raw.Length == 2 &&
                (raw[0] is >= 0x70 and <= 0x7F or >= 0xE0 and <= 0xE3
                    or 0xEB))
                displacement = unchecked((sbyte)raw[1]);
            else if (raw.Length == 3 && raw[0] is 0xE8 or 0xE9)
                displacement = BinaryPrimitives.ReadInt16LittleEndian(raw.AsSpan(1, 2));
            else if (raw.Length == 4 && raw[0] == 0x0F &&
                     raw[1] is >= 0x80 and <= 0x8F)
                displacement = BinaryPrimitives.ReadInt16LittleEndian(raw.AsSpan(2, 2));
            if (displacement is int signedDisplacement)
            {
                long unwrapped = (long)candidate.Address.Offset +
                    candidate.Length + signedDisplacement;
                if (unwrapped < 0 || unwrapped > ushort.MaxValue)
                    return Incomplete("rejected-wrapped-near16-relative-target");
            }

            int after = candidate.Address.Offset + candidate.Length;
            bool near = decoded.Op0Kind is OpKind.NearBranch16 or OpKind.NearBranch32;
            ulong target = decoded.NearBranchTarget;
            int? direct = near && target <= ushort.MaxValue ? (int)target : null;
            var expected = new List<(string Kind, Dos16Address? Destination)>();

            void Target(int destination, string kind)
            {
                if (destination < region.Origin || destination >= region.EndExclusive)
                    expected.Add(("out-of-region-" + kind, null));
                else
                    expected.Add((kind, new Dos16Address(region.Segment,
                        checked((ushort)destination))));
            }

            void Unresolved(string kind) => expected.Add((kind, null));

            switch (decoded.FlowControl)
            {
                case FlowControl.UnconditionalBranch:
                    if (direct is int branch) Target(branch, "branch");
                    else Unresolved("unresolved-far-or-dynamic-jump");
                    break;
                case FlowControl.ConditionalBranch:
                    if (direct is int conditional) Target(conditional, "branch-taken");
                    else Unresolved("unresolved-conditional-target");
                    Target(after, "branch-not-taken");
                    break;
                case FlowControl.Call:
                    if (direct is int call) Target(call, "call");
                    else Unresolved("unresolved-far-call");
                    Target(after, "call-return-candidate");
                    break;
                case FlowControl.IndirectBranch:
                    Unresolved("unresolved-indirect-jump");
                    break;
                case FlowControl.IndirectCall:
                    Unresolved("unresolved-indirect-call");
                    Target(after, "call-return-candidate");
                    break;
                case FlowControl.Interrupt:
                    Unresolved("unresolved-interrupt");
                    Target(after, "interrupt-return-candidate");
                    break;
                case FlowControl.Return:
                case FlowControl.Exception:
                    break;
                case FlowControl.XbeginXabortXend:
                    Unresolved("unresolved-transaction-control");
                    Target(after, "transaction-fallthrough-candidate");
                    break;
                default:
                    Target(after, "fallthrough");
                    break;
            }

            var actual = edgesBySource.TryGetValue(candidate.Address, out var found)
                ? found : Array.Empty<Dos16FlowEdge>();
            if (actual.Length != expected.Count)
                return Incomplete("rejected-missing-or-extra-graph-edge");
            for (int i = 0; i < actual.Length; i++)
                if (actual[i].Kind != expected[i].Kind ||
                    actual[i].Destination != expected[i].Destination)
                    return Incomplete("rejected-forged-graph-edge");

            unresolvedEdges += expected.Count(e => e.Destination is null);
            var valid = expected.Where(e => e.Destination is not null)
                .Select(e => e.Destination!.Value).Distinct().ToArray();
            if (valid.Any(x => !instructions.ContainsKey(x)))
                return Incomplete("incomplete-undecoded-edge-destination",
                    unresolved: unresolvedEdges);
            outgoing.Add(candidate.Address, valid);
        }

        var incoming = new Dictionary<Dos16Address, Dos16RegisterSnapshot>
        {
            [graph.Entry] = Dos16RegisterSnapshot.UnknownFor(region)
        };
        var afterStates = new Dictionary<Dos16Address, Dos16RegisterSnapshot>();
        var queue = new Queue<Dos16Address>();
        var queued = new HashSet<Dos16Address>();
        var revisions = new Dictionary<Dos16Address, int>();
        queue.Enqueue(graph.Entry);
        queued.Add(graph.Entry);
        int steps = 0, merges = 0, widenedFacts = 0;

        while (queue.Count != 0)
        {
            if (steps >= maximumSteps)
                return Incomplete("incomplete-worklist-step-budget",
                    steps, merges, widenedFacts, unresolvedEdges);
            var address = queue.Dequeue();
            queued.Remove(address);
            steps++;
            var next = Dos16RegisterTransfer.Apply(region,
                instructions[address], incoming[address]).After;
            afterStates[address] = next;
            foreach (var destination in outgoing[address])
            {
                if (!incoming.TryGetValue(destination, out var previous))
                {
                    incoming.Add(destination, next);
                    if (queued.Add(destination)) queue.Enqueue(destination);
                    continue;
                }
                int updates = revisions.GetValueOrDefault(destination);
                var joined = previous;
                bool changed = false;
                for (int reg = 0; reg < 8; reg++)
                {
                    var register = (Dos16WordRegister)reg;
                    var oldValue = previous.Get(register);
                    var result = Dos16AbstractValue.Join(oldValue, next.Get(register));
                    if (updates >= 8 && !oldValue.Covers(result))
                    {
                        result = Dos16AbstractValue.Widen(oldValue, result);
                        widenedFacts++;
                    }
                    if (oldValue.Covers(result)) continue;
                    joined = joined.Set(region, reg, result);
                    changed = true;
                }
                if (!changed) continue;
                incoming[destination] = joined;
                revisions[destination] = updates + 1;
                merges++;
                if (queued.Add(destination)) queue.Enqueue(destination);
            }
        }

        var states = afterStates.OrderBy(x => x.Key.Offset)
            .Select(x => new Dos16DataflowState(x.Key, incoming[x.Key], x.Value))
            .ToArray();
        return new Dos16DataflowReport(region.SourceSha256,
            region.MappingEvidence,
            "candidate-unverified;bounded-worklist;flags-unknown;" +
            (unresolvedEdges != 0 ? "unresolved-transfers-present" : "local-edges-checked"),
            false, steps, merges, widenedFacts, unresolvedEdges, states);
    }
}
