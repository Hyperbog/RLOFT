using System.Globalization;
using System.Text.Json;
using DosDis.Core;

static string Option(string[] args, string name, string fallback)
{
    int i = Array.IndexOf(args, name);
    return i < 0 ? fallback : i + 1 < args.Length ? args[i + 1]
        : throw new ArgumentException($"Missing value for {name}.");
}
static uint Hex(string s) => uint.Parse(s.StartsWith("0x", StringComparison.OrdinalIgnoreCase) ? s[2..] : s,
    NumberStyles.AllowHexSpecifier, CultureInfo.InvariantCulture);
static IEnumerable<string> RepeatedOption(string[] argv, string key)
{
    for (int i = 2; i < argv.Length; i++)
        if (argv[i] == key)
        {
            if (i + 1 >= argv.Length || argv[i + 1].StartsWith("--", StringComparison.Ordinal))
                throw new ArgumentException($"Missing value for {key}.");
            yield return argv[++i];
        }
}
static FlowAddress ParseSeed(string token)
{
    string[] parts = token.Split(':');
    if (parts.Length != 2) throw new ArgumentException("Seed format is object:hex-offset, for example 1:0x2975C.");
    return new FlowAddress(int.Parse(parts[0], CultureInfo.InvariantCulture), Hex(parts[1]));
}
static ProtectedDataRange ParseData(string token)
{
    string[] parts = token.Split(':');
    if (parts.Length != 3) throw new ArgumentException("Data range format is object:hex-begin:hex-end-exclusive.");
    return new ProtectedDataRange(int.Parse(parts[0], CultureInfo.InvariantCulture), Hex(parts[1]), Hex(parts[2]));
}
static void Print(object payload) => Console.WriteLine(JsonSerializer.Serialize(payload,
    new JsonSerializerOptions { WriteIndented = true }));

try
{
    if (args.Length < 2 || args[0] is not ("inspect" or "disasm" or "fixups" or "memory" or "analyze" or "report"))
    {
        Console.Error.WriteLine("Usage: dosdis inspect|fixups|memory|disasm|analyze|report <file> [options]\n" +
            "Analyze/report: [--seed 1:0x2975C] [--data 1:0x100:0x110] [--include-entry] [--max-instructions 100000] [--relocated]\n" +
            "Analyze: [--max-output 300] [--trace-out decisions.json] [--trace-capacity 512]\n" +
            "         [--known-answer reference.json --validation-out results.json];\n" +
            "Report: --out path.json|path.txt [--include-bytes] (no overwrite)");
        return 2;
    }
    var artifact = BinaryArtifact.Open(args[1]);
    LeImage? image = null;
    try { image = LeImage.Load(artifact); }
    catch (InvalidDataException) when (args[0] == "inspect") { /* Unsupported format: report only confirmed fields. */ }
    catch (NotSupportedException) when (args[0] == "inspect") { /* Unsupported LE page variant. */ }
    if (args[0] == "inspect")
    {
        Print(new {
            artifact.Name,
            artifact.Length,
            artifact.Sha256,
            Format = image == null ? "unrecognized-or-not-yet-supported" : "DOS LE",
            Module = image == null ? null : new {
                image.ModuleOrigin, image.HeaderOffset, image.PageSize,
                image.EntryObject, image.EntryOffset, image.Objects, Pages = image.Pages,
                FixupCount = image.Relocations.Count
            },
            Intelligence = FileIntelligence.Inspect(artifact, image)
        });
    }
    else if (args[0] == "fixups")
    {
        image ??= LeImage.Load(artifact);
        int max = int.Parse(Option(args, "--max", "40"), CultureInfo.InvariantCulture);
        if (max is < 0 or > 10000) throw new ArgumentOutOfRangeException(nameof(max));
        Print(new { artifact.Sha256, Total = image.Relocations.Count,
            Fixups = image.Relocations.Take(max) });
    }
    else if (args[0] is "analyze" or "report")
    {
        image ??= LeImage.Load(artifact);
        var seeds = RepeatedOption(args, "--seed").Select(ParseSeed).ToList();
        if (seeds.Count == 0 || args.Contains("--include-entry", StringComparer.Ordinal))
            seeds.Insert(0, new FlowAddress((int)image.EntryObject, image.EntryOffset));
        var data = RepeatedOption(args, "--data").Select(ParseData).ToArray();
        int maxInstructions = int.Parse(Option(args, "--max-instructions", "100000"), CultureInfo.InvariantCulture);
        int maxOutput = int.Parse(Option(args, "--max-output", "300"), CultureInfo.InvariantCulture);
        if (maxOutput is < 0 or > 10000) throw new ArgumentOutOfRangeException(nameof(maxOutput));
        bool traceRequested = args.Contains("--trace-out", StringComparer.Ordinal);
        bool capacitySpecified = args.Contains("--trace-capacity", StringComparer.Ordinal);
        if (args[0] != "analyze" && (traceRequested || capacitySpecified))
            throw new ArgumentException("Decision tracing is only supported by analyze.");
        if (capacitySpecified && !traceRequested)
            throw new ArgumentException("--trace-capacity requires --trace-out.");
        if (args.Count(x => x == "--trace-out") > 1 ||
            args.Count(x => x == "--trace-capacity") > 1)
            throw new ArgumentException("Decision trace options cannot be repeated.");
        string tracePath = traceRequested ? Option(args, "--trace-out", "") : "";
        if (traceRequested &&
            (string.IsNullOrWhiteSpace(tracePath) ||
             !string.Equals(Path.GetExtension(tracePath), ".json", StringComparison.OrdinalIgnoreCase)))
            throw new ArgumentException("--trace-out requires a .json destination.");
        var decisionTrace = traceRequested
            ? new AnalysisDecisionTrace(int.Parse(
                Option(args, "--trace-capacity", "512"), CultureInfo.InvariantCulture))
            : null;
        bool hasReference = args.Contains("--known-answer", StringComparer.Ordinal);
        bool hasValidationPath = args.Contains("--validation-out", StringComparer.Ordinal);
        if (hasReference != hasValidationPath)
            throw new ArgumentException("--known-answer and --validation-out must be supplied together.");
        if (args[0] != "analyze" && hasReference)
            throw new ArgumentException("Known-answer validation is supported only by analyze.");
        if (args.Count(x => x == "--known-answer") > 1 ||
            args.Count(x => x == "--validation-out") > 1)
            throw new ArgumentException("Known-answer validation options cannot be repeated.");
        KnownAnswerManifest? reference = hasReference
            ? KnownAnswerValidator.ReadManifest(Option(args, "--known-answer", "")) : null;
        var graph = ControlFlowAnalysis.Analyze(image, seeds, data, maxInstructions,
            args.Contains("--relocated", StringComparer.Ordinal), decisionTrace: decisionTrace);
        if (reference is not null)
        {
            var validation = KnownAnswerValidator.Validate(graph, image, artifact, reference);
            KnownAnswerValidator.WriteNew(Option(args, "--validation-out", ""), validation);
            Console.Error.WriteLine("Known-answer candidate comparison saved; source identity verified.");
        }
        if (decisionTrace is not null)
        {
            decisionTrace.WriteNew(tracePath);
            // Keep machine-readable analyze JSON on stdout only.
            Console.Error.WriteLine("Decision trace exported: " +
                decisionTrace.Snapshot().Count.ToString(CultureInfo.InvariantCulture) +
                " retained candidate decision events (earlier events may be dropped).");
        }
        if (args[0] == "report")
        {
            string output = Option(args, "--out", "");
            if (string.IsNullOrWhiteSpace(output))
                throw new ArgumentException("Report requires --out path.json or --out path.txt.");
            string extension = Path.GetExtension(output);
            bool asJson = string.Equals(extension, ".json", StringComparison.OrdinalIgnoreCase);
            if (!asJson && !string.Equals(extension, ".txt", StringComparison.OrdinalIgnoreCase))
                throw new ArgumentException("Report path must have a .json or .txt extension.");
            bool includeBytes = args.Contains("--include-bytes", StringComparer.Ordinal);
            AnalysisReportExport.WriteNew(output, graph, asJson, includeBytes);
            Console.WriteLine($"Candidate analysis report exported ({graph.Instructions.Count} instructions, " +
                $"bytes {(includeBytes ? "included" : "omitted")}). Original executable unchanged.");
        }
        else Print(new {
            graph.ArtifactSha256, graph.Seeds, graph.ProtectedData, graph.Truncated,
            Summary = new {
                Instructions = graph.Instructions.Count, Blocks = graph.Blocks.Count,
                FunctionCandidates = graph.FunctionCandidates.Count, Edges = graph.Edges.Count,
                Regions = graph.Ranges.Count, Diagnostics = graph.Diagnostics.Count,
                DataReferences = graph.DataReferences.Count, JumpTables = graph.JumpTables.Count,
                UnresolvedIndirectCalls = graph.Edges.Count(e => e.Kind == "unresolved-indirect-call"),
                UnresolvedIndirectJumps = graph.Edges.Count(e => e.Kind == "unresolved-indirect-jump"),
                OmittedOutput = graph.Instructions.Count > maxOutput || graph.Edges.Count > maxOutput ||
                    graph.Blocks.Count > maxOutput || graph.FunctionCandidates.Count > maxOutput ||
                    graph.Ranges.Count > maxOutput || graph.Diagnostics.Count > maxOutput ||
                    graph.DataReferences.Count > maxOutput || graph.JumpTables.Count > maxOutput
            },
            Instructions = graph.Instructions.Take(maxOutput), Edges = graph.Edges.Take(maxOutput),
            Blocks = graph.Blocks.Take(maxOutput), FunctionCandidates = graph.FunctionCandidates.Take(maxOutput),
            Ranges = graph.Ranges.Take(maxOutput), Diagnostics = graph.Diagnostics.Take(maxOutput),
            DataReferences = graph.DataReferences.Take(maxOutput), JumpTables = graph.JumpTables.Take(maxOutput)
        });
    }
    else if (args[0] == "memory")
    {
        image ??= LeImage.Load(artifact);
        int number = int.Parse(Option(args, "--object", "1"), CultureInfo.InvariantCulture);
        uint at = Hex(Option(args, "--offset", "0"));
        int length = int.Parse(Option(args, "--count", "16"), CultureInfo.InvariantCulture);
        if (length is < 1 or > 4096) throw new ArgumentOutOfRangeException(nameof(length));
        Print(new { artifact.Sha256, Object = number, Offset = $"0x{at:X}",
            MemoryView = "relocated-virtual-memory", Bytes = Convert.ToHexString(image.ReadRelocatedBytes(number, at, length)),
            FirstSourceFileOffset = image.ToFileOffset(number, at) });
    }
    else
    {
        image ??= LeImage.Load(artifact);
        int objectNumber = int.Parse(Option(args, "--object", "1"), CultureInfo.InvariantCulture);
        uint offset = Hex(Option(args, "--offset", "0"));
        int count = int.Parse(Option(args, "--count", "16"), CultureInfo.InvariantCulture);
        bool relocated = args.Contains("--relocated", StringComparer.Ordinal);
        Print(new { artifact.Sha256, Object = objectNumber, Offset = $"0x{offset:X}",
            MemoryView = relocated ? "relocated-virtual-memory" : "original-file-bytes",
            Lines = Disassembly.Decode(image, objectNumber, offset, count, relocated) });
    }
    return 0;
}
catch (Exception ex) when (ex is IOException or ArgumentException or FormatException or OverflowException or NotSupportedException)
{
    Console.Error.WriteLine($"Analysis failed: {ex.Message}");
    return 1;
}
