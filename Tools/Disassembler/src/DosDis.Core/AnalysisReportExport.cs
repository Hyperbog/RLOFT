using System.Globalization;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace DosDis.Core;

/// <summary>
/// Human-readable and structured offline exports of CANDIDATE analysis evidence.
/// No original executable, absolute file paths, private project comments, or
/// full memory images are embedded. Instruction bytes require explicit opt-in.
/// </summary>
public static class AnalysisReportExport
{
    public const int SchemaVersion = 1;
    private const int MaximumExportBytes = 128 * 1024 * 1024;
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull
    };

    public static string BuildJson(FlowAnalysisResult graph, bool includeInstructionBytes = false)
    {
        ArgumentNullException.ThrowIfNull(graph);
        // Exports are archival statements of a candidate graph, never a
        // declaration that decoded bytes or recovered boundaries are correct.
        return JsonSerializer.Serialize(new
        {
            format = "dosdis-analysis-report",
            schemaVersion = SchemaVersion,
            generatedUtc = DateTimeOffset.UtcNow.ToString("O", CultureInfo.InvariantCulture),
            toolVersion = typeof(AnalysisReportExport).Assembly.GetName().Version?.ToString() ?? "unknown",
            analysisStatus = "candidate-unverified",
            source = new { sha256 = graph.ArtifactSha256, sourceBinaryIncluded = false,
                sourcePathIncluded = false, instructionBytesIncluded = includeInstructionBytes },
            counts = new { instructions = graph.Instructions.Count, functions = graph.FunctionCandidates.Count,
                edges = graph.Edges.Count, references = graph.DataReferences.Count,
                jumpTables = graph.JumpTables.Count, diagnostics = graph.Diagnostics.Count,
                indirectCallTables = graph.JumpTables.Count(x => x.DispatchKind == "indirect-call-table"),
                indirectJumpTables = graph.JumpTables.Count(x => x.DispatchKind == "indirect-jump-table"),
                unclassifiedDispatchTables = graph.JumpTables.Count(x => x.DispatchKind == "unclassified-indirect-table"),
                unresolvedIndirectCalls = graph.Edges.Count(e => e.Kind == "unresolved-indirect-call"),
                unresolvedIndirectJumps = graph.Edges.Count(e => e.Kind == "unresolved-indirect-jump"),
                truncated = graph.Truncated },
            seeds = graph.Seeds,
            protectedRanges = graph.ProtectedData,
            instructions = graph.Instructions.Select(x => new
            {
                x.Address, x.Length, x.Assembly, x.FlowControl, x.Evidence,
                originalBytes = includeInstructionBytes ? x.OriginalBytes : null
            }),
            edges = graph.Edges,
            blocks = graph.Blocks,
            functionCandidates = graph.FunctionCandidates,
            classifiedRanges = graph.Ranges,
            diagnostics = graph.Diagnostics,
            dataReferences = graph.DataReferences,
            jumpTableCandidates = graph.JumpTables
        }, JsonOptions);
    }

    public static string BuildText(FlowAnalysisResult graph, bool includeInstructionBytes = false)
    {
        ArgumentNullException.ThrowIfNull(graph);
        var text = new StringBuilder();
        text.AppendLine("DOSDis candidate analysis report — schema 1");
        text.AppendLine("Classification: CANDIDATE / UNVERIFIED — not a faithful decompilation");
        text.Append("Source SHA-256: ").AppendLine(graph.ArtifactSha256);
        text.AppendLine("No original executable, source file path, or project comments included.");
        text.AppendLine(includeInstructionBytes
            ? "Original instruction-byte excerpts: explicitly included"
            : "Original instruction-byte excerpts: omitted (default)");
        text.Append("Instructions: ").Append(graph.Instructions.Count)
            .Append(" | Function candidates: ").Append(graph.FunctionCandidates.Count)
            .Append(" | Control-flow edges: ").Append(graph.Edges.Count)
            .Append(" | Data references: ").Append(graph.DataReferences.Count)
            .Append(" | Jump-table candidates: ").Append(graph.JumpTables.Count)
            .Append(" | Diagnostics: ").Append(graph.Diagnostics.Count)
            .Append(" | Unresolved indirect calls: ")
            .Append(graph.Edges.Count(e => e.Kind == "unresolved-indirect-call"))
            .Append(" | Unresolved indirect jumps: ")
            .Append(graph.Edges.Count(e => e.Kind == "unresolved-indirect-jump"))
            .Append(" | Truncated: ").AppendLine(graph.Truncated ? "yes" : "no");
        text.AppendLine();
        text.AppendLine("[SEEDS]");
        foreach (var seed in graph.Seeds) text.AppendLine(Format(seed));
        text.AppendLine("[PROTECTED DATA]");
        foreach (var range in graph.ProtectedData)
            text.Append(Format(range.Object, range.Begin)).Append("..0x")
                .Append(range.EndExclusive.ToString("X8", CultureInfo.InvariantCulture))
                .Append(" ").AppendLine(Safe(range.Provenance));
        text.AppendLine("[INSTRUCTIONS — CANDIDATES]");
        foreach (var insn in graph.Instructions)
        {
            text.Append(Format(insn.Address)).Append(' ');
            if (includeInstructionBytes) text.Append(Safe(insn.OriginalBytes)).Append(' ');
            text.Append(Safe(insn.Assembly)).Append(" | ").Append(Safe(insn.FlowControl))
                .Append(" | ").AppendLine(Safe(insn.Evidence));
        }
        text.AppendLine("[FUNCTION CANDIDATES]");
        foreach (var fn in graph.FunctionCandidates)
            text.Append(Format(fn.Entry)).Append(" blocks=").Append(fn.Blocks.Count)
                .Append(" | ").AppendLine(Safe(fn.Evidence));
        text.AppendLine("[CONTROL FLOW]");
        foreach (var edge in graph.Edges)
            text.Append(Format(edge.Source)).Append(" -> ")
                .Append(edge.Destination is { } dest ? Format(dest) : "unresolved")
                .Append(" | ").Append(Safe(edge.Kind)).Append(" | ").AppendLine(Safe(edge.Evidence));
        text.AppendLine("[BLOCKS]");
        foreach (var block in graph.Blocks)
            text.Append(Format(block.Start)).Append("..0x")
                .Append(block.EndExclusive.ToString("X8", CultureInfo.InvariantCulture))
                .Append(" instructionStarts=").AppendLine(block.InstructionAddresses.Count.ToString(CultureInfo.InvariantCulture));
        text.AppendLine("[CLASSIFIED RANGES]");
        foreach (var range in graph.Ranges)
            text.Append(Format(range.Object, range.Begin)).Append("..0x")
                .Append(range.EndExclusive.ToString("X8", CultureInfo.InvariantCulture))
                .Append(" | ").Append(Safe(range.Classification)).Append(" | ").AppendLine(Safe(range.Evidence));
        text.AppendLine("[DATA REFERENCES]");
        foreach (var reference in graph.DataReferences)
            text.Append(Format(reference.Source)).Append(" -> ").Append(Format(reference.Target))
                .Append(" | ").Append(Safe(reference.Kind)).Append(" | ").AppendLine(Safe(reference.Evidence));
        text.AppendLine("[INDIRECT DISPATCH TABLE CANDIDATES — CALL / JMP]");
        foreach (var table in graph.JumpTables)
            text.Append(Format(table.Source)).Append(" kind=").Append(Safe(table.DispatchKind))
                .Append(" table=").Append(Format(table.Table))
                .Append(" width=").Append(table.EntryWidth).Append(" candidates=")
                .Append(string.Join(", ", table.CandidateTargets.Select(x => Format(x))))
                .Append(" | ").AppendLine(Safe(table.Evidence));
        text.AppendLine("[DIAGNOSTICS]");
        foreach (var item in graph.Diagnostics)
        {
            text.Append(Format(item.Address)).Append(" | ").Append(Safe(item.Code))
                .Append(" | ").AppendLine(Safe(item.Description));
            if (item.Conflict is { } conflict)
                text.Append("  proposed=").Append(Safe(conflict.ProposedAssembly))
                    .Append(" retained=").Append(conflict.RetainedStart is { } retained
                        ? Format(retained) : "(analyst-protected-data)")
                    .Append(" retained-assembly=").Append(Safe(conflict.RetainedAssembly ?? "none"))
                    .Append(" reason=").Append(Safe(conflict.RejectionReason))
                    .Append(" disposition=").Append(Safe(conflict.Disposition))
                    .Append(" unresolved=").AppendLine(conflict.RemainsUnresolved ? "yes" : "no");
        }
        return text.ToString();
    }

    /// <summary>Never replaces an existing export or leaves a partial final file.</summary>
    public static void WriteNew(string outputPath, FlowAnalysisResult graph,
        bool asJson, bool includeInstructionBytes = false)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(outputPath);
        ArgumentNullException.ThrowIfNull(graph);
        string destination = Path.GetFullPath(outputPath);
        string expectedExt = asJson ? ".json" : ".txt";
        if (!string.Equals(Path.GetExtension(destination), expectedExt, StringComparison.OrdinalIgnoreCase))
            throw new ArgumentException($"Expected a {expectedExt} report filename.", nameof(outputPath));
        string data = asJson ? BuildJson(graph, includeInstructionBytes) : BuildText(graph, includeInstructionBytes);
        if (Encoding.UTF8.GetByteCount(data) > MaximumExportBytes)
            throw new InvalidDataException("Analysis report exceeds the 128 MiB export safety limit.");
        string directory = Path.GetDirectoryName(destination)!;
        string temporary = Path.Combine(directory, $".{Path.GetFileName(destination)}.{Guid.NewGuid():N}.part");
        try
        {
            File.WriteAllText(temporary, data, new UTF8Encoding(encoderShouldEmitUTF8Identifier: false));
            // Same-directory rename is atomic for a single volume, and with no
            // overwrite cannot silently replace earlier analyst evidence.
            File.Move(temporary, destination);
        }
        finally
        {
            if (File.Exists(temporary)) File.Delete(temporary);
        }
    }

    private static string Format(FlowAddress address) => Format(address.Object, address.Offset);
    private static string Format(int objectNumber, uint offset) =>
        $"{objectNumber}:0x{offset:X8}";
    private static string Safe(string value) => value.Replace("\r", "\\r", StringComparison.Ordinal)
        .Replace("\n", "\\n", StringComparison.Ordinal);
}
