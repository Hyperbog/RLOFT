using System.Text;
using System.Text.Json;
using Iced.Intel;

namespace DosDis.Core;

/// <summary>
/// Predeclared x86-16 expectations obtained from an independently implemented
/// decoder. The supplied oracle is analyst-authored, not authenticated by
/// this reader; comparison never certifies its independence or runtime flow.
/// </summary>
public sealed record Dos16ExternalOracleManifest(
    int SchemaVersion, string GameId, string Module,
    string SourceSha256, int SourceByteLength,
    int RegionFileOffset, int RegionByteLength, ushort Segment, ushort Origin,
    string DecoderName, string DecoderVersion, string DecoderConfiguration,
    string OracleEvidenceId,
    IReadOnlyList<Dos16ExternalExpectedInstruction> ExpectedInstructions);

/// <summary>
/// Externally researched instruction identity and optional direct CFG edge.
/// FlowControl uses a canonical name from the Iced FlowControl vocabulary
/// for output normalization, not the external decoder's printed syntax.
/// </summary>
public sealed record Dos16ExternalExpectedInstruction(
    ushort Offset, long SourceFileOffset, string OriginalHex,
    string FlowControl, string EvidenceId,
    string? DirectEdgeKind = null, ushort? DirectTarget = null,
    Dos16InstructionFacts? ExpectedFacts = null);

public sealed record Dos16ExternalOracleFinding(
    ushort Offset, string Status, string DetailCode, string EvidenceId);

public sealed record Dos16ExternalOracleReport(
    string Format, int SchemaVersion, string SourceSha256,
    int SourceByteLength, int RegionFileOffset, int RegionByteLength,
    ushort Segment, ushort Origin, ushort AnalysisEntry,
    string MappingEvidence, string GameId, string Module,
    string DecoderName, string DecoderVersion, string DecoderConfiguration,
    string OracleEvidenceId, string IndependenceStatus,
    string AnalysisStatus, bool Truncated, int Expected,
    int Matched, int Missing, int Conflicting, int NotObserved,
    int UnbenchmarkedCandidates,
    int StructuredOperandExpectations, int FullyMatchedOperandExpectations,
    IReadOnlyList<Dos16ExternalOracleFinding> Findings);

/// <summary>
/// Read-only, exact-source x86-16 comparison against a separately prepared
/// external oracle. This is NOT a second decoder, independence attestation,
/// executable-code certification, or an automatic build/release gate.
/// </summary>
public static class Dos16ExternalOracleValidator
{
    public const int SchemaVersion = 1;
    private const int MaxManifestBytes = 512 * 1024;
    private const int MaxOutputBytes = 2 * 1024 * 1024;
    private const int MaxExpectations = 2048;
    private static readonly JsonSerializerOptions JsonOptions =
        new(JsonSerializerDefaults.Web) { WriteIndented = true };

    // JsonSerializerDefaults.Web accepts case-insensitive field names. Without
    // this guard, a duplicated property (including different casing) can
    // silently replace an externally reviewed expectation during import.
    // Preserve a single unambiguous claim at every JSON object level.
    private static void RejectAmbiguousJsonProperties(JsonElement value)
    {
        if (value.ValueKind == JsonValueKind.Object)
        {
            var names = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            foreach (var property in value.EnumerateObject())
            {
                if (!names.Add(property.Name))
                    throw new InvalidDataException(
                        "External oracle JSON contains duplicated or case-colliding properties.");
                RejectAmbiguousJsonProperties(property.Value);
            }
        }
        else if (value.ValueKind == JsonValueKind.Array)
        {
            foreach (var item in value.EnumerateArray())
                RejectAmbiguousJsonProperties(item);
        }
    }

    // System.Text.Json ignores unknown properties by default. This is unsafe
    // for an oracle: a typo in an optional expectedFacts or memory field
    // would silently reduce the tested semantic denominator.
    private static JsonElement? NamedMember(JsonElement obj, string name)
    {
        if (obj.ValueKind != JsonValueKind.Object)
            return null;
        foreach (var property in obj.EnumerateObject())
            if (string.Equals(property.Name, name, StringComparison.OrdinalIgnoreCase))
                return property.Value;
        return null;
    }

    private static void RejectUnknownProperties(JsonElement obj,
        params string[] allowed)
    {
        if (obj.ValueKind != JsonValueKind.Object)
            return;
        foreach (var property in obj.EnumerateObject())
            if (!allowed.Contains(property.Name, StringComparer.OrdinalIgnoreCase))
                throw new InvalidDataException(
                    "External oracle JSON contains an unknown expectation property.");
    }

    private static void RejectUnknownOracleProperties(JsonElement root)
    {
        RejectUnknownProperties(root, "schemaVersion", "gameId", "module",
            "sourceSha256", "sourceByteLength", "regionFileOffset",
            "regionByteLength", "segment", "origin", "decoderName",
            "decoderVersion", "decoderConfiguration", "oracleEvidenceId",
            "expectedInstructions");
        var rows = NamedMember(root, "expectedInstructions");
        if (rows?.ValueKind != JsonValueKind.Array)
            return; // Typed import rejects invalid or missing arrays.
        foreach (var row in rows.Value.EnumerateArray())
        {
            RejectUnknownProperties(row, "offset", "sourceFileOffset",
                "originalHex", "flowControl", "evidenceId",
                "directEdgeKind", "directTarget", "expectedFacts");
            var facts = NamedMember(row, "expectedFacts");
            if (facts?.ValueKind != JsonValueKind.Object)
                continue;
            RejectUnknownProperties(facts.Value, "mnemonic",
                "operandCount", "operand0", "operand1");
            foreach (string name in new[] { "operand0", "operand1" })
            {
                var operand = NamedMember(facts.Value, name);
                if (operand?.ValueKind != JsonValueKind.Object)
                    continue;
                RejectUnknownProperties(operand.Value, "kind", "register",
                    "immediateValue", "memory");
                var memory = NamedMember(operand.Value, "memory");
                if (memory?.ValueKind == JsonValueKind.Object)
                    RejectUnknownProperties(memory.Value, "base", "index",
                        "displacementSize", "displacement", "size",
                        "segmentOverride");
            }
        }
    }

    public static Dos16ExternalOracleManifest ReadManifest(string path)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);
        var info = new FileInfo(path);
        if (info.Length > MaxManifestBytes)
            throw new InvalidDataException("External x86-16 oracle manifest exceeds 512 KiB.");
        try
        {
            // Parse once so both the duplicate-field audit and the typed
            // import inspect precisely the same source snapshot.
            using var document = JsonDocument.Parse(File.ReadAllText(path));
            RejectAmbiguousJsonProperties(document.RootElement);
            RejectUnknownOracleProperties(document.RootElement);
            var manifest = document.RootElement.Deserialize<Dos16ExternalOracleManifest>(
                JsonOptions)
                ?? throw new InvalidDataException("External oracle manifest was empty.");
            ValidateManifest(manifest);
            return manifest;
        }
        catch (JsonException e)
        {
            throw new InvalidDataException("Malformed external x86-16 oracle JSON.", e);
        }
    }

    // Complete, source-relative 16-bit ModR/M addressing forms only.
    // The default DS/SS selection is deliberately not asserted:
    // SegmentOverride is null unless a prefix byte selected a segment.
    private static void ValidateMemory(Dos16MemoryAddressFact memory)
    {
        bool baseOK = memory.Base is null or "BX" or "BP" or "SI" or "DI";
        bool indexOK = memory.Index is null or "SI" or "DI";
        bool pairOK = memory.Index is null || memory.Base is "BX" or "BP";
        bool segmentOK = memory.SegmentOverride is null or
            "CS" or "DS" or "ES" or "SS" or "FS" or "GS";
        bool displacementOK = memory.DisplacementSize switch
        {
            0 => memory.Displacement == 0,
            1 => memory.Displacement <= 0x7F ||
                memory.Displacement >= 0xFF80 && memory.Displacement <= 0xFFFF,
            2 => memory.Displacement <= 0xFFFF,
            _ => false
        };
        if (!baseOK || !indexOK || !pairOK || !segmentOK ||
            !displacementOK ||
            (memory.Base is null && memory.Index is null &&
                memory.DisplacementSize != 2) ||
            // In 16-bit ModR/M, bare [BP] has no mod=00 encoding.
            // It must be represented by an explicitly encoded disp8/16.
            (memory.Base == "BP" && memory.Index is null &&
                memory.DisplacementSize == 0) ||
            memory.Size is not ("UInt8" or "UInt16" or "UInt32"))
            throw new InvalidDataException(
                "External memory operand exceeds exact supported 16-bit ModRM scope.");
    }

    private static void ValidateOperand(Dos16OperandFact? operand)
    {
        if (operand is null ||
            !Enum.TryParse<OpKind>(operand.Kind, false, out var kind) ||
            !string.Equals(Enum.GetName(kind), operand.Kind,
                StringComparison.Ordinal))
            throw new InvalidDataException("Invalid normalized external operand kind.");
        if (kind == OpKind.Register)
        {
            if (operand.Register is null || operand.ImmediateValue is not null ||
                operand.Memory is not null ||
                !Enum.TryParse<Register>(operand.Register, false, out var register) ||
                register == Register.None ||
                !string.Equals(Enum.GetName(register), operand.Register,
                    StringComparison.Ordinal))
                throw new InvalidDataException(
                    "External register operand requires a canonical register name.");
        }
        else if (kind == OpKind.Memory)
        {
            if (operand.Register is not null || operand.ImmediateValue is not null ||
                operand.Memory is null)
                throw new InvalidDataException(
                    "External memory operand must have complete address facts.");
            ValidateMemory(operand.Memory);
        }
        else
        {
            if (operand.Register is not null || operand.Memory is not null)
                throw new InvalidDataException(
                    "Immediate operands cannot also assert register or memory facts.");
            bool valid = kind switch
            {
                OpKind.Immediate8 => operand.ImmediateValue is >= 0 and <= 255,
                OpKind.Immediate16 => operand.ImmediateValue is >= 0 and <= 65535,
                OpKind.Immediate32 => operand.ImmediateValue is >= 0 and <= uint.MaxValue,
                OpKind.Immediate8to16 or OpKind.Immediate8to32 =>
                    operand.ImmediateValue is >= -128 and <= 127,
                _ => false
            };
            if (!valid)
                throw new InvalidDataException(
                    "External operand must be a register or a precisely sized immediate.");
        }
    }

    private static void ValidateFacts(Dos16InstructionFacts facts)
    {
        if (facts.Mnemonic is null ||
            !Enum.TryParse<Mnemonic>(facts.Mnemonic, false, out var mnemonic) ||
            !string.Equals(Enum.GetName(mnemonic), facts.Mnemonic,
                StringComparison.Ordinal) ||
            facts.OperandCount is < 0 or > 2 ||
            (facts.OperandCount >= 1) != (facts.Operand0 is not null) ||
            (facts.OperandCount >= 2) != (facts.Operand1 is not null))
            throw new InvalidDataException(
                "Invalid or unsupported x86-16 mnemonic/operand expectation.");
        if (facts.Operand0 is not null) ValidateOperand(facts.Operand0);
        if (facts.Operand1 is not null) ValidateOperand(facts.Operand1);
    }

    private static void ValidateManifest(Dos16ExternalOracleManifest manifest)
    {
        ArgumentNullException.ThrowIfNull(manifest);
        if (manifest.SchemaVersion != SchemaVersion ||
            manifest.SourceSha256 is null || manifest.SourceSha256.Length != 64 ||
            !manifest.SourceSha256.All(c =>
                c is >= '0' and <= '9' or >= 'a' and <= 'f' or >= 'A' and <= 'F') ||
            manifest.SourceByteLength < 1 ||
            manifest.RegionFileOffset < 0 || manifest.RegionByteLength < 1 ||
            (long)manifest.RegionFileOffset + manifest.RegionByteLength >
                manifest.SourceByteLength ||
            (long)manifest.Origin + manifest.RegionByteLength > 0x10000 ||
            string.IsNullOrWhiteSpace(manifest.GameId) ||
            string.IsNullOrWhiteSpace(manifest.Module) ||
            string.IsNullOrWhiteSpace(manifest.DecoderName) ||
            string.IsNullOrWhiteSpace(manifest.DecoderVersion) ||
            string.IsNullOrWhiteSpace(manifest.DecoderConfiguration) ||
            string.IsNullOrWhiteSpace(manifest.OracleEvidenceId) ||
            manifest.ExpectedInstructions is null ||
            manifest.ExpectedInstructions.Count is < 1 or > MaxExpectations)
            throw new InvalidDataException(
                "External x86-16 oracle lacks bounded, source-specific provenance.");

        // Prevent a built-in decoder result from being mislabeled as a
        // *different* decoder. Other names are still untrusted claims:
        // external origin and reviewer must be assessed outside this code.
        string decoder = manifest.DecoderName.Trim().ToLowerInvariant();
        if (decoder.Contains("iced", StringComparison.Ordinal) ||
            decoder.Contains("dosdis", StringComparison.Ordinal))
            throw new InvalidDataException(
                "An external oracle cannot identify the embedded DOSDis/Iced decoder.");

        if (manifest.ExpectedInstructions.Any(e => e is null))
            throw new InvalidDataException(
                "External x86-16 oracle contains a null instruction expectation.");
        var previousEnd = -1;
        foreach (var e in manifest.ExpectedInstructions.OrderBy(x => x.Offset))
        {
            if (e is null || string.IsNullOrWhiteSpace(e.EvidenceId) ||
                e.OriginalHex is null || e.OriginalHex.Length is < 2 or > 30 ||
                (e.OriginalHex.Length & 1) != 0 ||
                !e.OriginalHex.All(c =>
                    c is >= '0' and <= '9' or >= 'a' and <= 'f' or >= 'A' and <= 'F') ||
                !Enum.TryParse<FlowControl>(e.FlowControl, false, out var flow) ||
                !string.Equals(Enum.GetName(flow), e.FlowControl, StringComparison.Ordinal) ||
                e.Offset < manifest.Origin ||
                (long)e.Offset + e.OriginalHex.Length / 2 >
                    (long)manifest.Origin + manifest.RegionByteLength ||
                e.SourceFileOffset !=
                    (long)manifest.RegionFileOffset + e.Offset - manifest.Origin ||
                ((e.DirectEdgeKind is null) != (e.DirectTarget is null)) ||
                (e.DirectEdgeKind is not null && e.DirectEdgeKind is not
                    ("branch" or "branch-taken" or "call")) ||
                (e.DirectEdgeKind is not null &&
                    e.DirectEdgeKind != (flow switch
                    {
                        FlowControl.Call => "call",
                        FlowControl.UnconditionalBranch => "branch",
                        FlowControl.ConditionalBranch => "branch-taken",
                        _ => null
                    })) ||
                e.Offset < previousEnd)
                throw new InvalidDataException(
                    "External x86-16 oracle has an invalid, duplicated, or overlapping instruction.");
            if (e.ExpectedFacts is not null)
                ValidateFacts(e.ExpectedFacts);
            previousEnd = e.Offset + e.OriginalHex.Length / 2;
        }
    }

    // Integrity recheck, NOT a second independent decoder. A cached graph may
    // have legitimate original hex while retaining forged flow/operand facts
    // or an injected direct edge. Re-decode only this original file window
    // using the embedded decoder before counting an external match.
    private static bool CandidateMatchesOriginalDecode(
        Dos16CodeRegion region, Dos16FlowInstruction row,
        Dos16ExternalExpectedInstruction expected)
    {
        if (row.Length is < 1 or > 15)
            return false;
        int physical = checked(region.FileOffset + expected.Offset - region.Origin);
        var originalBytes = region.Source.Data.Slice(physical, row.Length).ToArray();
        var decoder = Iced.Intel.Decoder.Create(16, new ByteArrayCodeReader(originalBytes));
        decoder.IP = expected.Offset;
        decoder.Decode(out var original);
        if (original.Code == Code.INVALID || original.Length != row.Length ||
            !string.Equals(original.FlowControl.ToString(), row.FlowControl,
                StringComparison.Ordinal))
            return false;
        if (row.DecoderFacts is not null &&
            !Equals(Dos16ControlFlowAnalysis.CaptureFacts(original), row.DecoderFacts))
            return false;
        if (expected.DirectEdgeKind is not null &&
            (original.Op0Kind is not (OpKind.NearBranch16 or OpKind.NearBranch32) ||
             original.NearBranchTarget != expected.DirectTarget!.Value))
            return false;
        return true;
    }

    public static Dos16ExternalOracleReport Compare(
        Dos16CodeRegion region, Dos16FlowResult graph,
        Dos16ExternalOracleManifest manifest)
    {
        ArgumentNullException.ThrowIfNull(region);
        ArgumentNullException.ThrowIfNull(graph);
        ValidateManifest(manifest);
        if (!string.Equals(manifest.SourceSha256, region.SourceSha256,
                StringComparison.OrdinalIgnoreCase) ||
            manifest.SourceByteLength != region.Source.Length ||
            manifest.RegionFileOffset != region.FileOffset ||
            manifest.RegionByteLength != region.ByteLength ||
            manifest.Origin != region.Origin ||
            manifest.Segment != region.Segment ||
            !string.Equals(graph.SourceSha256, region.SourceSha256,
                StringComparison.OrdinalIgnoreCase) ||
            graph.FileOffset != region.FileOffset ||
            graph.RegionByteLength != region.ByteLength ||
            graph.MappingEvidence != region.MappingEvidence ||
            graph.Entry.Segment != region.Segment ||
            graph.Entry.Offset < region.Origin ||
            graph.Entry.Offset >= region.EndExclusive)
            throw new InvalidDataException(
                "External x86-16 oracle/CFG/source region identity mismatch.");

        var decoded = new Dictionary<ushort, Dos16FlowInstruction>();
        foreach (var instruction in graph.Instructions)
            if (instruction is null ||
                instruction.Address.Segment != region.Segment ||
                !decoded.TryAdd(instruction.Address.Offset, instruction))
                throw new InvalidDataException(
                    "External oracle comparison requires unique same-segment CFG rows.");

        var findings = new List<Dos16ExternalOracleFinding>(
            manifest.ExpectedInstructions.Count);
        foreach (var expected in manifest.ExpectedInstructions)
        {
            var actualFileOffset = region.FileOffset + expected.Offset - region.Origin;
            string stored = Convert.ToHexString(region.Source.Data.Slice(
                actualFileOffset, expected.OriginalHex.Length / 2));
            string detail;
            string status;
            if (!string.Equals(stored, expected.OriginalHex,
                    StringComparison.OrdinalIgnoreCase))
            {
                status = "conflict";
                detail = "external-oracle-bytes-disagree-with-original-file";
            }
            else if (!decoded.TryGetValue(expected.Offset, out var row))
            {
                status = "not-observed";
                detail = "source-backed-instruction-not-discovered";
            }
            else if (row.SourceFileOffset != expected.SourceFileOffset ||
                     row.Length != expected.OriginalHex.Length / 2 ||
                     !string.Equals(row.OriginalBytes, stored,
                         StringComparison.OrdinalIgnoreCase))
            {
                status = "conflict";
                detail = "candidate-instruction-location-length-or-bytes-disagree";
            }
            else if (!string.Equals(row.FlowControl, expected.FlowControl,
                         StringComparison.Ordinal))
            {
                status = "conflict";
                detail = "candidate-flow-category-disagrees-with-external-oracle";
            }
            else if (expected.ExpectedFacts is not null &&
                     row.DecoderFacts is null)
            {
                status = "not-observed";
                detail = "candidate-structured-operand-facts-unavailable";
            }
            else if (expected.ExpectedFacts is not null &&
                     (row.DecoderFacts!.OperandCount > 2 ||
                      (expected.ExpectedFacts.Operand0?.Kind == "Memory" &&
                       row.DecoderFacts.Operand0?.Memory is null) ||
                      (expected.ExpectedFacts.Operand1?.Kind == "Memory" &&
                       row.DecoderFacts.Operand1?.Memory is null)))
            {
                status = "not-observed";
                detail = "candidate-memory-address-facts-outside-supported-scope";
            }
            else if (expected.ExpectedFacts is not null &&
                     !Equals(row.DecoderFacts, expected.ExpectedFacts))
            {
                status = "conflict";
                detail = "candidate-structured-operands-disagree-with-external-oracle";
            }
            else if (expected.DirectEdgeKind is not null &&
                     !graph.Edges.Any(e => e.Source ==
                            new Dos16Address(region.Segment, expected.Offset) &&
                         e.Destination ==
                            new Dos16Address(region.Segment, expected.DirectTarget!.Value) &&
                         e.Kind == expected.DirectEdgeKind))
            {
                status = "missing";
                detail = "externally-expected-direct-edge-not-found";
            }
            else if (!CandidateMatchesOriginalDecode(region, row, expected))
            {
                status = "conflict";
                detail = "candidate-decoder-or-direct-edge-disagrees-with-original-bytes";
            }
            else
            {
                status = "match";
                detail = "source-and-declared-oracle-agree";
            }
            findings.Add(new Dos16ExternalOracleFinding(
                expected.Offset, status, detail, expected.EvidenceId));
        }

        var expectedOffsets = manifest.ExpectedInstructions
            .Select(e => e.Offset).ToHashSet();
        return new Dos16ExternalOracleReport(
            "dosdis-x86-16-external-oracle-comparison", SchemaVersion,
            region.SourceSha256, region.Source.Length,
            region.FileOffset, region.ByteLength, region.Segment, region.Origin,
            graph.Entry.Offset, region.MappingEvidence,
            manifest.GameId, manifest.Module,
            manifest.DecoderName, manifest.DecoderVersion,
            manifest.DecoderConfiguration, manifest.OracleEvidenceId,
            "claimed-external-not-reviewed",
            "candidate-comparison-not-certification",
            graph.Truncated, findings.Count,
            findings.Count(f => f.Status == "match"),
            findings.Count(f => f.Status == "missing"),
            findings.Count(f => f.Status == "conflict"),
            findings.Count(f => f.Status == "not-observed"),
            decoded.Keys.Count(offset => !expectedOffsets.Contains(offset)),
            manifest.ExpectedInstructions.Count(e => e.ExpectedFacts is not null),
            manifest.ExpectedInstructions.Zip(findings,
                (expected, finding) =>
                    expected.ExpectedFacts is not null && finding.Status == "match")
                .Count(matched => matched),
            findings);
    }

    public static string BuildJson(Dos16ExternalOracleReport report)
    {
        ArgumentNullException.ThrowIfNull(report);
        return JsonSerializer.Serialize(report, JsonOptions);
    }

    /// <summary>Nonoverwriting local JSON export, never a release attestation.</summary>
    public static void WriteNew(string path, Dos16ExternalOracleReport report)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);
        var destination = Path.GetFullPath(path);
        if (!string.Equals(Path.GetExtension(destination), ".json",
                StringComparison.OrdinalIgnoreCase))
            throw new ArgumentException("External oracle report must have .json extension.");
        var directory = Path.GetDirectoryName(destination)!;
        if (!Directory.Exists(directory))
            throw new DirectoryNotFoundException(
                "External oracle export directory does not exist.");
        string output = BuildJson(report);
        if (Encoding.UTF8.GetByteCount(output) > MaxOutputBytes)
            throw new InvalidDataException(
                "External oracle result exceeds 2 MiB safety limit.");
        string staging = Path.Combine(directory, "." + Path.GetFileName(destination) +
            "." + Guid.NewGuid().ToString("N") + ".part");
        try
        {
            File.WriteAllText(staging, output, new UTF8Encoding(false));
            File.Move(staging, destination);
        }
        finally
        {
            if (File.Exists(staging)) File.Delete(staging);
        }
    }
}
