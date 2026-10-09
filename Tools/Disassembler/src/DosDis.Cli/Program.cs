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
    if (args.Length < 2 || args[0] is not ("inspect" or "disasm" or "fixups" or "memory" or "analyze" or "report" or "mz" or "mz-disasm" or "mz-analyze" or "mz-relocate" or "com" or "com-disasm" or "com-analyze" or "overlay" or "overlay-disasm" or "overlay-map" or "overlay-analyze" or "overlay-fixups"))
    {
        Console.Error.WriteLine("Usage: dosdis inspect|mz|mz-disasm|mz-analyze|mz-relocate|com|com-disasm|com-analyze|overlay|overlay-map|overlay-disasm|overlay-analyze|overlay-fixups|fixups|memory|disasm|analyze|report <file> [options]\n" +
            "MZ relocation simulation: mz-relocate --load-segment HEX [--max 100]; mz-analyze [--load-segment HEX] (only candidate FAR annotations)\n" +
            "16-bit CFG: mz-analyze|com-analyze|overlay-analyze [--entry HEX] [--max-instructions 100000]\n" +
            "Overlay: container inspection; overlay-map|overlay-fixups --resident START.EXE (validated MZ directory)\n" +
            "Overlay-disasm: --regions manifest.json --region ID [--offset 0] [--count 32]\n" +
            "COM: explicitly selected headerless mode [--origin 100] (hex); com-disasm [--offset 100] [--count 32]\n" +
            "MZ: verified DOS MZ header/relocations; mz-disasm [--segment 0] [--offset 0] [--count 32] (x86-16 candidates)\n" +
            "Analyze/report: [--seed 1:0x2975C] [--data 1:0x100:0x110] [--include-entry] [--max-instructions 100000] [--relocated]\n" +
            "Analyze: [--max-output 300] [--trace-out decisions.json] [--trace-capacity 512]\n" +
            "         [--known-answer reference.json --validation-out results.json];\n" +
            "Analyze/report: [--game-id menzo] [--module CODE.1] [--source-archive name.zip] [--internal-path folder/CODE.1]\n" +
            "Analyze/report: [--gap-out gaps.json] [--min-gap-bytes 256] [--max-gaps 100]\n" +
            "All supported actions: [--identity-catalog fingerprints.json] (opt-in; original bytes not uploaded)\n" +
            "Report: --out path.json|path.txt [--include-bytes] (no overwrite)");
        return 2;
    }
    var artifact = BinaryArtifact.Open(args[1]);
    if (args.Count(x => x == "--identity-catalog") > 1)
        throw new ArgumentException("--identity-catalog cannot be repeated.");
    var catalog = args.Contains("--identity-catalog", StringComparer.Ordinal)
        ? SourceIdentityRegistry.ReadFile(Option(args, "--identity-catalog", "")) : null;
    LeImage? image = null;
    if (args[0] is not ("mz" or "mz-disasm" or "mz-analyze" or "mz-relocate" or "com" or "com-disasm" or "com-analyze" or "overlay" or "overlay-disasm" or "overlay-map" or "overlay-analyze" or "overlay-fixups"))
    {
        try { image = LeImage.Load(artifact); }
        catch (InvalidDataException) when (args[0] == "inspect") { /* Identify signature, not fake mapped code. */ }
        catch (NotSupportedException) when (args[0] == "inspect") { /* Unsupported LE page variant. */ }
    }
    if (args[0] is "mz" or "mz-disasm" or "mz-analyze" or "mz-relocate")
    {
        var mz = DosMzImage.Load(artifact);
        if (args[0] == "mz")
        {
            int maximum = int.Parse(Option(args, "--max", "100"), CultureInfo.InvariantCulture);
            if (maximum is < 0 or > 4096) throw new ArgumentOutOfRangeException(nameof(maximum));
            Print(new { artifact.Sha256, Format = "dos-mz-16", mz.LoadImageFileOffset,
                mz.FileBackedImageLength, mz.DeclaredExecutableFileLength,
                mz.InitialCs, mz.InitialIp, mz.InitialSs, mz.InitialSp,
                mz.HasTrailingOverlayData, RelocationCount = mz.Relocations.Count,
                Relocations = mz.Relocations.Take(maximum),
                Status = "header-validated;not-verified-runtime-code" });
        }
        else if (args[0] == "mz-relocate")
        {
            if (args.Count(x => x == "--load-segment") != 1)
                throw new ArgumentException("mz-relocate requires one explicitly assumed --load-segment HEX.");
            ushort load = checked((ushort)Hex(Option(args, "--load-segment", "")));
            var simulation = mz.SimulateRelocations(load);
            int max = int.Parse(Option(args, "--max", "100"), CultureInfo.InvariantCulture);
            if (max is < 0 or > 4096) throw new ArgumentOutOfRangeException(nameof(max));
            Print(new { simulation.Summary.SourceSha256,
                simulation.Summary.AssumedLoadSegment, simulation.Summary.AssumedEntryCs,
                simulation.Summary.AssumedStackSs, simulation.Summary.InitialIp,
                simulation.Summary.InitialSp, simulation.Summary.RelocationWriteCount,
                simulation.Summary.OverlappingRelocationWrites,
                simulation.Summary.AnalysisStatus,
                OmittedDetails = Math.Max(0, simulation.Summary.Writes.Count - max),
                Writes = simulation.Summary.Writes.Take(max) });
        }
        else if (args[0] == "mz-analyze")
        {
            var region = mz.CreateEntryCodeRegion();
            ushort entry = checked((ushort)Hex(Option(args, "--entry",
                mz.InitialIp.ToString("X", CultureInfo.InvariantCulture))));
            int budget = int.Parse(Option(args, "--max-instructions", "100000"), CultureInfo.InvariantCulture);
            var graph16 = Dos16ControlFlowAnalysis.Analyze(region, entry, budget);
            if (args.Contains("--load-segment", StringComparer.Ordinal))
            {
                if (args.Count(x => x == "--load-segment") != 1)
                    throw new ArgumentException("--load-segment cannot be repeated.");
                ushort load = checked((ushort)Hex(Option(args, "--load-segment", "")));
                var simulated = mz.SimulateRelocations(load);
                Print(new {
                    CandidateGraph = graph16,
                    RelocationModel = new {
                        simulated.Summary.SourceSha256,
                        simulated.Summary.AssumedLoadSegment,
                        simulated.Summary.RelocationWriteCount,
                        simulated.Summary.OverlappingRelocationWrites,
                        simulated.Summary.AnalysisStatus
                    },
                    FarImmediateCandidates = DosMzFarTargetRecovery.Inspect(mz, graph16, simulated),
                    Note = "Far targets are separate, non-enqueued candidates; original CFG unresolved edges remain."
                });
            }
            else Print(graph16);
        }
        else
        {
            ushort segment = checked((ushort)Hex(Option(args, "--segment", mz.InitialCs.ToString("X", CultureInfo.InvariantCulture))));
            ushort offset = checked((ushort)Hex(Option(args, "--offset", mz.InitialIp.ToString("X", CultureInfo.InvariantCulture))));
            int count = int.Parse(Option(args, "--count", "32"), CultureInfo.InvariantCulture);
            Print(new { artifact.Sha256, Format = "dos-mz-16",
                AddressAssumptions = "initial-module-relative-segment;original-unrelocated-bytes",
                AnalysisStatus = "requested-linear-candidate-unverified",
                Instructions = mz.Decode(segment, offset, count) });
        }
    }
    else if (args[0] is "com" or "com-disasm" or "com-analyze")
    {
        ushort origin = checked((ushort)Hex(Option(args, "--origin", "100")));
        var com = DosComImage.LoadExplicit(artifact, origin);
        if (args[0] == "com")
            Print(new { artifact.Sha256, Format = "headerless-explicit-com-mode",
                com.Origin, com.FileBackedImageLength,
                Status = "analyst-assumed-16-bit-origin;no-header-or-relocations" });
        else if (args[0] == "com-analyze")
        {
            ushort entry = checked((ushort)Hex(Option(args, "--entry",
                origin.ToString("X", CultureInfo.InvariantCulture))));
            int budget = int.Parse(Option(args, "--max-instructions", "100000"), CultureInfo.InvariantCulture);
            Print(Dos16ControlFlowAnalysis.Analyze(com.CreateCodeRegion(), entry, budget));
        }
        else
        {
            ushort offset = checked((ushort)Hex(Option(args, "--offset",
                origin.ToString("X", CultureInfo.InvariantCulture))));
            int count = int.Parse(Option(args, "--count", "32"), CultureInfo.InvariantCulture);
            Print(new { artifact.Sha256, Format = "headerless-explicit-com-mode",
                OriginalSourceOrigin = origin, Offset = offset,
                AnalysisStatus = "requested-linear-candidate-unverified",
                Instructions = com.Decode(offset, count) });
        }
    }
    else if (args[0] is "overlay" or "overlay-disasm" or "overlay-map" or "overlay-analyze" or "overlay-fixups")
    {
        var overlay = DosOverlayImage.LoadEnvelope(artifact);
        if (args[0] == "overlay")
        {
            Print(new { overlay.Envelope, MappingStatus =
                "container-envelope-only;internal-code-and-fixups-unverified" });
        }
        else if (args[0] is "overlay-map" or "overlay-fixups")
        {
            if (args.Count(x => x == "--resident") != 1)
                throw new ArgumentException("Overlay directory analysis requires exactly one --resident EXE path.");
            var residentArtifact = BinaryArtifact.Open(Option(args, "--resident", ""));
            var resident = DosMzImage.Load(residentArtifact);
            var directory = DosOverlayDirectoryRecovery.Recover(resident, overlay);
            if (args[0] == "overlay-fixups")
                Print(DosOverlayFixupAudit.Analyze(resident, overlay));
            else
                Print(directory);
        }
        else
        {
            if (args.Count(x => x == "--regions") != 1 ||
                args.Count(x => x == "--region") != 1)
                throw new ArgumentException(
                    "Overlay decoding requires one --regions manifest.json and one --region ID.");
            var regions = DosOverlayImage.ReadRegionManifest(Option(args, "--regions", ""));
            string id = Option(args, "--region", "");
            if (args[0] == "overlay-analyze")
            {
                var region = overlay.CreateCodeRegion(regions, id);
                ushort entry = checked((ushort)Hex(Option(args, "--entry",
                    region.Origin.ToString("X", CultureInfo.InvariantCulture))));
                int budget = int.Parse(Option(args, "--max-instructions", "100000"), CultureInfo.InvariantCulture);
                Print(Dos16ControlFlowAnalysis.Analyze(region, entry, budget));
            }
            else
            {
                ushort offset = checked((ushort)Hex(Option(args, "--offset", "0")));
                int count = int.Parse(Option(args, "--count", "32"), CultureInfo.InvariantCulture);
                Print(new {
                    overlay.Envelope, SelectedRegion = id,
                    MappingStatus = "analyst-explicit-source-hash-bound-unverified",
                    AnalysisStatus = "requested-linear-candidate-unverified",
                    Instructions = overlay.DecodeExplicit(regions, id, offset, count)
                });
            }
        }
    }
    else if (args[0] == "inspect")
    {
        var detected = BinaryFormatDetector.Inspect(artifact, image);
        Print(new {
            artifact.Name,
            artifact.Length,
            artifact.Sha256,
            Format = detected.FormatId,
            FormatDetection = detected,
            Module = image == null ? null : new {
                image.ModuleOrigin, image.HeaderOffset, image.PageSize,
                image.EntryObject, image.EntryOffset, image.Objects, Pages = image.Pages,
                FixupCount = image.Relocations.Count
            },
            Intelligence = FileIntelligence.Inspect(artifact, image, catalog)
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
        var provenance = SourceProvenance.ForArtifact(artifact, image,
            args.Contains("--game-id", StringComparer.Ordinal) ? Option(args, "--game-id", "") : null,
            args.Contains("--module", StringComparer.Ordinal) ? Option(args, "--module", "") : null,
            args.Contains("--source-archive", StringComparer.Ordinal) ? Option(args, "--source-archive", "") : null,
            args.Contains("--internal-path", StringComparer.Ordinal) ? Option(args, "--internal-path", "") : null,
            catalog);
        foreach (string option in new[] { "--game-id", "--module", "--source-archive", "--internal-path" })
            if (args.Count(x => x == option) > 1)
                throw new ArgumentException("Provenance options cannot be repeated.");
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
        int minimumGapBytes = int.Parse(Option(args, "--min-gap-bytes", "256"), CultureInfo.InvariantCulture);
        int maximumGaps = int.Parse(Option(args, "--max-gaps", "100"), CultureInfo.InvariantCulture);
        var gapLedger = ExecutableGapAnalysis.Analyze(graph, image, minimumGapBytes, maximumGaps);
        if (args.Contains("--gap-out", StringComparer.Ordinal))
        {
            if (args.Count(x => x == "--gap-out") != 1)
                throw new ArgumentException("--gap-out cannot be repeated.");
            ExecutableGapAnalysis.WriteNew(Option(args, "--gap-out", ""), gapLedger);
            Console.Error.WriteLine("Source-bound executable gap ledger exported.");
        }
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
            AnalysisReportExport.WriteNew(output, graph, asJson, includeBytes, provenance, gapLedger);
            Console.WriteLine($"Candidate analysis report exported ({graph.Instructions.Count} instructions, " +
                $"bytes {(includeBytes ? "included" : "omitted")}). Original executable unchanged.");
        }
        else Print(new {
            Provenance = provenance,
            graph.ArtifactSha256, graph.Seeds, graph.ProtectedData, graph.Truncated,
            Summary = new {
                Instructions = graph.Instructions.Count, Blocks = graph.Blocks.Count,
                FunctionCandidates = graph.FunctionCandidates.Count, Edges = graph.Edges.Count,
                Regions = graph.Ranges.Count, Diagnostics = graph.Diagnostics.Count,
                DataReferences = graph.DataReferences.Count, JumpTables = graph.JumpTables.Count,
                UnknownFileBackedBytes = gapLedger.UnknownFileBackedBytes,
                UnknownGaps = gapLedger.EligibleGaps,
                OmittedGaps = gapLedger.OmittedGaps,
                UnresolvedIndirectCalls = graph.Edges.Count(e => e.Kind == "unresolved-indirect-call"),
                UnresolvedIndirectJumps = graph.Edges.Count(e => e.Kind == "unresolved-indirect-jump"),
                OmittedOutput = graph.Instructions.Count > maxOutput || graph.Edges.Count > maxOutput ||
                    graph.Blocks.Count > maxOutput || graph.FunctionCandidates.Count > maxOutput ||
                    graph.Ranges.Count > maxOutput || graph.Diagnostics.Count > maxOutput ||
                    graph.DataReferences.Count > maxOutput || graph.JumpTables.Count > maxOutput ||
                    gapLedger.Gaps.Count > maxOutput
            },
            Instructions = graph.Instructions.Take(maxOutput), Edges = graph.Edges.Take(maxOutput),
            Blocks = graph.Blocks.Take(maxOutput), FunctionCandidates = graph.FunctionCandidates.Take(maxOutput),
            Ranges = graph.Ranges.Take(maxOutput), Diagnostics = graph.Diagnostics.Take(maxOutput),
            DataReferences = graph.DataReferences.Take(maxOutput), JumpTables = graph.JumpTables.Take(maxOutput),
            UnknownGaps = gapLedger.Gaps.Take(maxOutput)
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
