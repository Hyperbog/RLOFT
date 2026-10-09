using System.Text;
using System.Text.Json;

namespace DosDis.Core;

/// <summary>
/// Analyst-supplied, independently researched expectations for ONE exact
/// original executable. File names never identify the binary by themselves.
/// </summary>
public sealed record KnownAnswerManifest(
    int SchemaVersion, string GameId, string Module, string SourceSha256,
    int SourceByteLength, string ExecutableFormat, string SourceEvidence,
    IReadOnlyList<KnownInstructionExpectation> VerifiedInstructions,
    IReadOnlyList<KnownNonCodeExpectation> VerifiedNonCode,
    IReadOnlyList<KnownEdgeExpectation> VerifiedEdges,
    IReadOnlyList<KnownFixupExpectation> VerifiedFixups,
    string? SourceArchive = null, string? InternalPath = null);

public sealed record KnownInstructionExpectation(
    FlowAddress Address, string OriginalHex, string EvidenceId);
public sealed record KnownNonCodeExpectation(
    int Object, uint Begin, uint EndExclusive, string EvidenceId);
public sealed record KnownEdgeExpectation(
    FlowAddress Source, FlowAddress Destination, string Kind, string EvidenceId);
public sealed record KnownFixupExpectation(
    FlowAddress Source, FlowAddress Target, string EvidenceId);

public sealed record KnownAnswerFinding(
    string Category, FlowAddress Address, string Status, string EvidenceId);

public sealed record KnownAnswerValidationReport(
    string Format, int SchemaVersion, string GameId, string Module,
    string SourceSha256, int SourceByteLength, string ExecutableFormat,
    string SourceEvidence, string AnalysisStatus,
    int Matched, int Missing, int Conflicting, int NotObserved,
    int UnbenchmarkedCandidateInstructions,
    IReadOnlyList<KnownAnswerFinding> Findings);

/// <summary>
/// A comparison against independent research, NOT a promotion of analyzer
/// candidates to verified instructions. Manifest and binary hashes must match.
/// </summary>
public static class KnownAnswerValidator
{
    public const int SchemaVersion = 1;
    private const int MaxManifestBytes = 1024 * 1024;
    private const int MaxExportBytes = 8 * 1024 * 1024;
    private static readonly JsonSerializerOptions Options = new(JsonSerializerDefaults.Web)
    {
        WriteIndented = true
    };

    public static KnownAnswerManifest ReadManifest(string path)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);
        var info = new FileInfo(path);
        if (info.Length > MaxManifestBytes)
            throw new InvalidDataException("Known-answer manifest exceeds 1 MiB.");
        try
        {
            var manifest = JsonSerializer.Deserialize<KnownAnswerManifest>(
                File.ReadAllText(path), Options)
                ?? throw new InvalidDataException("Known-answer manifest was empty.");
            ValidateManifest(manifest);
            return manifest;
        }
        catch (JsonException e)
        {
            throw new InvalidDataException("Invalid known-answer JSON.", e);
        }
    }

    private static void ValidateManifest(KnownAnswerManifest manifest)
    {
        if (manifest.SchemaVersion != SchemaVersion ||
            manifest.SourceSha256 is null || manifest.SourceSha256.Length != 64 ||
            !manifest.SourceSha256.All(c => c is >= '0' and <= '9' or >= 'a' and <= 'f' or >= 'A' and <= 'F') ||
            string.IsNullOrWhiteSpace(manifest.GameId) ||
            !manifest.GameId.All(c => c is >= 'a' and <= 'z' or >= '0' and <= '9' or '-') ||
            string.IsNullOrWhiteSpace(manifest.Module) ||
            !manifest.Module.All(c => c is >= 'A' and <= 'Z' or >= '0' and <= '9' or '.') ||
            manifest.SourceByteLength < 1 ||
            manifest.ExecutableFormat != "LE" ||
            string.IsNullOrWhiteSpace(manifest.SourceEvidence) ||
            manifest.VerifiedInstructions is null || manifest.VerifiedNonCode is null ||
            manifest.VerifiedEdges is null || manifest.VerifiedFixups is null)
            throw new InvalidDataException("Incomplete or unsupported source-specific known-answer manifest.");
        int total = manifest.VerifiedInstructions.Count + manifest.VerifiedNonCode.Count +
            manifest.VerifiedEdges.Count + manifest.VerifiedFixups.Count;
        if (total is < 1 or > 4096)
            throw new InvalidDataException("Known-answer expectation count must be between 1 and 4096.");
        foreach (var x in manifest.VerifiedInstructions)
            if (x is null || x.Address.Object < 1 ||
                x.OriginalHex is null || x.OriginalHex.Length is < 2 or > 30 ||
                (x.OriginalHex.Length & 1) != 0 ||
                !x.OriginalHex.All(c => c is >= '0' and <= '9' or >= 'a' and <= 'f' or >= 'A' and <= 'F') ||
                string.IsNullOrWhiteSpace(x.EvidenceId))
                throw new InvalidDataException("Invalid verified instruction expectation.");
        foreach (var x in manifest.VerifiedNonCode)
            if (x is null || x.Object < 1 || x.Begin >= x.EndExclusive ||
                (ulong)x.EndExclusive - x.Begin > 1024 * 1024 ||
                string.IsNullOrWhiteSpace(x.EvidenceId))
                throw new InvalidDataException("Invalid verified non-code expectation.");
        foreach (var x in manifest.VerifiedEdges)
            if (x is null || x.Source.Object < 1 || x.Destination.Object < 1 ||
                string.IsNullOrWhiteSpace(x.Kind) || string.IsNullOrWhiteSpace(x.EvidenceId))
                throw new InvalidDataException("Invalid verified branch expectation.");
        foreach (var x in manifest.VerifiedFixups)
            if (x is null || x.Source.Object < 1 || x.Target.Object < 1 ||
                string.IsNullOrWhiteSpace(x.EvidenceId))
                throw new InvalidDataException("Invalid verified relocation expectation.");
    }

    public static KnownAnswerValidationReport Validate(
        FlowAnalysisResult graph, LeImage image, BinaryArtifact artifact, KnownAnswerManifest manifest)
    {
        ArgumentNullException.ThrowIfNull(graph);
        ArgumentNullException.ThrowIfNull(image);
        ArgumentNullException.ThrowIfNull(artifact);
        ArgumentNullException.ThrowIfNull(manifest);
        ValidateManifest(manifest);
        if (!string.Equals(manifest.SourceSha256, artifact.Sha256, StringComparison.OrdinalIgnoreCase) ||
            !string.Equals(graph.ArtifactSha256, artifact.Sha256, StringComparison.OrdinalIgnoreCase) ||
            !string.Equals(image.ArtifactSha256, artifact.Sha256, StringComparison.OrdinalIgnoreCase) ||
            manifest.SourceByteLength != artifact.Length)
            throw new InvalidDataException("Known-answer source SHA-256 or byte length does not match the opened original.");

        var findings = new List<KnownAnswerFinding>();
        var instructions = graph.Instructions.ToDictionary(x => x.Address);
        foreach (var expected in manifest.VerifiedInstructions)
        {
            string state = instructions.TryGetValue(expected.Address, out var found)
                ? (string.Equals(found.OriginalBytes, expected.OriginalHex, StringComparison.OrdinalIgnoreCase)
                    ? "match" : "conflict")
                : "not-observed";
            findings.Add(new KnownAnswerFinding("verified-instruction", expected.Address, state, expected.EvidenceId));
        }
        foreach (var expected in manifest.VerifiedNonCode)
        {
            FlowAddress where = new(expected.Object, expected.Begin);
            bool overlaps = graph.Instructions.Any(insn => insn.Address.Object == expected.Object &&
                (ulong)insn.Address.Offset < expected.EndExclusive &&
                (ulong)insn.Address.Offset + (uint)insn.Length > expected.Begin);
            bool protectedRange = graph.Ranges.Any(range =>
                range.Object == expected.Object && range.Begin <= expected.Begin &&
                range.EndExclusive >= expected.EndExclusive &&
                range.Classification == "protected-data");
            findings.Add(new KnownAnswerFinding("verified-noncode", where,
                overlaps ? "conflict" : protectedRange ? "match" : "not-classified", expected.EvidenceId));
        }
        foreach (var expected in manifest.VerifiedEdges)
        {
            string status = !instructions.ContainsKey(expected.Source) ? "not-observed" :
                graph.Edges.Any(edge => edge.Source == expected.Source &&
                    edge.Destination == expected.Destination && edge.Kind == expected.Kind)
                    ? "match" : "missing";
            findings.Add(new KnownAnswerFinding("verified-flow-edge", expected.Source, status, expected.EvidenceId));
        }
        foreach (var expected in manifest.VerifiedFixups)
        {
            bool fixupExists = image.Relocations.Any(fixup =>
                fixup.SourceObject == expected.Source.Object &&
                fixup.SourceOffset == expected.Source.Offset &&
                fixup.TargetObject == expected.Target.Object &&
                (ulong)fixup.TargetOffset + fixup.Additive == expected.Target.Offset);
            findings.Add(new KnownAnswerFinding("verified-le-fixup", expected.Source,
                fixupExists ? "match" : "missing", expected.EvidenceId));
        }
        // A partial manifest is NOT a complete instruction inventory:
        // nonbenchmarked candidates are not automatically unexpected/wrong.
        var expectedStarts = manifest.VerifiedInstructions.Select(x => x.Address).ToHashSet();
        return new KnownAnswerValidationReport(
            "dosdis-known-answer-validation", SchemaVersion, manifest.GameId, manifest.Module,
            artifact.Sha256, artifact.Length, manifest.ExecutableFormat,
            manifest.SourceEvidence, "candidate-comparison-not-certification",
            findings.Count(x => x.Status == "match"),
            findings.Count(x => x.Status == "missing"),
            findings.Count(x => x.Status == "conflict"),
            findings.Count(x => x.Status is "not-observed" or "not-classified"),
            graph.Instructions.Count(x => !expectedStarts.Contains(x.Address)),
            findings);
    }

    public static string BuildJson(KnownAnswerValidationReport report)
    {
        ArgumentNullException.ThrowIfNull(report);
        return JsonSerializer.Serialize(report, Options);
    }

    /// <summary>Explicit local export only; does not overwrite previous evidence.</summary>
    public static void WriteNew(string path, KnownAnswerValidationReport report)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);
        string destination = Path.GetFullPath(path);
        if (!string.Equals(Path.GetExtension(destination), ".json", StringComparison.OrdinalIgnoreCase))
            throw new ArgumentException("Validation output must be JSON.", nameof(path));
        string directory = Path.GetDirectoryName(destination)!;
        if (!Directory.Exists(directory))
            throw new DirectoryNotFoundException("Validation output directory does not exist.");
        string json = BuildJson(report);
        if (Encoding.UTF8.GetByteCount(json) > MaxExportBytes)
            throw new InvalidDataException("Validation report exceeds 8 MiB.");
        string temporary = Path.Combine(directory, "." + Path.GetFileName(destination) +
            "." + Guid.NewGuid().ToString("N") + ".part");
        try
        {
            File.WriteAllText(temporary, json, new UTF8Encoding(false));
            File.Move(temporary, destination);
        }
        finally
        {
            if (File.Exists(temporary)) File.Delete(temporary);
        }
    }
}
