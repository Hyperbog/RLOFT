using DosDis.Core;

/// <summary>
/// Bounded, deterministic property/metamorphic feedback that executes
/// only when the owner authorizes the existing manual smoke workflow.
/// No original game bytes, background agents or external services.
/// </summary>
internal static class Dos16MetamorphicFeedbackSmoke
{
    private const uint Seed = 0x16D05D15;
    private static uint Next(ref uint state) =>
        state = unchecked(state * 1664525u + 1013904223u);

    private static void Check(bool condition, string family, int caseNumber,
        string detail)
    {
        if (!condition)
            throw new Exception("x86-16 feedback " + family +
                " seed=0x" + Seed.ToString("X8") +
                " case=" + caseNumber + " " + detail);
    }

    // Independent membership oracle; do not use Covers() to test Covers().
    private static bool Contains(Dos16AbstractValue domain, ushort expected) =>
        domain.Kind switch
        {
            Dos16ValueKind.Unknown => true,
            Dos16ValueKind.Constant or Dos16ValueKind.FiniteSet =>
                domain.Members.Contains(expected),
            Dos16ValueKind.UnsignedInterval =>
                expected >= domain.Minimum && expected <= domain.Maximum,
            _ => false
        };

    private static (Dos16CodeRegion Region, Dos16FlowResult Graph)
        Guarded(int bx, byte[] scale)
    {
        byte[] bytes = new byte[0x50];
        bytes[0] = 0xBB; // MOV BX,imm16
        bytes[1] = (byte)bx;
        bytes[2] = (byte)(bx >> 8);
        bytes[3] = 0x83; bytes[4] = 0xFB; bytes[5] = 0x03; // CMP BX,3
        bytes[6] = 0x77; bytes[7] = checked((byte)(scale.Length + 5)); // JA reject
        scale.CopyTo(bytes, 8);
        int at = 8 + scale.Length;
        bytes[at++] = 0x2E; bytes[at++] = 0xFF;
        bytes[at++] = 0xA7; bytes[at++] = 0x20;
        bytes[at++] = 0x01; bytes[at] = 0xC3;
        // Four source-backed two-byte near pointers at CS:0120.
        for (int index = 0; index < 4; index++)
        {
            bytes[0x20 + index * 2] = (byte)(0x30 + index * 2);
            bytes[0x21 + index * 2] = 0x01;
        }
        var artifact = BinaryArtifact.FromBytes(bytes,
            "synthetic-metamorphic-feedback.com");
        var region = DosComImage.LoadExplicit(artifact).CreateCodeRegion();
        return (region, Dos16ControlFlowAnalysis.Analyze(region, 0x100));
    }

    internal static void Run()
    {
        uint state = Seed;
        int finiteCases = 0, intervalCases = 0, correlatedCases = 0,
            permutationCases = 0, mutationCases = 0;

        // 192 finite cases: exact modular SHL is checked against plain
        // ushort arithmetic, not the abstract interpreter's own math.
        for (int caseId = 0; caseId < 192; caseId++)
        {
            int length = 1 + (int)(Next(ref state) % 8);
            ushort[] inputs = new ushort[length];
            for (int member = 0; member < length; member++)
                inputs[member] = (ushort)Next(ref state);
            int shift = (int)(Next(ref state) % 16);
            var observed = Dos16AbstractValue.ShiftLeft(
                Dos16AbstractValue.Finite(inputs), shift);
            ushort[] expected = inputs.Select(x =>
                unchecked((ushort)(x << shift))).Distinct()
                    .OrderBy(x => x).ToArray();
            Check(observed.Kind ==
                (expected.Length == 1 ? Dos16ValueKind.Constant :
                    Dos16ValueKind.FiniteSet) &&
                observed.Members.SequenceEqual(expected),
                "finite-shift", caseId,
                "shift=" + shift + " inputs=" +
                string.Join(",", inputs.Select(x => x.ToString("X4"))) +
                " expected=" +
                string.Join(",", expected.Select(x => x.ToString("X4"))) +
                " actual=" + observed.Kind);
            finiteCases++;
        }

        // 192 short-range intervals, including 16-bit overflow
        // boundaries. Every concrete value is enumerated independently.
        for (int caseId = 0; caseId < 192; caseId++)
        {
            int start = (int)(Next(ref state) & 0xFFFF);
            int length = (int)(Next(ref state) % 24);
            int end = Math.Min(ushort.MaxValue, start + length);
            int shift = (int)(Next(ref state) % 16);
            var observed = Dos16AbstractValue.ShiftLeft(
                Dos16AbstractValue.Interval((ushort)start, (ushort)end),
                shift);
            for (int input = start; input <= end; input++)
            {
                ushort output = unchecked((ushort)(input << shift));
                Check(Contains(observed, output),
                    "interval-soundness", caseId,
                    "input=0x" + input.ToString("X4") +
                    " shift=" + shift + " output=0x" +
                    output.ToString("X4") + " abstract=" + observed.Kind +
                    " bounds=" + observed.Minimum + ":" + observed.Maximum);
            }
            // If the entire multiplication remains in one 64K
            // quotient, the interval must stay precise.
            long low = (long)start << shift;
            long high = (long)end << shift;
            if (low / 65536 == high / 65536)
                Check(observed.Minimum == unchecked((ushort)low) &&
                    observed.Maximum == unchecked((ushort)high) &&
                    observed.Kind != Dos16ValueKind.Unknown,
                    "interval-precision", caseId,
                    "start=" + start + " end=" + end + " shift=" + shift);
            else
                Check(observed.Kind == Dos16ValueKind.Unknown,
                    "interval-wrap", caseId,
                    "mixed quotients must not infer a contiguous value range");
            intervalCases++;
        }

        // Independent guarded-index pattern matrix: four x86-16 scale
        // encodings crossed with six known BX values. An unsigned CMP
        // bound of 3 permits only source-backed indices 0..3.
        byte[][] scalings =
        {
            new byte[] { 0xD1, 0xE3 },        // SHL BX,1
            new byte[] { 0xC1, 0xE3, 0x01 },  // SHL BX,imm8=1
            new byte[] { 0x01, 0xDB },        // ADD BX,BX
            new byte[] { 0x03, 0xDB }         // ADD BX,BX reverse encoding
        };
        int id = 0;
        foreach (byte[] scale in scalings)
        {
            for (int bx = 0; bx < 6; bx++, id++)
            {
                var fixture = Guarded(bx, scale);
                var report = Dos16CorrelatedDispatchRecovery.Inspect(
                    fixture.Region, fixture.Graph);
                bool inBound = bx <= 3;
                Check(!report.Incomplete && report.InspectedSites == 1 &&
                    report.GuardedStorageSites == 1 &&
                    report.BoundedIndexSites == (inBound ? 1 : 0) &&
                    report.UnresolvedSites == (inBound ? 0 : 1),
                    "guarded-pattern", id,
                    "scale=" + Convert.ToHexString(scale) +
                    " bx=" + bx + " expectedSelected=" + inBound +
                    " actualGuarded=" + report.GuardedStorageSites +
                    " actualBounded=" + report.BoundedIndexSites +
                    " actualUnresolved=" + report.UnresolvedSites +
                    " status=" + report.AnalysisStatus);
                var site = report.Sites.Single();
                Check(inBound
                        ? site.CandidateIndices.SequenceEqual(new[] { bx }) &&
                          site.CandidateStoredEntries.Count == 1 &&
                          site.CandidateStoredEntries[0].Index == bx
                        : site.CandidateIndices.Count == 0 &&
                          site.CandidateStoredEntries.Count == 0,
                    "guarded-index", id,
                    "scale=" + Convert.ToHexString(scale) +
                    " bx=" + bx + " siteStatus=" + site.Status);
                correlatedCases++;

                // Permuting source instruction rows and entire edge
                // groups must not change semantic results. Within a
                // branch's edge group, the decoder's edge order is
                // intentionally preserved as a separate invariant.
                var permuted = fixture.Graph with
                {
                    Instructions = fixture.Graph.Instructions.Reverse().ToArray(),
                    Edges = fixture.Graph.Edges.GroupBy(x => x.Source)
                        .Reverse().SelectMany(group => group).ToArray()
                };
                var reordered = Dos16CorrelatedDispatchRecovery.Inspect(
                    fixture.Region, permuted);
                Check(!reordered.Incomplete &&
                    reordered.GuardedStorageSites == report.GuardedStorageSites &&
                    reordered.BoundedIndexSites == report.BoundedIndexSites &&
                    reordered.Sites.Single().CandidateIndices
                        .SequenceEqual(site.CandidateIndices),
                    "source-inventory-permutation", id,
                    "same original bytes cannot depend on input row order");
                permutationCases++;

                // A graph with mutated source/coverage evidence must
                // fail closed: no successful result and no leaked
                // partial table candidates.
                var stale = fixture.Graph with
                {
                    CandidateInstructionBytes =
                        fixture.Graph.CandidateInstructionBytes + 1
                };
                var rejected = Dos16CorrelatedDispatchRecovery.Inspect(
                    fixture.Region, stale);
                Check(rejected.Incomplete &&
                    rejected.Sites.Count == 0 &&
                    rejected.BoundedIndexSites == 0 &&
                    rejected.AnalysisStatus.Contains(
                        "rejected-inconsistent-byte-coverage"),
                    "coverage-invalidation", id,
                    "metadata tampering must invalidate every selected pointer");
                mutationCases++;
            }
        }
        // Systematic one-at-a-time mutation sweep over a known-good
        // graph. Every instruction-byte or outgoing-edge corruption is
        // checked independently: no successful correlation may survive.
        var baseline = Guarded(1, scalings[0]);
        var baselineReport = Dos16CorrelatedDispatchRecovery.Inspect(
            baseline.Region, baseline.Graph);
        Check(!baselineReport.Incomplete && baselineReport.BoundedIndexSites == 1,
            "mutation-baseline", 0, "known-good graph failed before mutation");

        for (int instructionIndex = 0;
            instructionIndex < baseline.Graph.Instructions.Count;
            instructionIndex++)
        {
            var modified = baseline.Graph.Instructions.ToArray();
            modified[instructionIndex] = modified[instructionIndex] with
            {
                OriginalBytes = "00"
            };
            var forged = Dos16CorrelatedDispatchRecovery.Inspect(
                baseline.Region, baseline.Graph with { Instructions = modified });
            Check(forged.Incomplete && forged.Sites.Count == 0 &&
                forged.BoundedIndexSites == 0,
                "original-byte-mutation", instructionIndex,
                "changing retained bytes of any decoded node must fail closed");
            mutationCases++;
        }

        for (int edgeIndex = 0; edgeIndex < baseline.Graph.Edges.Count; edgeIndex++)
        {
            var modified = baseline.Graph.Edges.ToArray();
            modified[edgeIndex] = modified[edgeIndex] with
            {
                Kind = "synthetic-invalid-edge-kind"
            };
            var forged = Dos16CorrelatedDispatchRecovery.Inspect(
                baseline.Region, baseline.Graph with { Edges = modified });
            Check(forged.Incomplete && forged.Sites.Count == 0 &&
                forged.BoundedIndexSites == 0,
                "control-edge-mutation", edgeIndex,
                "corrupted edge semantics must block every selected pointer");
            mutationCases++;
        }

        Console.WriteLine(
            "PASS: bounded deterministic x86-16 feedback " +
            "seed=0x" + Seed.ToString("X8") +
            " finite=" + finiteCases +
            " intervals=" + intervalCases +
            " guarded=" + correlatedCases +
            " permutations=" + permutationCases +
            " rejectedMutations=" + mutationCases);
    }
}
