using System.Buffers.Binary;
using System.Security.Cryptography;
using DosDis.Core;

internal static class CodeClassificationBenchmarkSmoke
{
    private static void Same<T>(T actual, T expected, string label)
    {
        if (!EqualityComparer<T>.Default.Equals(actual, expected))
            throw new Exception($"Benchmark {label}: expected {expected}; got {actual}");
    }

    private static void Reject(Action run, string label)
    {
        try { run(); }
        catch (InvalidDataException) { return; }
        throw new Exception("Benchmark failed to reject " + label);
    }

    private static void U32(byte[] b, int i, uint value) =>
        BinaryPrimitives.WriteUInt32LittleEndian(b.AsSpan(i), value);

    // Original-free, synthetic MZ/LE with one 4-byte stored object.
    // First 90 C3 is code, following CD 21 is independently marked
    // non-translated for this fixture. The scorer cannot use this
    // synthetic reference to claim any original game's correctness.
    private static BinaryArtifact MakeLeFixture()
    {
        byte[] b = new byte[0x600];
        b[0] = (byte)'M'; b[1] = (byte)'Z';
        U32(b, 0x3C, 0x80);
        int h = 0x80;
        b[h] = (byte)'L'; b[h + 1] = (byte)'E';
        U32(b, h + 0x14, 1);
        U32(b, h + 0x18, 1);
        U32(b, h + 0x1c, 0);
        U32(b, h + 0x28, 4096);
        U32(b, h + 0x2c, 4);
        U32(b, h + 0x40, 0xc4);
        U32(b, h + 0x44, 1);
        U32(b, h + 0x48, 0xdc);
        U32(b, h + 0x80, 0x200);
        U32(b, h + 0xc4, 4);
        U32(b, h + 0xc8, 0x10000);
        U32(b, h + 0xcc, 0x2005);
        U32(b, h + 0xd0, 1);
        U32(b, h + 0xd4, 1);
        b[h + 0xdc + 2] = 1;
        b[0x200] = 0x90; b[0x201] = 0xC3;
        b[0x202] = 0xCD; b[0x203] = 0x21;
        return BinaryArtifact.FromBytes(b);
    }

    public static void Run()
    {
        // Test the externally sourced, machine-readable CODE.1
        // classification MAP, but never import its original bytes to CI.
        string localManifest = Path.Combine(AppContext.BaseDirectory,
            "strahd-code1-object1-classification.json");
        string json = File.ReadAllText(localManifest);
        CodeClassificationReference preserved = CodeClassificationBenchmark.ParseReference(json);
        Same(preserved.SourceByteLength, 277787, "audited full-container length");
        Same(preserved.ObjectByteLength, 70402, "audited object denominator");
        Same(preserved.ExpectedTranslatedBytes, 69361, "translated-byte denominator");
        Same(preserved.ExpectedExcludedBytes, 1041, "excluded-byte denominator");
        Same(preserved.Exclusions.Count, 282, "independently listed exclusion ranges");
        Same(preserved.Exclusions.Where(x => x.Kind == "table").Sum(x => x.EndExclusive - x.Begin),
            204, "reference table exclusions");
        Same(preserved.Exclusions.Where(x => x.Kind == "alignment").Sum(x => x.EndExclusive - x.Begin),
            575, "reference alignment exclusions");
        Same(preserved.Exclusions.Where(x => x.Kind == "noncode").Sum(x => x.EndExclusive - x.Begin),
            262, "reference conservative generic exclusions");

        BinaryArtifact artifact = MakeLeFixture();
        LeImage image = LeImage.Load(artifact);
        byte[] originalObject = image.ReadStoredBytes(1, 0, 4);
        var refSynthetic = new CodeClassificationReference(
            CodeClassificationBenchmark.CurrentSchema,
            artifact.Sha256, artifact.Length, 1, 4,
            Convert.ToHexString(SHA256.HashData(originalObject)), 0x200,
            2, 2, "audited-translation-ledger-not-execution-proof",
            "synthetic-original-free-independent-hypothesis",
            new[] { new ClassificationExclusion(2, 4, "table") });
        FlowAnalysisResult graph = ControlFlowAnalysis.Analyze(image, maxInstructions: 32);
        Same(graph.Instructions.Count, 2, "entry NOP+RET graph count");
        CodeClassificationScore score =
            CodeClassificationBenchmark.Score(artifact, image, graph, refSynthetic);
        Same(score.CandidateOnTranslated, 2, "correctly recovered code bytes");
        Same(score.CandidateOnExcluded, 0, "no excluded bytes falsely decoded");
        Same(score.UnclassifiedTranslated, 0, "no known translated byte missed");
        Same(score.UnclassifiedExcluded, 2, "excluded abstentions not counted as decoder true-negative proof");
        Same(score.CandidateBytePrecision, 1.0, "precision with no false positives");
        Same(score.TranslatedByteRecall, 1.0, "recall with no missed translation");

        var missing = graph with { Instructions = graph.Instructions.Take(1).ToArray() };
        var missingScore = CodeClassificationBenchmark.Score(artifact, image, missing, refSynthetic);
        Same(missingScore.CandidateBytePrecision, 1.0, "no invented false positives");
        Same(missingScore.TranslatedByteRecall, 0.5, "one source-backed byte of two missed");
        Same(missingScore.MissedTranslationSpans.Count, 1, "one missed-span diagnostic");
        Same(missingScore.MissedTranslationSpans[0].Begin, 1, "exact missed code offset");

        var invented = new FlowInstruction(new FlowAddress(1, 2), 2,
            "CD21", "int 21h", "Interrupt", "synthetic candidate");
        var falsePositive = graph with
        {
            Instructions = graph.Instructions.Concat(new[] { invented }).ToArray()
        };
        var falsePositiveScore = CodeClassificationBenchmark.Score(
            artifact, image, falsePositive, refSynthetic);
        Same(falsePositiveScore.CandidateOnTranslated, 2, "true source classification");
        Same(falsePositiveScore.CandidateOnExcluded, 2, "false positives on audited exclusions");
        Same(falsePositiveScore.CandidateBytePrecision, 0.5, "false-positive precision");
        Same(falsePositiveScore.TranslatedByteRecall, 1.0, "recall unaffected by excluded code");
        Same(falsePositiveScore.FalsePositiveExclusionSpans[0].Begin, 2,
            "incorrect candidate's exact object offset");
        Same(falsePositiveScore.FalsePositiveExclusionSpans[0].EndExclusive, 4,
            "incorrect candidate's exclusive upper bound");

        var noPredictions = graph with { Instructions = Array.Empty<FlowInstruction>() };
        var emptyScore = CodeClassificationBenchmark.Score(artifact, image, noPredictions, refSynthetic);
        Same(emptyScore.CandidateBytePrecision, (double?)null,
            "zero predicted instructions cannot claim perfect precision");
        Same(emptyScore.TranslatedByteRecall, 0.0,
            "zero discovered translated bytes yields zero recall");
        Same(emptyScore.UnclassifiedExcluded, 2,
            "unselected exclusions are abstentions, not runtime evidence");

        Reject(() => CodeClassificationBenchmark.Score(artifact, image, graph,
            refSynthetic with { SourceSha256 = new string('0', 64) }),
            "mismatched original full-file SHA");
        Reject(() => CodeClassificationBenchmark.Score(artifact, image, graph,
            refSynthetic with { ObjectSha256 = new string('0', 64) }),
            "mismatched object bytes SHA");
        Reject(() => CodeClassificationBenchmark.Score(artifact, image, graph,
            refSynthetic with { FirstPhysicalFileOffset = 0x201 }),
            "unverified original physical object offset");
        Reject(() => CodeClassificationBenchmark.Validate(refSynthetic with
            { Exclusions = new[] {
                new ClassificationExclusion(2, 4, "table"),
                new ClassificationExclusion(2, 3, "table") } }),
            "overlap/out-of-order exclusion");
        Reject(() => CodeClassificationBenchmark.Validate(refSynthetic with
            { ExpectedExcludedBytes = 1, ExpectedTranslatedBytes = 3 }),
            "forged exclusion count");
        Reject(() => CodeClassificationBenchmark.Score(artifact, image,
            graph with { Instructions = graph.Instructions.Select(x =>
                x.Address.Offset == 0 ? x with { OriginalBytes = "FF" } : x).ToArray() },
            refSynthetic), "forged instruction original bytes");
        Reject(() => CodeClassificationBenchmark.Score(artifact, image,
            graph with { Instructions = graph.Instructions.Concat(
                new[] { graph.Instructions[0] }).ToArray() }, refSynthetic),
            "duplicate/overlapping candidate instructions");
        Reject(() => CodeClassificationBenchmark.ParseReference("{}"),
            "empty/incomplete classification manifest");
        Reject(() => CodeClassificationBenchmark.ParseReference(
            new string(' ', 600_000)), "oversized manifest");

        Console.WriteLine("PASS: original-free SHA-bound CODE.1 classification reference integrity, " +
            "positive/negative code-byte scoring, correct unknown handling, original-byte " +
            "identity, unsafe overlap/bounds and tampering rejection");
    }
}
