using System.Security.Cryptography;
using System.Text.Json;

namespace DosDis.Core;

/// <summary>Audited translation-ledger exclusion, not an ISA claim that
/// an alignment NOP or other excluded byte is intrinsically undecodable.</summary>
public sealed record ClassificationExclusion(int Begin, int EndExclusive, string Kind);

/// <summary>
/// Standalone external reference, with no proprietary original bytes.
/// It is bound to a whole-file hash AND the hash of a specific LE object.
/// </summary>
public sealed record CodeClassificationReference(
    int SchemaVersion, string SourceSha256, int SourceByteLength,
    int ObjectNumber, int ObjectByteLength, string ObjectSha256,
    long? FirstPhysicalFileOffset, int ExpectedTranslatedBytes,
    int ExpectedExcludedBytes, string ReferenceStatus,
    string Provenance, IReadOnlyList<ClassificationExclusion> Exclusions);

public sealed record ClassificationFinding(int Begin, int EndExclusive, string Kind);

/// <summary>Byte *classification* agreement, never an instruction-boundary,
/// C++ semantic, reachability or runtime-execution certification.</summary>
public sealed record CodeClassificationScore(
    string SourceSha256, int ObjectNumber, int ObjectByteLength,
    string ReferenceStatus, string Status, bool AnalysisTruncated,
    int CandidateInstructionCount, int CandidateInstructionBytes,
    int TranslatedReferenceBytes, int ExcludedReferenceBytes,
    int CandidateOnTranslated, int CandidateOnExcluded,
    int UnclassifiedTranslated, int UnclassifiedExcluded,
    double? CandidateBytePrecision, double? TranslatedByteRecall,
    IReadOnlyList<ClassificationFinding> FalsePositiveExclusionSpans,
    IReadOnlyList<ClassificationFinding> MissedTranslationSpans,
    bool FindingsTruncated);

public static class CodeClassificationBenchmark
{
    public const int CurrentSchema = 1;
    private const int MaxObjectBytes = 8 * 1024 * 1024;
    private const int MaxJsonCharacters = 512 * 1024;
    private const int MaxExcludedRanges = 100_000;
    private const int MaxFindings = 128;
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
        MaxDepth = 24
    };

    /// <summary>Parse a manifest as externally asserted evidence. Only Score
    /// can establish whether source bytes match the manifest's fingerprints.</summary>
    public static CodeClassificationReference ParseReference(string json)
    {
        ArgumentNullException.ThrowIfNull(json);
        if (json.Length is < 2 or > MaxJsonCharacters)
            throw new InvalidDataException("Classification reference exceeds bounded JSON size.");
        CodeClassificationReference reference;
        try
        {
            reference = JsonSerializer.Deserialize<CodeClassificationReference>(json, JsonOptions)
                ?? throw new InvalidDataException("Empty classification reference.");
        }
        catch (JsonException ex)
        {
            throw new InvalidDataException("Invalid classification reference JSON.", ex);
        }
        Validate(reference);
        return reference;
    }

    public static void Validate(CodeClassificationReference reference)
    {
        ArgumentNullException.ThrowIfNull(reference);
        if (reference.SchemaVersion != CurrentSchema ||
            reference.SourceByteLength is < 1 or > (256 * 1024 * 1024) ||
            reference.ObjectNumber is < 1 or > 4096 ||
            reference.ObjectByteLength is < 1 or > MaxObjectBytes ||
            reference.ExpectedTranslatedBytes < 0 ||
            reference.ExpectedExcludedBytes < 0 ||
            (long)reference.ExpectedTranslatedBytes + reference.ExpectedExcludedBytes != reference.ObjectByteLength ||
            reference.FirstPhysicalFileOffset is < 0 ||
            string.IsNullOrWhiteSpace(reference.Provenance) ||
            reference.Provenance.Length > 512 ||
            reference.ReferenceStatus != "audited-translation-ledger-not-execution-proof")
            throw new InvalidDataException("Unsupported or incomplete classification reference dimensions.");
        RequireSha(reference.SourceSha256);
        RequireSha(reference.ObjectSha256);
        if (reference.Exclusions is null || reference.Exclusions.Count > MaxExcludedRanges)
            throw new InvalidDataException("Missing or oversized exclusion ranges.");
        int lastEnd = 0;
        long sum = 0;
        foreach (var item in reference.Exclusions)
        {
            if (item is null || item.Kind is not ("table" or "alignment" or "noncode") ||
                item.Begin < lastEnd || item.EndExclusive <= item.Begin ||
                item.EndExclusive > reference.ObjectByteLength)
                throw new InvalidDataException("Exclusion ranges must be valid, sorted and disjoint.");
            sum += item.EndExclusive - item.Begin;
            lastEnd = item.EndExclusive;
        }
        if (sum != reference.ExpectedExcludedBytes)
            throw new InvalidDataException("Audited exclusion count does not match manifest.");
    }

    public static CodeClassificationScore Score(
        BinaryArtifact artifact, LeImage image, FlowAnalysisResult graph,
        CodeClassificationReference reference)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        ArgumentNullException.ThrowIfNull(image);
        ArgumentNullException.ThrowIfNull(graph);
        Validate(reference);
        if (!string.Equals(artifact.Sha256, reference.SourceSha256, StringComparison.OrdinalIgnoreCase) ||
            !string.Equals(image.ArtifactSha256, artifact.Sha256, StringComparison.OrdinalIgnoreCase) ||
            !string.Equals(graph.ArtifactSha256, artifact.Sha256, StringComparison.OrdinalIgnoreCase) ||
            artifact.Length != reference.SourceByteLength)
            throw new InvalidDataException("Benchmark original-file SHA-256/length does not match reference and analysis.");
        if (reference.ObjectNumber > image.Objects.Count ||
            image.Objects[reference.ObjectNumber - 1].VirtualSize != (uint)reference.ObjectByteLength)
            throw new InvalidDataException("Benchmark object identity/virtual size does not match original LE image.");

        int n = reference.ObjectByteLength;
        // Enforce an exact file-backed object and its independent byte hash.
        // This also rejects synthetic or malformed overlays with unbacked holes.
        byte[] original = image.ReadStoredBytes(reference.ObjectNumber, 0, n);
        string objectSha = Convert.ToHexString(SHA256.HashData(original));
        if (!string.Equals(objectSha, reference.ObjectSha256, StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("Benchmark object byte SHA-256 is not the asserted reference object.");
        if (reference.FirstPhysicalFileOffset is { } expectedFirst)
        {
            if (image.ToFileOffset(reference.ObjectNumber, 0) != expectedFirst)
                throw new InvalidDataException("Reference object physical start differs from the verified LE mapping.");
            // The offset is an assertion about the entire flat object, not
            // just page zero: detect reordered pages or aliasing.
            for (int i = 0; i < n; i++)
                if (image.ToFileOffset(reference.ObjectNumber, (uint)i) != expectedFirst + i)
                    throw new InvalidDataException("Reference assumes a contiguous object but LE pages are noncontiguous.");
        }

        var excluded = new bool[n];
        foreach (var range in reference.Exclusions)
            Array.Fill(excluded, true, range.Begin, range.EndExclusive - range.Begin);
        var candidate = new bool[n];
        int candidateCount = 0;
        foreach (var instruction in graph.Instructions)
        {
            if (instruction.Address.Object != reference.ObjectNumber) continue;
            if (instruction.Length is < 1 or > 15 ||
                instruction.Address.Offset >= n ||
                instruction.Length > n - (long)instruction.Address.Offset)
                throw new InvalidDataException("Candidate instruction has invalid object bounds.");
            int begin = checked((int)instruction.Address.Offset);
            byte[] emitted;
            try { emitted = Convert.FromHexString(instruction.OriginalBytes); }
            catch (FormatException ex) { throw new InvalidDataException("Candidate instruction hex is invalid.", ex); }
            if (emitted.Length != instruction.Length ||
                !original.AsSpan(begin, instruction.Length).SequenceEqual(emitted))
                throw new InvalidDataException("Candidate instruction bytes disagree with original file-backed object.");
            for (int i = begin; i < begin + instruction.Length; i++)
            {
                if (candidate[i]) throw new InvalidDataException("Candidate instruction overlap/duplicate is not scoreable.");
                candidate[i] = true;
            }
            candidateCount++;
        }

        int coveredCode = 0, coveredExcluded = 0, missingCode = 0, missingExcluded = 0;
        for (int i = 0; i < n; i++)
        {
            if (excluded[i])
            {
                if (candidate[i]) coveredExcluded++;
                else missingExcluded++;
            }
            else
            {
                if (candidate[i]) coveredCode++;
                else missingCode++;
            }
        }
        if (coveredCode + missingCode != reference.ExpectedTranslatedBytes ||
            coveredExcluded + missingExcluded != reference.ExpectedExcludedBytes)
            throw new InvalidDataException("Benchmark byte partition does not reconcile.");

        // Keep actionable disagreements bounded; incomplete lists are marked.
        int falsePositiveFindings = 0, missedFindings = 0;
        var falsePositive = BuildFindings(i => excluded[i] && candidate[i],
            "candidate-on-excluded", n, ref falsePositiveFindings);
        var missed = BuildFindings(i => !excluded[i] && !candidate[i],
            "translated-not-discovered", n, ref missedFindings);
        int predicted = coveredCode + coveredExcluded;
        return new CodeClassificationScore(artifact.Sha256, reference.ObjectNumber, n,
            reference.ReferenceStatus,
            "candidate-byte-agreement-not-decoder-or-runtime-certification",
            graph.Truncated, candidateCount, predicted,
            reference.ExpectedTranslatedBytes, reference.ExpectedExcludedBytes,
            coveredCode, coveredExcluded, missingCode, missingExcluded,
            predicted == 0 ? null : (double)coveredCode / predicted,
            reference.ExpectedTranslatedBytes == 0 ? null : (double)coveredCode / reference.ExpectedTranslatedBytes,
            falsePositive, missed, falsePositiveFindings > MaxFindings || missedFindings > MaxFindings);
    }

    private static IReadOnlyList<ClassificationFinding> BuildFindings(
        Func<int, bool> flagged, string kind, int length, ref int total)
    {
        var findings = new List<ClassificationFinding>();
        for (int i = 0; i < length;)
        {
            if (!flagged(i)) { i++; continue; }
            int start = i;
            while (i < length && flagged(i)) i++;
            total++;
            if (findings.Count < MaxFindings)
                findings.Add(new ClassificationFinding(start, i, kind));
        }
        return findings;
    }

    private static void RequireSha(string? sha)
    {
        if (sha is null || sha.Length != 64 ||
            sha.Any(c => !Uri.IsHexDigit(c)))
            throw new InvalidDataException("Benchmark SHA-256 must be exactly 64 hexadecimal digits.");
    }
}
