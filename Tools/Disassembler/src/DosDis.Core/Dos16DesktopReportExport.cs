using System.Globalization;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace DosDis.Core;

/// <summary>
/// Desktop-facing offline export of ORIGINAL-FILE-BACKED, UNVERIFIED x86-16
/// candidates. Does not include a proprietary input, private file path,
/// project notes, live DOS selector assumptions, or whole-image byte dumps.
/// MZ-relative addresses are not LE object-relative addresses.
/// </summary>
public static class Dos16DesktopReportExport
{
    public const int SchemaVersion = 1;
    private const int MaximumOutputCharacters = 32 * 1024 * 1024;
    private static readonly JsonSerializerOptions Options = new()
    {
        WriteIndented = true,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull
    };

    public static string BuildJson(
        BinaryArtifact source, DosMzImage mz, Dos16FlowResult graph,
        bool includeInstructionBytes = false)
    {
        Validate(source, mz, graph);
        string result = JsonSerializer.Serialize(new
        {
            format = "dosdis-mz-16-candidate-analysis",
            schemaVersion = SchemaVersion,
            generatedUtc = DateTimeOffset.UtcNow.ToString("O", CultureInfo.InvariantCulture),
            analysisStatus = "candidate-unverified;single-original-MZ-entry-segment;not-executed",
            source = new
            {
                sha256 = source.Sha256,
                byteLength = source.Length,
                executableFormat = "MZ-16",
                sourcePathIncluded = false,
                sourceBinaryIncluded = false,
                instructionBytesIncluded = includeInstructionBytes
            },
            storedImage = new
            {
                mz.LoadImageFileOffset, mz.FileBackedImageLength,
                mz.DeclaredExecutableFileLength,
                mz.HasTrailingOverlayData,
                mz.InitialCs, mz.InitialIp, mz.InitialSs, mz.InitialSp,
                relocationCount = mz.Relocations.Count,
                relocationValuesApplied = false,
                runtimeLoadSegmentKnown = false
            },
            graph = new
            {
                graph.Entry, graph.MappingEvidence, graph.AnalysisStatus,
                graph.FileOffset, graph.RegionByteLength,
                graph.CandidateInstructionBytes, graph.UnknownFileBackedBytes,
                graph.Truncated,
                instructionCount = graph.Instructions.Count,
                edgeCount = graph.Edges.Count, diagnosticCount = graph.Diagnostics.Count
            },
            instructions = graph.Instructions.Select(x => new
            {
                x.Address, x.Length, x.SourceFileOffset, x.Assembly,
                x.FlowControl, x.DecoderFacts,
                originalBytes = includeInstructionBytes ? x.OriginalBytes : null
            }),
            edges = graph.Edges,
            diagnostics = graph.Diagnostics,
            caveat = "Entry-segment x86-16 candidate control flow only. No proven functions, executed paths, runtime relocations, dynamic targets, overlays, or full-code coverage."
        }, Options);
        return Bound(result);
    }

    public static string BuildText(
        BinaryArtifact source, DosMzImage mz, Dos16FlowResult graph,
        bool includeInstructionBytes = false)
    {
        Validate(source, mz, graph);
        var sb = new StringBuilder();
        sb.AppendLine("DOSDis — 16-bit MZ candidate disassembly / analysis");
        sb.AppendLine("Status: CANDIDATE / UNVERIFIED — no DOS execution or runtime relocation");
        sb.Append("Source SHA-256: ").AppendLine(source.Sha256);
        sb.Append("Original file length: ").AppendLine(source.Length.ToString(CultureInfo.InvariantCulture));
        sb.Append("MZ stored image: file+0x").Append(mz.LoadImageFileOffset.ToString("X"))
            .Append(" length ").AppendLine(mz.FileBackedImageLength.ToString(CultureInfo.InvariantCulture));
        sb.Append("Initial module-relative CS:IP: ")
            .Append(mz.InitialCs.ToString("X4")).Append(':')
            .AppendLine(mz.InitialIp.ToString("X4"));
        sb.Append("Stored MZ relocation entries: ").AppendLine(mz.Relocations.Count.ToString(CultureInfo.InvariantCulture));
        sb.Append("Candidate instructions: ").Append(graph.Instructions.Count)
            .Append(" | edges: ").Append(graph.Edges.Count)
            .Append(" | diagnostics: ").Append(graph.Diagnostics.Count)
            .Append(" | unknown file-backed region bytes: ").Append(graph.UnknownFileBackedBytes)
            .Append(" | truncated: ").AppendLine(graph.Truncated ? "yes" : "no");
        sb.AppendLine("No original executable, private path, analyst comments or DOS loader allocation included.");
        sb.AppendLine(includeInstructionBytes
            ? "Original instruction-byte excerpts explicitly included."
            : "Original instruction-byte excerpts excluded (default).");
        sb.AppendLine("[INSTRUCTIONS — NOT PROVEN EXECUTED]");
        foreach (var row in graph.Instructions)
        {
            sb.Append(row.Address.Segment.ToString("X4")).Append(':')
                .Append(row.Address.Offset.ToString("X4")).Append(" file+0x")
                .Append(row.SourceFileOffset.ToString("X8")).Append(' ');
            if (includeInstructionBytes) sb.Append(row.OriginalBytes).Append(' ');
            sb.Append(row.Assembly).Append(" | ").AppendLine(row.FlowControl);
        }
        sb.AppendLine("[CANDIDATE CONTROL-FLOW EDGES]");
        foreach (var edge in graph.Edges)
        {
            sb.Append(edge.Source.Segment.ToString("X4")).Append(':')
                .Append(edge.Source.Offset.ToString("X4")).Append(" -> ");
            if (edge.Destination is { } to)
                sb.Append(to.Segment.ToString("X4")).Append(':').Append(to.Offset.ToString("X4"));
            else sb.Append("UNRESOLVED");
            sb.Append(" | ").Append(edge.Kind).Append(" | ").AppendLine(edge.Evidence);
        }
        sb.AppendLine("[DIAGNOSTICS]");
        foreach (var note in graph.Diagnostics)
            sb.Append(note.Address.Segment.ToString("X4")).Append(':')
                .Append(note.Address.Offset.ToString("X4")).Append(" | ")
                .Append(note.Code).Append(" | ").AppendLine(note.Explanation);
        sb.AppendLine("No function-boundary certification, dynamic target recovery or original-game runtime trace.");
        return Bound(sb.ToString());
    }

    public static void WriteNew(string path, BinaryArtifact source, DosMzImage mz,
        Dos16FlowResult graph, bool json, bool includeInstructionBytes = false)
    {
        string report = json
            ? BuildJson(source, mz, graph, includeInstructionBytes)
            : BuildText(source, mz, graph, includeInstructionBytes);
        using var file = new FileStream(path, FileMode.CreateNew, FileAccess.Write, FileShare.None);
        using var writer = new StreamWriter(file, new UTF8Encoding(false));
        writer.Write(report);
    }

    private static string Bound(string result)
    {
        if (result.Length > MaximumOutputCharacters)
            throw new InvalidDataException("16-bit MZ candidate report exceeds bounded export limit.");
        return result;
    }

    private static void Validate(BinaryArtifact source, DosMzImage mz, Dos16FlowResult graph)
    {
        ArgumentNullException.ThrowIfNull(source);
        ArgumentNullException.ThrowIfNull(mz);
        ArgumentNullException.ThrowIfNull(graph);
        if (!string.Equals(source.Sha256, mz.SourceSha256, StringComparison.OrdinalIgnoreCase)
            || !string.Equals(source.Sha256, graph.SourceSha256, StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("16-bit analysis does not belong to this original source SHA256.");
        if (mz.IsLauncherStub || graph.Entry.Segment != mz.InitialCs ||
            graph.Entry.Offset != mz.InitialIp ||
            graph.FileOffset != mz.LoadImageFileOffset + mz.InitialCs * 16 ||
            graph.RegionByteLength != mz.CreateEntryCodeRegion().ByteLength)
            throw new InvalidDataException("16-bit analysis does not match the standalone MZ entry window.");
        if (graph.Instructions.Count > 100_000 ||
            graph.Instructions.Any(x => x.Address.Segment != graph.Entry.Segment
                || x.SourceFileOffset != (long)graph.FileOffset + x.Address.Offset
                || x.Address.Offset + x.Length > graph.RegionByteLength))
            throw new InvalidDataException("16-bit report contains unbacked or inconsistent source addresses.");
    }
}
