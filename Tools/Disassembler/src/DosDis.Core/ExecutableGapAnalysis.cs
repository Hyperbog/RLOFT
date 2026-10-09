using System.Text;
using System.Text.Json;

namespace DosDis.Core;

/// <summary>Unknown executable-object spans; never an implicit code sweep.</summary>
public sealed record ExecutableGap(
    FlowAddress Begin, uint EndExclusive, uint Length, long? FirstFileOffset,
    int RelocationSources, int RelocationTargets, int IncomingEdges,
    int DataReferences, int AnalystSeeds, string Classification, string Explanation);

public sealed record ExecutableGapReport(
    string Format, int SchemaVersion, string SourceSha256,
    long ExecutableObjectBytes, long CandidateInstructionBytes,
    long AnalystProtectedBytes, long UnknownFileBackedBytes,
    long UnbackedVirtualBytes, int TotalUnknownSpans, int EligibleGaps,
    int OmittedGaps, bool AnalysisTruncated, int MinimumGapBytes,
    IReadOnlyList<ExecutableGap> Gaps);

public static class ExecutableGapAnalysis
{
    public const int SchemaVersion = 1;
    private const int MaximumJsonBytes = 8 * 1024 * 1024;

    public static ExecutableGapReport Analyze(FlowAnalysisResult graph, LeImage image,
        int minimumGapBytes = 256, int maximumGaps = 100)
    {
        ArgumentNullException.ThrowIfNull(graph);
        ArgumentNullException.ThrowIfNull(image);
        if (minimumGapBytes is < 1 or > 16 * 1024 * 1024)
            throw new ArgumentOutOfRangeException(nameof(minimumGapBytes));
        if (maximumGaps is < 1 or > 4096)
            throw new ArgumentOutOfRangeException(nameof(maximumGaps));
        if (!string.Equals(graph.ArtifactSha256, image.ArtifactSha256,
                StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("Gap report and LE object map have different original hashes.");

        var objects = image.Objects.Where(o => (o.Flags & 4u) != 0)
            .ToDictionary(o => o.Number);
        long total = objects.Values.Sum(o => (long)o.VirtualSize);
        long code = 0, protectedBytes = 0, unknown = 0, virtualBytes = 0;
        foreach (var r in graph.Ranges)
        {
            if (!objects.TryGetValue(r.Object, out var obj) ||
                r.Begin >= r.EndExclusive || r.EndExclusive > obj.VirtualSize)
                throw new InvalidDataException("Invalid executable-object range in candidate graph.");
            long bytes = (long)r.EndExclusive - r.Begin;
            switch (r.Classification)
            {
                case "candidate-code": code += bytes; break;
                case "protected-data": protectedBytes += bytes; break;
                case "unknown-file-backed": unknown += bytes; break;
                case "unbacked-virtual": virtualBytes += bytes; break;
                default: throw new InvalidDataException("Unsupported candidate range classification.");
            }
        }
        if (total != code + protectedBytes + unknown + virtualBytes)
            throw new InvalidDataException("Executable object byte accounting does not close.");

        var unknownRanges = graph.Ranges
            .Where(r => r.Classification == "unknown-file-backed").ToArray();
        var eligible = unknownRanges.Where(r =>
            (long)r.EndExclusive - r.Begin >= minimumGapBytes).ToArray();
        var selected = eligible.OrderByDescending(r => (long)r.EndExclusive - r.Begin)
            .ThenBy(r => r.Object).ThenBy(r => r.Begin)
            .Take(maximumGaps).ToArray();

        static bool Inside(ClassifiedRange range, int number, long offset) =>
            number == range.Object && offset >= range.Begin && offset < range.EndExclusive;

        var gaps = new List<ExecutableGap>(selected.Length);
        foreach (var r in selected)
        {
            int relocSource = image.Relocations.Count(f => Inside(r, f.SourceObject, f.SourceOffset));
            int relocTarget = image.Relocations.Count(f =>
                Inside(r, f.TargetObject, (long)f.TargetOffset + f.Additive));
            int edgeReferences = graph.Edges.Count(e => e.Destination is { } target &&
                Inside(r, target.Object, target.Offset));
            int dataReferences = graph.DataReferences.Count(d => Inside(r, d.Target.Object, d.Target.Offset));
            int seeds = graph.Seeds.Count(s => Inside(r, s.Object, s.Offset));
            string explanation = graph.Truncated ? "analysis-budget-truncated" :
                relocSource + relocTarget + edgeReferences + dataReferences + seeds > 0
                    ? "referenced-but-not-proven-reachable-code" : "not-reached-by-candidate-traversal";
            gaps.Add(new ExecutableGap(new FlowAddress(r.Object, r.Begin),
                r.EndExclusive, r.EndExclusive - r.Begin, image.ToFileOffset(r.Object, r.Begin),
                relocSource, relocTarget, edgeReferences, dataReferences, seeds,
                r.Classification, explanation));
        }
        return new ExecutableGapReport("dosdis-executable-gap-ledger", SchemaVersion,
            graph.ArtifactSha256, total, code, protectedBytes, unknown, virtualBytes,
            unknownRanges.Length, eligible.Length, eligible.Length - gaps.Count,
            graph.Truncated, minimumGapBytes, gaps);
    }

    public static string BuildJson(ExecutableGapReport report)
    {
        ArgumentNullException.ThrowIfNull(report);
        return JsonSerializer.Serialize(report, new JsonSerializerOptions { WriteIndented = true });
    }

    public static void WriteNew(string filename, ExecutableGapReport report)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(filename);
        string path = Path.GetFullPath(filename);
        if (!string.Equals(Path.GetExtension(path), ".json", StringComparison.OrdinalIgnoreCase))
            throw new ArgumentException("Gap ledger output requires .json.", nameof(filename));
        string directory = Path.GetDirectoryName(path)!;
        if (!Directory.Exists(directory))
            throw new DirectoryNotFoundException("Gap output directory does not exist.");
        string json = BuildJson(report);
        if (Encoding.UTF8.GetByteCount(json) > MaximumJsonBytes)
            throw new InvalidDataException("Gap export exceeds 8 MiB.");
        string staged = Path.Combine(directory, "." + Path.GetFileName(path) +
            "." + Guid.NewGuid().ToString("N") + ".part");
        try
        {
            File.WriteAllText(staged, json, new UTF8Encoding(false));
            File.Move(staged, path);
        }
        finally
        {
            if (File.Exists(staged)) File.Delete(staged);
        }
    }
}
