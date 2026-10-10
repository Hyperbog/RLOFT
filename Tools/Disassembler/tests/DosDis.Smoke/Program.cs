using System.Buffers.Binary;
using System.Text.Json;
using DosDis.Core;

static void Equal<T>(T actual, T expected, string label)
{
    if (!EqualityComparer<T>.Default.Equals(actual, expected))
        throw new Exception($"{label}: expected {expected}, received {actual}");
}
static void Fails(Action action, string label)
{
    try { action(); }
    catch (InvalidDataException) { return; }
    catch (NotSupportedException) { return; }
    throw new Exception($"{label}: malformed input was accepted");
}
static void Dword(byte[] b, int at, uint v) => BinaryPrimitives.WriteUInt32LittleEndian(b.AsSpan(at, 4), v);
static byte[] MakeFixture(bool nested)
{
    var b = new byte[0x600];
    int m = nested ? 0x180 : 0, h = m + 0x80;
    b[0] = (byte)'M'; b[1] = (byte)'Z';
    b[m] = (byte)'M'; b[m + 1] = (byte)'Z';
    Dword(b, m + 0x3c, 0x80);
    b[h] = (byte)'L'; b[h + 1] = (byte)'E';
    Dword(b, h + 0x14, 1);   // one stored LE page
    Dword(b, h + 0x18, 1);   // entry object
    Dword(b, h + 0x1c, 0);   // entry offset
    Dword(b, h + 0x28, 4096);
    Dword(b, h + 0x2c, 4);   // only four bytes stored in final page
    Dword(b, h + 0x40, 0xc4); // object table
    Dword(b, h + 0x44, 1);
    Dword(b, h + 0x48, 0xdc); // page map immediately after object
    Dword(b, h + 0x80, 0x200); // nested origin, NOT outer 0
    Dword(b, h + 0xc4, 4); // object virtual size
    Dword(b, h + 0xc8, 0x10000);
    Dword(b, h + 0xcc, 0x2005);
    Dword(b, h + 0xd0, 1);
    Dword(b, h + 0xd4, 1);
    b[h + 0xdc + 2] = 1; // first page ordinal, big-endian 3-byte LE page map index
    b[m + 0x200] = 0x90; b[m + 0x201] = 0xC3; // nop / ret
    b[m + 0x202] = 0x90; b[m + 0x203] = 0xC3;
    return b;
}

try
{
    Dos16ExternalOracleSmoke.Run();
    Dos16AbstractValueSmoke.Run();
    Dos16RegisterTransferSmoke.Run();
    Dos16DataflowSmoke.Run();
    Dos16TableIndexEvidenceSmoke.Run();
    Dos16CorrelatedDispatchSmoke.Run();
    Dos16MetamorphicFeedbackSmoke.Run();
    Dos16AlgebraicFeedbackSmoke.Run();
    // Diagnostics are bounded and safe to share by default: no executable
    // contents, source paths, exception messages, private comments or stacks.
    const string diagnosticHash = "fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed";
    var diagnostics = new DiagnosticSession(capacity: 8);
    for (int i = 0; i < 12; i++)
        diagnostics.Record(DiagnosticStage.AnalysisStarted, diagnosticHash, 1, (uint)i);
    Equal(diagnostics.Snapshot().Count, 8, "bounded diagnostic ring size");
    Equal(diagnostics.Snapshot()[0].Sequence, 5L, "oldest diagnostics dropped");
    Equal(diagnostics.Snapshot()[7].Sequence, 12L, "diagnostic sequence stays monotonic");
    diagnostics.Record(DiagnosticStage.HandledError, diagnosticHash,
        error: new IOException("secret-path-C:\\\\private\\\\original.CODE.1"));
    string diagnosticJson = diagnostics.BuildJson();
    using (var parsed = JsonDocument.Parse(diagnosticJson))
    {
        Equal(parsed.RootElement.GetProperty("schemaVersion").GetInt32(), 1,
            "versioned diagnostic schema");
        Equal(parsed.RootElement.GetProperty("droppedEvents").GetInt64(), 5L,
            "diagnostic event drop counter");
        Equal(parsed.RootElement.GetProperty("events").GetArrayLength(), 8,
            "diagnostic event count matches capacity");
        Equal(parsed.RootElement.GetProperty("privacy").GetProperty("originalExecutableIncluded").GetBoolean(),
            false, "diagnostics do not embed game data");
    }
    Equal(diagnosticJson.Contains("secret-path"), false, "exception messages are not serialized");
    Equal(diagnosticJson.Contains("original.CODE.1"), false, "source path excluded from diagnostics");
    Equal(diagnosticJson.Contains("IOException"), true, "exception type retained for debugging");
    bool rejectedDiagnosticHash = false;
    try { diagnostics.Record(DiagnosticStage.FileOpenedRaw, "not-an-original-hash"); }
    catch (ArgumentException) { rejectedDiagnosticHash = true; }
    Equal(rejectedDiagnosticHash, true, "diagnostics reject arbitrary identity text");
    string diagnosticDir = Path.Combine(Path.GetTempPath(), "dosdis-diagnostic-test-" + Guid.NewGuid().ToString("N"));
    Directory.CreateDirectory(diagnosticDir);
    try
    {
        string report = Path.Combine(diagnosticDir, "diagnostics.json");
        diagnostics.WriteReportNew(report);
        string originalReport = File.ReadAllText(report);
        Equal(originalReport.Contains("dosdis-diagnostic-session"), true, "saved diagnostic report readable");
        bool noOverwrite = false;
        try { diagnostics.WriteReportNew(report); }
        catch (IOException) { noOverwrite = true; }
        Equal(noOverwrite, true, "diagnostic export refuses overwrite");
        Equal(File.ReadAllText(report), originalReport, "failed report does not alter previous evidence");
        Equal(Directory.EnumerateFiles(diagnosticDir, "*.part").Any(), false,
            "no staging diagnostic files left behind");
    }
    finally { Directory.Delete(diagnosticDir, recursive: true); }
    Console.WriteLine("PASS: bounded diagnostic events, privacy defaults, atomic export and non-overwriting reports");

    // Arbitrary resources and unsupported binaries remain useful as read-only
    // physical hex, without being misclassified as executable instructions.
    byte[] rawResource = { 0x52, 0x45, 0x53, 0x00, 0xFF, 0x20, 0x7E };
    var resourceSnapshot = BinaryArtifact.FromBytes(rawResource, "raw-resource.dat");
    var resourceHex = RawHexPreview.Render(resourceSnapshot);
    Equal(resourceHex.Contains("file+0x00000000"), true, "raw physical file address");
    Equal(resourceHex.Contains("52 45 53 00 FF 20 7E"), true, "raw byte hex evidence");
    Equal(resourceHex.Contains("RES.. ~"), true, "raw ASCII representation");
    Equal(FileIntelligence.Inspect(resourceSnapshot)["analysis_support"].Status,
        "tool-capability", "unsupported file reports actual analyzer scope");
    Equal(FileIntelligence.Inspect(resourceSnapshot).ContainsKey("minimum_dos_version"),
        false, "raw data file must not inherit DOS executable assumptions");
    rawResource[0] = 0x00;
    Equal(resourceSnapshot.Data[0], (byte)0x52, "raw hex view never mutates source");
    Equal(RawHexPreview.Render(BinaryArtifact.FromBytes(Array.Empty<byte>())),
        "(empty file; no bytes to inspect)", "empty raw file clearly identified");
    var sixteenPlus = BinaryArtifact.FromBytes(Enumerable.Range(0, 17).Select(x => (byte)x).ToArray());
    Equal(RawHexPreview.Render(sixteenPlus, 16, 1).StartsWith("file+0x00000010"),
        true, "raw file seeking honors absolute offsets");
    bool invalidRawOffset = false;
    try { RawHexPreview.Render(sixteenPlus, 17); }
    catch (ArgumentOutOfRangeException) { invalidRawOffset = true; }
    Equal(invalidRawOffset, true, "reject past-EOF raw seek");
    Console.WriteLine("PASS: raw bytes, EOF, physical-offset hex navigation and unsupported-format capability label");

    // P0: signature detection is never permission to decode an overlay,
    // resource container, LX module or headerless COM driver as flat x86.
    foreach (var candidate in new[] {
        (Signature: "FBOV", Name: "GAME.OVR", Expected: "fbov"),
        (Signature: "TPOV", Name: "GAME.OVR", Expected: "tpov"),
        (Signature: "AESOP/16 V1.00", Name: "HACK.RES", Expected: "aesop-resource"),
        (Signature: "GFFI", Name: "RESOURCE.GFF", Expected: "gff-container")
    })
    {
        var sample = BinaryArtifact.FromBytes(System.Text.Encoding.ASCII.GetBytes(candidate.Signature),
            candidate.Name);
        var detected = BinaryFormatDetector.Inspect(sample);
        Equal(detected.FormatId, candidate.Expected, "signature-first container type");
        Equal(detected.LoaderStatus.EndsWith("-required", StringComparison.Ordinal), true,
            "known container is not passed to an unvalidated x86 loader");
    }
    var comCandidate = BinaryArtifact.FromBytes(new byte[] { 0xB8, 0, 0x4C, 0xCD, 0x21 },
        "SOUND.COM");
    Equal(BinaryFormatDetector.Inspect(comCandidate).FormatId, "unknown-headerless",
        "a COM extension alone never proves a flat load context");
    var comAssumed = DosComImage.LoadExplicit(comCandidate, 0x100);
    Equal(comAssumed.ToFileOffset(0x100), 0L,
        "explicit COM origin maps the first original file byte");
    Equal(comAssumed.ToFileOffset(0x102), 2L,
        "COM image-relative addresses preserve physical source offsets");
    Equal(comAssumed.ToFileOffset(0x0FF), (long?)null,
        "COM PSP bytes before the declared origin are never invented");
    var comInstructions = comAssumed.Decode(0x100, 2);
    Equal(comInstructions.Count, 2, "COM 16-bit mode decodes requested instructions only");
    Equal(comInstructions[0].OriginalBytes, "B8004C", "COM x86-16 MOV immediate bytes");
    Equal(comInstructions[1].OriginalBytes, "CD21", "COM DOS INT instruction bytes");
    bool comPastSourceRejected = false;
    try { comAssumed.Decode(0x0FF); }
    catch (ArgumentOutOfRangeException) { comPastSourceRejected = true; }
    Equal(comPastSourceRejected, true, "COM cannot decode outside declared image origin");
    Fails(() => DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(new byte[0x10000]), 0x100),
        "oversized single-segment COM image cannot silently wrap memory");

    byte[] mzBytes = new byte[512];
    mzBytes[0] = (byte)'M'; mzBytes[1] = (byte)'Z';
    static void MzWord(byte[] data, int at, ushort value) =>
        BinaryPrimitives.WriteUInt16LittleEndian(data.AsSpan(at, 2), value);
    MzWord(mzBytes, 2, 0);      // final page fully occupied
    MzWord(mzBytes, 4, 1);      // 512 bytes in the declared MZ file
    MzWord(mzBytes, 6, 1);      // one internal relocation source
    MzWord(mzBytes, 8, 2);      // 32-byte DOS MZ header
    MzWord(mzBytes, 0x14, 0);   // CS:IP = 0000:0000
    MzWord(mzBytes, 0x16, 0);
    MzWord(mzBytes, 0x18, 0x1C); // relocation entries at 0x1C
    MzWord(mzBytes, 0x1C, 1);   // 0000:0001 (imm16 of MOV)
    MzWord(mzBytes, 0x1E, 0);
    mzBytes[0x20] = 0xB8; mzBytes[0x21] = 0x34; mzBytes[0x22] = 0x12;
    mzBytes[0x23] = 0xC3; // mov ax,1234h; ret
    var mzArtifact = BinaryArtifact.FromBytes(mzBytes, "MAIN.EXE");
    Equal(BinaryFormatDetector.Inspect(mzArtifact).FormatId, "mz",
        "standalone DOS MZ signature identified");
    var mzImage = DosMzImage.Load(mzArtifact);
    Equal(mzImage.LoadImageFileOffset, 32, "DOS header paragraph mapping");
    Equal(mzImage.FileBackedImageLength, 480, "MZ declared load module length");
    Equal(mzImage.Relocations.Count, 1, "MZ relocation count");
    Equal(mzImage.Relocations[0].SourceFileOffset, 33L, "MZ relocation original file offset");
    Equal(mzImage.ToFileOffset(0, 0), 32L, "DOS 16-bit segment:offset source mapping");
    Equal(mzImage.ToFileOffset(0x30, 0), (long?)null, "unbacked MZ address cannot map to a fictional file offset");
    var mzLines = mzImage.Decode(0, 0, 2);
    Equal(mzLines.Count, 2, "requested 16-bit linear candidate count");
    Equal(mzLines[0].OriginalBytes, "B83412", "16-bit MOV immediate instruction bytes");
    Equal(mzLines[1].OriginalBytes, "C3", "16-bit return instruction bytes");
    Equal(mzLines[1].SourceFileOffset, 35L, "second instruction source offset");
    // DOS MZ relocation values are simulated in table order, never
    // applied to the archived original executable.
    var simulatedMz = mzImage.SimulateRelocations(0x2000);
    Equal(simulatedMz.Summary.SourceSha256, mzArtifact.Sha256,
        "MZ simulation preserves exact source identity");
    Equal(simulatedMz.Summary.AssumedEntryCs, (ushort)0x2000,
        "assumed runtime entry segment recorded without changing original CS");
    Equal(simulatedMz.Summary.RelocationWriteCount, 1,
        "DOS loader relocation table entries are modeled explicitly");
    Equal(simulatedMz.Summary.Writes[0].OriginalStoredWord, (ushort)0x1234,
        "MZ fixup preserves the original selector word");
    Equal(simulatedMz.Summary.Writes[0].ValueAfterWrite, (ushort)0x3234,
        "MZ relocation adds the assumed load segment");
    Equal(simulatedMz.ReadRelocatedWord(1), (ushort)0x3234,
        "simulated load-module copy contains relocated bytes");
    Equal(mzImage.Decode(0, 0, 1)[0].OriginalBytes, "B83412",
        "non-destructive relocation must not edit the original binary");
    Equal(simulatedMz.MapRuntimeFarPointer(0x2000, 0)?.OriginalFileOffset, 32L,
        "nonwrapped MZ runtime selector maps to the original file");
    Equal(simulatedMz.MapRuntimeFarPointer(0x1FFF, 0), null,
        "wrapped or below-base runtime selectors are not guessed");
    Equal(simulatedMz.MapRuntimeFarPointer(0x2000, 0xFFFF), null,
        "runtime mapping never invents file-backed bytes");
    Equal(mzImage.SimulateRelocations(0xF000).ReadRelocatedWord(1), (ushort)0x0234,
        "DOS MZ loader additions use 16-bit modular arithmetic");
    var nearA20Boundary = mzImage.SimulateRelocations(0xFFF0);
    Equal(nearA20Boundary.MapRuntimeFarPointer(0xFFF0, 0xFF)?.ImageOffset, 255,
        "last nonwrapped 20-bit DOS address can map to backed original bytes");
    Equal(nearA20Boundary.MapRuntimeFarPointer(0xFFF0, 0x100), null,
        "A20-dependent or 1 MiB-wrapped far pointers remain unresolved");

    // Loader table order is important: a repeated relocation location is
    // accounted for twice and prominently flagged as overlapping evidence.
    byte[] doubleFixup = (byte[])mzBytes.Clone();
    MzWord(doubleFixup, 8, 3); // relocation table has space in 48-byte header
    MzWord(doubleFixup, 6, 2);
    MzWord(doubleFixup, 0x20, 1); MzWord(doubleFixup, 0x22, 0);
    doubleFixup[0x30] = 0xB8; MzWord(doubleFixup, 0x31, 0x1234);
    doubleFixup[0x33] = 0xC3;
    var repeatedImage = DosMzImage.Load(BinaryArtifact.FromBytes(doubleFixup));
    var repeated = repeatedImage.SimulateRelocations(0x2000);
    Equal(repeated.Summary.OverlappingRelocationWrites, true,
        "duplicate MZ relocation writes are not silently collapsed");
    Equal(repeated.Summary.Writes[1].ValueBeforeWrite, (ushort)0x3234,
        "second fixup observes the first applied value");
    Equal(repeated.ReadRelocatedWord(1), (ushort)0x5234,
        "repeated original MZ fixup is modeled in table order");

    // Unprefixed 16:16 far CALL/JMP requires exactly one resident MZ selector
    // relocation. The candidate never replaces an unresolved runtime edge.
    byte[] farBytes = (byte[])mzBytes.Clone();
    MzWord(farBytes, 0x1C, 3); // selector imm16 starts at source image offset 3
    new byte[] { 0x9A, 0x10, 0, 0, 0, 0xC3 }.CopyTo(farBytes, 0x20);
    farBytes[0x30] = 0xC3;
    var farImage = DosMzImage.Load(BinaryArtifact.FromBytes(farBytes));
    var farGraph = Dos16ControlFlowAnalysis.Analyze(
        farImage.CreateEntryCodeRegion(), farImage.InitialIp);
    var farView = farImage.SimulateRelocations(0x2000);
    var farCandidates = DosMzFarTargetRecovery.Inspect(farImage, farGraph, farView);
    Equal(farCandidates.Count, 1, "unique source-backed relocated FAR CALL candidate recovered");
    Equal(farCandidates[0].TransferKind, "far-immediate-call",
        "far candidate keeps transfer type");
    Equal(farCandidates[0].RelativeTarget, new Dos16Address(0, 0x10),
        "relocation-backed far CALL resolves original module-relative location");
    Equal(farCandidates[0].TargetFileOffset, 0x30L,
        "relocation-backed FAR target maps to original file bytes");
    Equal(farGraph.Edges.Any(e => e.Kind == "unresolved-far-call"),
        true, "original FAR CFG edge remains conservatively unresolved");
    // FAR relocation bytes do not license a fabricated CFG address or
    // an unrelated MZ segment window. An old graph may share its full
    // source SHA while pointing at the wrong original-file offset.
    Fails(() => DosMzFarTargetRecovery.Inspect(farImage,
        farGraph with { FileOffset = farGraph.FileOffset + 1 }, farView),
        "FAR recovery rejects a misaligned original code-window offset");
    Fails(() => DosMzFarTargetRecovery.Inspect(farImage,
        farGraph with { RegionByteLength = farGraph.RegionByteLength - 1 },
        farView), "FAR recovery rejects a stale source-backed window length");
    Fails(() => DosMzFarTargetRecovery.Inspect(farImage,
        farGraph with { MappingEvidence = "forged-MZ-mapping" }, farView),
        "FAR recovery rejects borrowed source-mapping evidence");
    Fails(() => DosMzFarTargetRecovery.Inspect(farImage,
        farGraph with { Entry = new Dos16Address(1, 0) }, farView),
        "FAR recovery rejects an unrelated MZ relative CS graph");

    var forgedFarAddress = farGraph with {
        Instructions = farGraph.Instructions.Select(insn =>
            insn.Length == 5
                ? insn with { Address = new Dos16Address(insn.Address.Segment, 1) }
                : insn).ToArray()
    };
    Equal(DosMzFarTargetRecovery.Inspect(
        farImage, forgedFarAddress, farView).Count, 0,
        "relocated FAR bytes cannot be attributed to a forged instruction IP");

    var staleFarEdge = farGraph with {
        Edges = farGraph.Edges.Select(e =>
            e.Kind == "unresolved-far-call"
                ? e with { Kind = "call" }
                : e).ToArray()
    };
    Equal(DosMzFarTargetRecovery.Inspect(
        farImage, staleFarEdge, farView).Count, 0,
        "resolved-looking graph edge cannot replace an unresolved FAR call");
    var missingFarEdge = farGraph with {
        Edges = farGraph.Edges.Where(e =>
            e.Kind != "unresolved-far-call").ToArray()
    };
    Equal(DosMzFarTargetRecovery.Inspect(
        farImage, missingFarEdge, farView).Count, 0,
        "missing machine-backed FAR edge blocks target annotations");

    Equal(DosMzFarTargetRecovery.Inspect(farImage,
        farGraph with { Truncated = true }, farView).Count, 0,
        "truncated candidate graph cannot publish FAR traversal destinations");
    var shortCrossSegment = DosMzCrossSegmentAnalysis.Analyze(
        farImage, 0x2000, maxEntryGraphs: 2, instructionsPerEntry: 1);
    Equal(shortCrossSegment.Truncated, true,
        "instruction-limited cross-segment review reports truncation");
    Equal(shortCrossSegment.CrossSegmentCandidates.Count, 0,
        "instruction-limited review retains no partially inferred FAR targets");
    // Two independently reached CS:IP addresses may map to the same
    // original file byte. They are an explicit physical alias, not two
    // contradictory decodings and not a verified runtime equivalence.
    // Near CALL reaches 0000:0010, while relocated FAR JMP reaches
    // 0001:0000; both address the same physical original RET byte.
    byte[] physicalAliasBytes = (byte[])mzBytes.Clone();
    new byte[] {
        0xE8, 0x0D, 0x00,       // CALL 0000:0010
        0xEA, 0x00, 0x00, 0x01, 0x00  // JMP FAR 0001:0000
    }.CopyTo(physicalAliasBytes, 0x20);
    physicalAliasBytes[0x30] = 0xC3;
    MzWord(physicalAliasBytes, 0x1C, 6); // relocation of FAR selector word
    var physicalAliasMz = DosMzImage.Load(
        BinaryArtifact.FromBytes(physicalAliasBytes, "alias-only-synthetic.exe"));
    var physicalAliasReview = DosMzCrossSegmentAnalysis.Analyze(
        physicalAliasMz, 0x2000, maxEntryGraphs: 4, instructionsPerEntry: 16);
    Equal(physicalAliasReview.Truncated, false,
        "two full source-backed MZ alias graphs are within analysis budgets");
    Equal(physicalAliasReview.CrossSegmentCandidates.Count, 1,
        "original selector relocation independently supplies FAR alias entry");
    Equal(physicalAliasReview.IndependentEntryGraphs.Count, 2,
        "far candidate destination remains a separately analyzed code window");
    Equal(physicalAliasReview.PhysicalConflicts.Count, 0,
        "identical physical RET bytes do not fabricate a decoding conflict");
    Equal(physicalAliasReview.PhysicalAliases.Count, 1,
        "distinct segment-relative IP aliases are retained explicitly");
    Equal(physicalAliasReview.PhysicalAliases[0].OriginalFileOffset, 0x30L,
        "physical alias retains the original file-backed address");
    Equal(physicalAliasReview.PhysicalAliases[0].First, new Dos16Address(0, 0x10),
        "first aliased instruction retains its initial segment address");
    Equal(physicalAliasReview.PhysicalAliases[0].Alternate,
        new Dos16Address(1, 0),
        "second aliased instruction retains independent relative CS:IP");
    Equal(physicalAliasReview.PhysicalAliases[0].Status.Contains(
        "runtime-equivalence-unverified"), true,
        "alias reporting never promotes alternate CS:IP to verified execution");
    // Synthetic CS-memory pointer recovery, never CFG reachability proof.
    var nearPointer = new byte[0x24];
    new byte[] { 0x2E, 0xFF, 0x26, 0x10, 0x01 }.CopyTo(nearPointer, 0);
    MzWord(nearPointer, 0x10, 0x120);
    nearPointer[0x20] = 0xC3;
    var indirectCom = DosComImage.LoadExplicit(BinaryArtifact.FromBytes(nearPointer));
    var indirectRegion = indirectCom.CreateCodeRegion();
    var indirectGraph = Dos16ControlFlowAnalysis.Analyze(indirectRegion, 0x100);
    var nearPointers = Dos16IndirectPointerRecovery.Inspect(indirectRegion, indirectGraph);
    Equal(nearPointers.IndirectSites, 1, "CS near pointer site counted");
    Equal(nearPointers.CandidateSites, 1, "CS direct memory pointer candidate");
    Equal(nearPointers.Candidates[0].Target, new Dos16Address(0, 0x120),
        "CS near pointer target remains candidate");
    Equal(nearPointers.Candidates[0].TargetFileOffset, 0x20L,
        "CS near pointer destination maps original source bytes");
    var mzNearFixup = (byte[])mzBytes.Clone();
    new byte[] { 0x2E, 0xFF, 0x26, 0x10, 0x00 }
        .CopyTo(mzNearFixup, 0x20); // CS:[0010h]
    MzWord(mzNearFixup, 0x30, 0x20);
    mzNearFixup[0x40] = 0xC3;
    MzWord(mzNearFixup, 0x1C, 0x10); // MZ loader relocates near word itself
    var mzNearSource = DosMzImage.Load(BinaryArtifact.FromBytes(mzNearFixup));
    var mzNearRegion = mzNearSource.CreateEntryCodeRegion();
    var mzNearGraph = Dos16ControlFlowAnalysis.Analyze(mzNearRegion, 0);
    Equal(Dos16IndirectPointerRecovery.Inspect(mzNearRegion, mzNearGraph,
        mzNearSource).CandidateSites, 0,
        "near CS pointer with overlapping MZ loader fixup is not source-stable");

    var mismatchedOpcodeLine = indirectGraph with {
        Instructions = indirectGraph.Instructions.Select(x =>
            x with { OriginalBytes = "DEADBEEF00" }).ToArray()
    };
    Equal(Dos16IndirectPointerRecovery.Inspect(
        indirectRegion, mismatchedOpcodeLine).CandidateSites, 0,
        "stale candidate instruction bytes cannot license source-backed pointer targets");
    Equal(indirectGraph.Instructions.Count, 1, "indirect candidate does not alter CFG");
    var dsBytes = (byte[])nearPointer.Clone();
    dsBytes[0] = 0x90;
    var ds = DosComImage.LoadExplicit(BinaryArtifact.FromBytes(dsBytes));
    var dsRegion = ds.CreateCodeRegion();
    Equal(Dos16IndirectPointerRecovery.Inspect(dsRegion,
        Dos16ControlFlowAnalysis.Analyze(dsRegion, 0x101)).CandidateSites,
        0, "DS memory pointer is not silently considered CS");
    var indexed = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(new byte[] { 0x2E, 0xFF, 0x27, 0xC3 }));
    Equal(Dos16IndirectPointerRecovery.Inspect(indexed.CreateCodeRegion(),
        Dos16ControlFlowAnalysis.Analyze(indexed.CreateCodeRegion(), 0x100)).CandidateSites,
        0, "CS indexed pointer requires separate bounded dataflow");
    Fails(() => Dos16IndirectPointerRecovery.Inspect(
        DosComImage.LoadExplicit(BinaryArtifact.FromBytes(new byte[] { 0xC3 })).CreateCodeRegion(),
        indirectGraph), "indirect evidence cannot be transferred to another source hash");

    var farMemoryBytes = (byte[])mzBytes.Clone();
    new byte[] { 0x2E, 0xFF, 0x1E, 0x10, 0x00, 0xC3 }
        .CopyTo(farMemoryBytes, 0x20);
    MzWord(farMemoryBytes, 0x30, 0x20);
    MzWord(farMemoryBytes, 0x32, 0);
    MzWord(farMemoryBytes, 0x1C, 0x12);
    farMemoryBytes[0x40] = 0xC3;
    var farMemoryMz = DosMzImage.Load(BinaryArtifact.FromBytes(farMemoryBytes));
    var farMemoryRegion = farMemoryMz.CreateEntryCodeRegion();
    var farMemoryGraph = Dos16ControlFlowAnalysis.Analyze(farMemoryRegion, 0);
    Equal(Dos16IndirectPointerRecovery.Inspect(farMemoryRegion,
        farMemoryGraph).CandidateSites, 0,
        "CS FAR memory pointer cannot be claimed without a load base");
    var farMemoryPointers = Dos16IndirectPointerRecovery.Inspect(farMemoryRegion,
        farMemoryGraph, farMemoryMz, farMemoryMz.SimulateRelocations(0x2000));
    Equal(farMemoryPointers.CandidateSites, 1,
        "source-backed CS FAR pointer selector fixup candidate");
    Equal(farMemoryPointers.Candidates[0].SelectorFixupOrdinal, (int?)0,
        "MZ FAR selector fixup ordinal preserved");
    Equal(farMemoryPointers.Candidates[0].TargetFileOffset, 0x40L,
        "relocated FAR memory pointer resolves backed original bytes");
    var wrongSelectorBytes = (byte[])farMemoryBytes.Clone();
    MzWord(wrongSelectorBytes, 0x1C, 0x18);
    var wrongSelectorMz = DosMzImage.Load(BinaryArtifact.FromBytes(wrongSelectorBytes));
    Equal(Dos16IndirectPointerRecovery.Inspect(
        wrongSelectorMz.CreateEntryCodeRegion(),
        Dos16ControlFlowAnalysis.Analyze(wrongSelectorMz.CreateEntryCodeRegion(), 0),
        wrongSelectorMz, wrongSelectorMz.SimulateRelocations(0x2000)).CandidateSites,
        0, "unrelated MZ fixup does not prove CS FAR pointer");
    Fails(() => Dos16IndirectPointerRecovery.Inspect(indirectRegion, indirectGraph,
        farMemoryMz, farMemoryMz.SimulateRelocations(0x2000)),
        "foreign MZ selector evidence must not cross source boundaries");
    Console.WriteLine("PASS: CS near and relocation-backed FAR indirect pointer hypotheses");
    // A count supplied by an analyst permits bounded inspection but is
    // NOT a verified runtime upper bound or an automatic CFG edge.
    var nearDispatchBytes = new byte[0x40];
    new byte[] { 0x2E, 0xFF, 0xA7, 0x20, 0x01 }
        .CopyTo(nearDispatchBytes, 0); // JMP WORD PTR CS:[BX+0120h]
    MzWord(nearDispatchBytes, 0x20, 0x130);
    MzWord(nearDispatchBytes, 0x22, 0x131);
    MzWord(nearDispatchBytes, 0x24, 0x132);
    nearDispatchBytes[0x30] = nearDispatchBytes[0x31] =
        nearDispatchBytes[0x32] = 0xC3;
    var nearDispatchSource = BinaryArtifact.FromBytes(nearDispatchBytes, "anonymous.bin");
    var nearDispatchRegion = DosComImage.LoadExplicit(nearDispatchSource).CreateCodeRegion();
    var nearDispatchGraph = Dos16ControlFlowAnalysis.Analyze(nearDispatchRegion, 0x100);
    var nearDispatchManifest = new Dos16TableManifest(1, nearDispatchSource.Sha256,
        new[] { new Dos16TableRequest(0x100, 0x120, 3, "near16", "synthetic-analyst-bounds") });
    var nearDispatch = Dos16DispatchTableRecovery.Inspect(
        nearDispatchRegion, nearDispatchGraph, nearDispatchManifest);
    Equal(nearDispatch.Candidates, 1, "explicit bounded CS-indexed near dispatch table");
    Equal(nearDispatch.Findings[0].Entries.Count, 3,
        "every near table entry is source-backed and counted");
    Equal(nearDispatch.Findings[0].Entries[2].Target.Offset, (ushort)0x132,
        "last near table target retains original 16-bit offset");
    Equal(nearDispatch.Findings[0].Entries[0].PointerFileOffset, 0x20L,
        "first dispatch table entry retains original file offset");
    var forgedIndexedSite = nearDispatchGraph with {
        Instructions = nearDispatchGraph.Instructions.Select(x =>
            x with { SourceFileOffset = x.SourceFileOffset + 1 }).ToArray()
    };
    Equal(Dos16DispatchTableRecovery.Inspect(nearDispatchRegion,
        forgedIndexedSite, nearDispatchManifest).Candidates, 0,
        "stale indexed site file offsets cannot recover otherwise backed tables");
    Equal(nearDispatchGraph.Instructions.Count, 1,
        "analyst table targets do not expand recursive code reachability");
    var invalidNearBounds = nearDispatchManifest with {
        Tables = new[] { new Dos16TableRequest(0x100, 0x120, 4, "near16", "bound-unproven") } };
    Equal(Dos16DispatchTableRecovery.Inspect(nearDispatchRegion, nearDispatchGraph,
        invalidNearBounds).Candidates, 0,
        "incomplete dispatch table cannot emit a partially validated target list");
    Fails(() => Dos16DispatchTableRecovery.Inspect(nearDispatchRegion, nearDispatchGraph,
        nearDispatchManifest with { SourceSha256 = new string('f', 64) }),
        "indexed table manifest must match full source SHA-256");
    var serializedNearManifest = JsonSerializer.Serialize(nearDispatchManifest,
        new JsonSerializerOptions(JsonSerializerDefaults.Web));
    Equal(Dos16DispatchTableRecovery.ParseManifest(serializedNearManifest).Tables.Count,
        1, "source-bound indexed table manifests round trip");

    var farDispatchBytes = (byte[])mzBytes.Clone();
    MzWord(farDispatchBytes, 8, 3); // 48-byte MZ header for two fixups
    MzWord(farDispatchBytes, 6, 2);
    MzWord(farDispatchBytes, 0x1C, 0x32); MzWord(farDispatchBytes, 0x1E, 0);
    MzWord(farDispatchBytes, 0x20, 0x36); MzWord(farDispatchBytes, 0x22, 0);
    new byte[] { 0x2E, 0xFF, 0xAF, 0x30, 0x00 }
        .CopyTo(farDispatchBytes, 0x30); // JMP FAR PTR CS:[BX+0030h]
    MzWord(farDispatchBytes, 0x60, 0x40); MzWord(farDispatchBytes, 0x62, 0);
    MzWord(farDispatchBytes, 0x64, 0x42); MzWord(farDispatchBytes, 0x66, 0);
    farDispatchBytes[0x70] = farDispatchBytes[0x72] = 0xC3;
    var farDispatchMz = DosMzImage.Load(BinaryArtifact.FromBytes(farDispatchBytes));
    var farDispatchRegion = farDispatchMz.CreateEntryCodeRegion();
    var farDispatchGraph = Dos16ControlFlowAnalysis.Analyze(farDispatchRegion, 0);
    var farDispatchManifest = new Dos16TableManifest(1, farDispatchMz.SourceSha256,
        new[] { new Dos16TableRequest(0, 0x30, 2, "far16", "synthetic-mz-pointer-table") });
    var farDispatch = Dos16DispatchTableRecovery.Inspect(farDispatchRegion,
        farDispatchGraph, farDispatchManifest, farDispatchMz,
        farDispatchMz.SimulateRelocations(0x2000));
    Equal(farDispatch.Candidates, 1,
        "all FAR indexed entries require unique MZ selector relocation evidence");
    Equal(farDispatch.Findings[0].Entries.Count, 2,
        "both independently relocated far pointer entries validate");
    Equal(farDispatch.Findings[0].Entries[1].FixupOrdinal, (int?)1,
        "second table entry retains distinct MZ selector fixup");
    Equal(Dos16DispatchTableRecovery.Inspect(farDispatchRegion,
        farDispatchGraph, farDispatchManifest).Candidates, 0,
        "FAR dispatch table requires MZ relocation and explicit load-base evidence");
    Equal(Dos16DispatchTableRecovery.Inspect(farDispatchRegion,
        farDispatchGraph, farDispatchManifest, farDispatchMz).Candidates, 0,
        "MZ geometry alone cannot invent relocated FAR table selectors");
    var farMissingFixupBytes = (byte[])farDispatchBytes.Clone();
    MzWord(farMissingFixupBytes, 0x20, 0x38);
    var farMissingFixupMz = DosMzImage.Load(
        BinaryArtifact.FromBytes(farMissingFixupBytes));
    Equal(Dos16DispatchTableRecovery.Inspect(
        farMissingFixupMz.CreateEntryCodeRegion(),
        Dos16ControlFlowAnalysis.Analyze(farMissingFixupMz.CreateEntryCodeRegion(), 0),
        farDispatchManifest with { SourceSha256 = farMissingFixupMz.SourceSha256 },
        farMissingFixupMz, farMissingFixupMz.SimulateRelocations(0x2000)).Candidates,
        0, "one unrelocated table selector rejects all candidate table targets");
    Console.WriteLine("PASS: evidence-bound CS-indexed near and FAR table candidates");

    // A genuinely guarded indexed dispatch: CMP BX,2; unsigned JA default;
    // SHL BX,1; JMP word ptr CS:[BX+120h]. No manual table manifest exists.
    var automaticBytes = new byte[0x40];
    new byte[] { 0x83, 0xFB, 0x02, 0x77, 0x07, 0xD1, 0xE3,
        0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3 }.CopyTo(automaticBytes, 0);
    MzWord(automaticBytes, 0x20, 0x130);
    MzWord(automaticBytes, 0x22, 0x131);
    MzWord(automaticBytes, 0x24, 0x132);
    automaticBytes[0x30] = automaticBytes[0x31] =
        automaticBytes[0x32] = 0xC3;
    var automaticSource = BinaryArtifact.FromBytes(automaticBytes, "unidentified-headerless.bin");
    var automaticRegion = DosComImage.LoadExplicit(automaticSource).CreateCodeRegion();
    var automaticGraph = Dos16ControlFlowAnalysis.Analyze(automaticRegion, 0x100);
    var automaticTables = Dos16AutomaticDispatchRecovery.Inspect(automaticRegion, automaticGraph);
    Equal(automaticTables.GuardedTableCandidates, 1,
        "unsigned BX guard and one-shift produces an automatic table candidate");
    Equal(automaticTables.Sites[0].EntryCount, (int?)3,
        "CMP BX,2 and JA give three nonnegative unsigned index possibilities");
    Equal(automaticTables.Sites[0].StoredTable?.Entries.Count, 3,
        "all three entries validated against original source bytes");
    var addBxBx = (byte[])automaticBytes.Clone();
    addBxBx[5] = 0x01; addBxBx[6] = 0xDB; // ADD BX,BX: r/m16, r16
    var addDouble = DosComImage.LoadExplicit(BinaryArtifact.FromBytes(addBxBx));
    Equal(Dos16AutomaticDispatchRecovery.Inspect(addDouble.CreateCodeRegion(),
        Dos16ControlFlowAnalysis.Analyze(addDouble.CreateCodeRegion(), 0x100))
        .GuardedTableCandidates, 1,
        "exact ADD BX,BX doubles the 16-bit near index after the unsigned guard");
    addBxBx[5] = 0x03; // ADD BX,BX: r16, r/m16 alternate encoding
    var addAlternate = DosComImage.LoadExplicit(BinaryArtifact.FromBytes(addBxBx));
    Equal(Dos16AutomaticDispatchRecovery.Inspect(addAlternate.CreateCodeRegion(),
        Dos16ControlFlowAnalysis.Analyze(addAlternate.CreateCodeRegion(), 0x100))
        .GuardedTableCandidates, 1,
        "alternate ADD BX,BX form preserves same near-pointer stride");
    addBxBx[6] = 0xCB; // ADD BX,CX does not double a known BX index
    var wrongAdd = DosComImage.LoadExplicit(BinaryArtifact.FromBytes(addBxBx));
    Equal(Dos16AutomaticDispatchRecovery.Inspect(wrongAdd.CreateCodeRegion(),
        Dos16ControlFlowAnalysis.Analyze(wrongAdd.CreateCodeRegion(), 0x100))
        .GuardedTableCandidates, 0,
        "adding another register cannot be treated as proven BX scaling");

    // A source-backed unique predecessor can include only bounded NOP
    // or direct unconditional jump bridges. These are intentionally
    // separate from the immediate-adjacency tests above.
    (Dos16CodeRegion Region, Dos16FlowResult Graph,
        Dos16GuardedDispatchReport Report) AnalyzeBridgedTable(byte[] instructions)
    {
        byte[] input = new byte[0x40];
        instructions.CopyTo(input, 0);
        MzWord(input, 0x20, 0x130);
        MzWord(input, 0x22, 0x131);
        MzWord(input, 0x24, 0x132);
        input[0x30] = input[0x31] = input[0x32] = 0xC3;
        var region = DosComImage.LoadExplicit(
            BinaryArtifact.FromBytes(input, "anonymous-com.bin")).CreateCodeRegion();
        var graph = Dos16ControlFlowAnalysis.Analyze(region, 0x100);
        return (region, graph,
            Dos16AutomaticDispatchRecovery.Inspect(region, graph));
    }

    // In-range branch guards can select the table instead of rejecting
    // out-of-range indexes. The branch target must be byte-backed,
    // uniquely reached, and its opposite path a distinct decoded exit.
    var jbeTaken = AnalyzeBridgedTable(new byte[] {
        0x83, 0xFB, 0x02, 0x76, 0x03, // CMP BX,2; JBE +3 (to stride)
        0xC3, 0x90, 0x90,             // false path returns
        0xD1, 0xE3,                   // true: SHL BX,1
        0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });
    Equal(jbeTaken.Report.GuardedTableCandidates, 1,
        "JBE taken edge proves inclusive unsigned index bound");
    Equal(jbeTaken.Report.Sites[0].EntryCount, (int?)3,
        "CMP BX,2 plus JBE accepts exactly 0,1,2");
    Equal(jbeTaken.Report.Sites[0].StoredTable?.Entries.Count, 3,
        "selected in-range branch validates every stored pointer");

    // Even the rejected side of a guard must be backed by valid original
    // decoded instruction bytes, not merely a matching graph address.
    var staleFailureExit = jbeTaken.Graph with {
        Instructions = jbeTaken.Graph.Instructions.Select(i =>
            i.Address.Offset == 0x105
                ? i with { SourceFileOffset = long.MaxValue } : i).ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(
        jbeTaken.Region, staleFailureExit).GuardedTableCandidates, 0,
        "unsigned in-range dispatch rejects an exit with a stale physical offset");
    var rewrittenFailureExit = jbeTaken.Graph with {
        Instructions = jbeTaken.Graph.Instructions.Select(i =>
            i.Address.Offset == 0x105
                ? i with { OriginalBytes = "90" } : i).ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(
        jbeTaken.Region, rewrittenFailureExit).GuardedTableCandidates, 0,
        "unsigned in-range dispatch rejects byte-spoofed exit instructions");
    var duplicatedFailureExit = jbeTaken.Graph with {
        Instructions = jbeTaken.Graph.Instructions.Concat(
            jbeTaken.Graph.Instructions.Where(i =>
                i.Address.Offset == 0x105)).ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(
        jbeTaken.Region, duplicatedFailureExit).GuardedTableCandidates, 0,
        "duplicate exit instruction claims cannot satisfy unique guard proof");
    Equal(Dos16UniqueTransparentPath.UpstreamCandidates(
        jbeTaken.Region, duplicatedFailureExit,
        new Dos16Address(0, 0x108)).Count, 0,
        "duplicate decoded starts must fail closed before predecessor traversal");
    var duplicatedGuardInstruction = automaticGraph with {
        Instructions = automaticGraph.Instructions.Concat(
            automaticGraph.Instructions.Where(i =>
                i.Address.Offset == 0x103)).ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(
        automaticRegion, duplicatedGuardInstruction).GuardedTableCandidates, 0,
        "duplicate decoded guard instructions cannot authorize automatic tables");


    // Identical file bytes with a forged control-flow label must not be
    // accepted as a valid CMP, scale, transparent NOP or rejected branch.
    // Those graph rows may arise from stale or externally edited snapshots.
    var retypedExit = jbeTaken.Graph with {
        Instructions = jbeTaken.Graph.Instructions.Select(i =>
            i.Address.Offset == 0x105
                ? i with { FlowControl = "Next" } : i).ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(
        jbeTaken.Region, retypedExit).GuardedTableCandidates, 0,
        "original RET byte cannot masquerade as a sequential exit");

    var retypedCompare = automaticGraph with {
        Instructions = automaticGraph.Instructions.Select(i =>
            i.Address.Offset == 0x100
                ? i with { FlowControl = "IndirectCall" } : i).ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(
        automaticRegion, retypedCompare).GuardedTableCandidates, 0,
        "source-backed CMP with falsified indirect-call label cannot prove bounds");
    var retypedStride = automaticGraph with {
        Instructions = automaticGraph.Instructions.Select(i =>
            i.Address.Offset == 0x105
                ? i with { FlowControl = "Return" } : i).ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(
        automaticRegion, retypedStride).GuardedTableCandidates, 0,
        "source-backed SHL with falsified return label must invalidate stride proof");
    // CMP; JA default; SHL BX,1; NOP; CS-indexed JMP.
    var nopBeforeTable = AnalyzeBridgedTable(new byte[] {
        0x83, 0xFB, 0x02, 0x77, 0x08,
        0xD1, 0xE3, 0x90, 0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });

    var retypedNop = nopBeforeTable.Graph with {
        Instructions = nopBeforeTable.Graph.Instructions.Select(i =>
            i.Address.Offset == 0x107
                ? i with { FlowControl = "IndirectBranch" } : i).ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(
        nopBeforeTable.Region, retypedNop).GuardedTableCandidates, 0,
        "original NOP with forged indirect jump label cannot bridge the path");
    Equal(Dos16UniqueTransparentPath.IsOriginalDecoded(
        nopBeforeTable.Region, nopBeforeTable.Graph.Instructions
            .Single(i => i.Address.Offset == 0x107)), true,
        "unchanged original NOP can pass exact decoded instruction proof");
    Equal(Dos16UniqueTransparentPath.IsOriginalDecoded(
        nopBeforeTable.Region, retypedNop.Instructions
            .Single(i => i.Address.Offset == 0x107)), false,
        "re-decoder explicitly rejects a forged control flow classification");



    var jbTaken = AnalyzeBridgedTable(new byte[] {
        0x83, 0xFB, 0x03, 0x72, 0x03, // CMP BX,3; JB +3
        0xC3, 0x90, 0x90,
        0xD1, 0xE3,
        0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });
    Equal(jbTaken.Report.GuardedTableCandidates, 1,
        "JB taken edge proves exclusive unsigned index bound");
    Equal(jbTaken.Report.Sites[0].EntryCount, (int?)3,
        "CMP BX,3 plus JB excludes the immediate");

    var jbeNearTaken = AnalyzeBridgedTable(new byte[] {
        0x83, 0xFB, 0x02, 0x0F, 0x86, 0x04, 0x00,
        0xC3, 0x90, 0x90, 0x90,   // near JBE rel16 +4 to offset 11
        0xD1, 0xE3,
        0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });
    Equal(jbeNearTaken.Report.GuardedTableCandidates, 1,
        "near16 JBE taken edge can select a bounded table block");
    var jbNearTaken = AnalyzeBridgedTable(new byte[] {
        0x83, 0xFB, 0x03, 0x0F, 0x82, 0x04, 0x00,
        0xC3, 0x90, 0x90, 0x90,
        0xD1, 0xE3,
        0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });
    Equal(jbNearTaken.Report.GuardedTableCandidates, 1,
        "near16 JB taken edge enforces strict less-than bound");

    var signedTaken = AnalyzeBridgedTable(new byte[] {
        0x83, 0xFB, 0x02, 0x7E, 0x03, // JLE is signed; cannot bound BX unsigned
        0xC3, 0x90, 0x90, 0xD1, 0xE3,
        0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });
    Equal(signedTaken.Report.GuardedTableCandidates, 0,
        "signed JLE taken edge cannot establish unsigned table index bound");

    var invertedOpcode = AnalyzeBridgedTable(new byte[] {
        0x83, 0xFB, 0x02, 0x73, 0x03, // JAE taken => BX>=2, not in range
        0xC3, 0x90, 0x90, 0xD1, 0xE3,
        0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });
    Equal(invertedOpcode.Report.GuardedTableCandidates, 0,
        "wrong unsigned polarity cannot justify taking the table edge");

    var extraBranchEdge = jbeTaken.Graph with {
        Edges = jbeTaken.Graph.Edges.Append(new Dos16FlowEdge(
            new Dos16Address(0, 0x103), new Dos16Address(0, 0x10A),
            "branch-taken", "synthetic-extra-edge")).ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(
        jbeTaken.Region, extraBranchEdge).GuardedTableCandidates, 0,
        "spurious additional taken edge must invalidate guard evidence");
    var spoofedTakenEdge = jbeTaken.Graph with {
        Edges = jbeTaken.Graph.Edges.Select(e =>
            e.Source.Offset == 0x103 && e.Kind == "branch-taken"
                ? e with { Destination = new Dos16Address(0, 0x109) } : e)
                .ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(
        jbeTaken.Region, spoofedTakenEdge).GuardedTableCandidates, 0,
        "a modified branch target must disagree with the real displacement");

    var skippedSourceBytes = nopBeforeTable.Graph with {
        Edges = nopBeforeTable.Graph.Edges
            .Where(e => !(e.Source.Offset == 0x107 &&
                e.Kind == "fallthrough"))
            .Append(new Dos16FlowEdge(
                new Dos16Address(0, 0x105), new Dos16Address(0, 0x108),
                "fallthrough", "synthetic-nonadjacent-fallthrough"))
            .ToArray()
    };
    Equal(Dos16UniqueTransparentPath.Connects(nopBeforeTable.Region,
        skippedSourceBytes, new Dos16Address(0, 0x105),
        new Dos16Address(0, 0x108), "fallthrough"), false,
        "claimed direct fallthrough must match the actual next IP");
    Equal(Dos16AutomaticDispatchRecovery.Inspect(
        nopBeforeTable.Region, skippedSourceBytes).GuardedTableCandidates, 0,
        "graph edges skipping an unmodeled byte may not forge a bounded table");

    var syntheticEntryBackEdge = automaticGraph with {
        Edges = automaticGraph.Edges.Append(new Dos16FlowEdge(
            new Dos16Address(0, 0x105), automaticGraph.Entry,
            "branch", "synthetic-entry-backedge")).ToArray()
    };
    Equal(Dos16UniqueTransparentPath.Connects(automaticRegion,
        syntheticEntryBackEdge, new Dos16Address(0, 0x105),
        automaticGraph.Entry, "branch"), false,
        "the independently seeded entry is not a uniquely reached successor");
    var exitIntoTable = (byte[])new byte[] {
        0x83, 0xFB, 0x02, 0x76, 0x03,
        0xEB, 0x03, 0x90, 0xD1, 0xE3,
        0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    };
    var mergedFailure = AnalyzeBridgedTable(exitIntoTable);
    Equal(mergedFailure.Report.GuardedTableCandidates, 0,
        "out-of-range branch rejoining the table invalidates unique predecessor proof");
    Console.WriteLine("PASS: exact JA/JAE reject-outside and JB/JBE select-inside bounds");

    // CMP; JA default; NOP; SHL BX,1; CS-indexed JMP.
    var nopBeforeShift = AnalyzeBridgedTable(new byte[] {
        0x83, 0xFB, 0x02, 0x77, 0x08,
        0x90, 0xD1, 0xE3, 0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });
    Equal(nopBeforeShift.Report.GuardedTableCandidates, 1,
        "NOP between unsigned guard and stride does not invalidate flags or BX");
    Equal(Dos16UniqueTransparentPath.Connects(nopBeforeShift.Region,
        nopBeforeShift.Graph, new Dos16Address(0, 0x103),
        new Dos16Address(0, 0x106), "branch-not-taken"), true,
        "one unique fallthrough plus a source-backed NOP proves the guard path");

    Equal(nopBeforeTable.Report.GuardedTableCandidates, 1,
        "NOP between index scaling and table still yields original-backed candidates");
    Equal(Dos16UniqueTransparentPath.Connects(nopBeforeTable.Region,
        nopBeforeTable.Graph, new Dos16Address(0, 0x105),
        new Dos16Address(0, 0x108), "fallthrough"), true,
        "table predecessor can be bridged by a checked NOP");

    // CMP; JA default; JMP short stride; unreachable padding; SHL BX,1;
    // CS-indexed JMP. Unique unconditional branch traverses basic blocks.
    var jumpBeforeShift = AnalyzeBridgedTable(new byte[] {
        0x83, 0xFB, 0x02, 0x77, 0x0C,
        0xEB, 0x03, 0x90, 0x90, 0x90,
        0xD1, 0xE3, 0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });
    Equal(jumpBeforeShift.Report.GuardedTableCandidates, 1,
        "short unconditional jump preserves bound when it uniquely leads to stride");
    Equal(Dos16UniqueTransparentPath.Connects(jumpBeforeShift.Region,
        jumpBeforeShift.Graph, new Dos16Address(0, 0x103),
        new Dos16Address(0, 0x10A), "branch-not-taken"), true,
        "guard path crosses a validated short JMP to another basic block");

    // CMP; JA default; SHL BX,1; JMP short dispatch; padding;
    // CS-indexed JMP. JMP rel16 uses the same bounded transparency proof.
    var jumpBeforeTable = AnalyzeBridgedTable(new byte[] {
        0x83, 0xFB, 0x02, 0x77, 0x0D,
        0xD1, 0xE3, 0xEB, 0x04, 0x90, 0x90, 0x90, 0x90,
        0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });
    Equal(jumpBeforeTable.Report.GuardedTableCandidates, 1,
        "short unconditional jump after stride can reach distinct indirect branch block");
    var nearJumpBeforeTable = AnalyzeBridgedTable(new byte[] {
        0x83, 0xFB, 0x02, 0x77, 0x0D,
        0xD1, 0xE3, 0xE9, 0x03, 0x00, 0x90, 0x90, 0x90,
        0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });
    Equal(nearJumpBeforeTable.Report.GuardedTableCandidates, 1,
        "unprefixed near16 unconditional jump preserves exact source bounds");

    // The same bounded proof must work around a copied-register guard:
    // CMP AX; NOP; JA default; MOV BX,AX; NOP; SHL; NOP; indexed JMP.
    var copiedAcrossNops = AnalyzeBridgedTable(new byte[] {
        0x83, 0xF8, 0x02, 0x90, 0x77, 0x0B,
        0x89, 0xC3, 0x90, 0xD1, 0xE3, 0x90,
        0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });
    Equal(copiedAcrossNops.Report.GuardedTableCandidates, 1,
        "source-backed NOPs preserve flags and exact AX to BX register copy");
    Equal(copiedAcrossNops.Report.Sites[0].EntryCount, (int?)3,
        "copied register still carries the unsigned three-entry bound");

    var competingPath = nopBeforeTable.Graph with {
        Edges = nopBeforeTable.Graph.Edges.Append(new Dos16FlowEdge(
            new Dos16Address(0, 0x100), new Dos16Address(0, 0x108),
            "branch", "synthetic-unverified-extra-incoming")).ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(nopBeforeTable.Region,
        competingPath).GuardedTableCandidates, 0,
        "competing predecessor into bridged table blocks automatic index proof");

    var bxMutation = AnalyzeBridgedTable(new byte[] {
        0x83, 0xFB, 0x02, 0x77, 0x08,
        0x43, 0xD1, 0xE3, 0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });
    Equal(bxMutation.Report.GuardedTableCandidates, 0,
        "INC BX between guard and stride invalidates the bounded index");

    var manyNops = AnalyzeBridgedTable(new byte[] {
        0x83, 0xFB, 0x02, 0x77, 0x0D, 0xD1, 0xE3,
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
        0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3
    });
    Equal(manyNops.Report.GuardedTableCandidates, 0,
        "candidate path exceeding the transparent hop budget stays unresolved");

    var staleBridgedGraph = jumpBeforeTable.Graph with {
        Instructions = jumpBeforeTable.Graph.Instructions.Select(x =>
            x.Address.Offset == 0x107
                ? x with { SourceFileOffset = long.MaxValue } : x).ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(jumpBeforeTable.Region,
        staleBridgedGraph).GuardedTableCandidates, 0,
        "stale bridge opcode provenance cannot license indirect table recovery");
    Console.WriteLine("PASS: bounded unique NOP/JMP paths across basic blocks");

    var jaeGuard = (byte[])automaticBytes.Clone();
    jaeGuard[2] = 3; jaeGuard[3] = 0x73; // JAE after CMP BX,3 => BX in [0,2]
    var jaeRegion = DosComImage.LoadExplicit(BinaryArtifact.FromBytes(jaeGuard)).CreateCodeRegion();
    var jaeTable = Dos16AutomaticDispatchRecovery.Inspect(jaeRegion,
        Dos16ControlFlowAnalysis.Analyze(jaeRegion, 0x100));
    Equal(jaeTable.GuardedTableCandidates, 1,
        "unsigned JAE comparator can establish a strict upper bound");
    Equal(jaeTable.Sites[0].EntryCount, (int?)3,
        "JAE excludes the compared value from possible table indices");
    var transferredBound = new byte[0x40];
    new byte[] { 0x83, 0xF8, 0x02, 0x77, 0x09, 0x89, 0xC3,
        0xD1, 0xE3, 0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3 }
        .CopyTo(transferredBound, 0); // CMP AX,2; JA; MOV BX,AX; SHL BX,1; CS:JMP [BX+120]
    MzWord(transferredBound, 0x20, 0x130);
    MzWord(transferredBound, 0x22, 0x131);
    MzWord(transferredBound, 0x24, 0x132);
    transferredBound[0x30] = transferredBound[0x31] =
        transferredBound[0x32] = 0xC3;
    var copiedBoundRegion = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(transferredBound)).CreateCodeRegion();
    Equal(Dos16AutomaticDispatchRecovery.Inspect(copiedBoundRegion,
        Dos16ControlFlowAnalysis.Analyze(copiedBoundRegion, 0x100))
        .GuardedTableCandidates, 1,
        "guarded AX range propagates through one exact MOV BX,AX");

    // 3D iw is CMP AX,imm16, a different exact instruction encoding
    // from 81 F8 iw and 83 F8 ib. The AX -> BX copy must still be
    // present on the one uniquely bounded path.
    var accumulatorCompare = (byte[])transferredBound.Clone();
    accumulatorCompare[0] = 0x3D; // CMP AX,0002h
    accumulatorCompare[1] = 0x02;
    accumulatorCompare[2] = 0x00;
    var accumulatorRegion = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(accumulatorCompare)).CreateCodeRegion();
    var accumulatorGraph = Dos16ControlFlowAnalysis.Analyze(accumulatorRegion, 0x100);
    var accumulatorReport = Dos16AutomaticDispatchRecovery.Inspect(
        accumulatorRegion, accumulatorGraph);
    Equal(accumulatorReport.GuardedTableCandidates, 1,
        "accumulator-specific CMP AX,imm16 proves AX bound before MOV BX,AX");
    Equal(accumulatorReport.Sites[0].EntryCount, (int?)3,
        "CMP AX,2; JA default admits three candidate pointer entries");

    var nonBoundedAccumulator = (byte[])accumulatorCompare.Clone();
    nonBoundedAccumulator[2] = 0x01; // CMP AX,0102h > supported index bound
    var largeAccumulatorRegion = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(nonBoundedAccumulator)).CreateCodeRegion();
    Equal(Dos16AutomaticDispatchRecovery.Inspect(largeAccumulatorRegion,
        Dos16ControlFlowAnalysis.Analyze(largeAccumulatorRegion, 0x100))
        .GuardedTableCandidates, 0,
        "large CMP AX,imm16 cannot become a fabricated small dispatch count");

    var wrongAccumulatorCopy = (byte[])accumulatorCompare.Clone();
    wrongAccumulatorCopy[6] = 0xCB; // MOV BX,CX instead of MOV BX,AX
    var mismatchAccumulatorRegion = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(wrongAccumulatorCopy)).CreateCodeRegion();
    Equal(Dos16AutomaticDispatchRecovery.Inspect(mismatchAccumulatorRegion,
        Dos16ControlFlowAnalysis.Analyze(mismatchAccumulatorRegion, 0x100))
        .GuardedTableCandidates, 0,
        "accumulator CMP bound cannot flow through an unrelated register copy");

    var copiedFromWrongRegister = (byte[])transferredBound.Clone();
    copiedFromWrongRegister[6] = 0xCB; // MOV BX,CX, but CMP still bounds AX
    var wrongCopyRegion = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(copiedFromWrongRegister)).CreateCodeRegion();
    Equal(Dos16AutomaticDispatchRecovery.Inspect(wrongCopyRegion,
        Dos16ControlFlowAnalysis.Analyze(wrongCopyRegion, 0x100))
        .GuardedTableCandidates, 0,
        "register copy must match the exact register bounded by unsigned CMP");
    var guardedCx = (byte[])copiedFromWrongRegister.Clone();
    guardedCx[1] = 0xF9; // CMP CX,2; MOV BX,CX
    var guardedCxRegion = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(guardedCx)).CreateCodeRegion();
    Equal(Dos16AutomaticDispatchRecovery.Inspect(guardedCxRegion,
        Dos16ControlFlowAnalysis.Analyze(guardedCxRegion, 0x100))
        .GuardedTableCandidates, 1,
        "bound on CX is propagated through MOV BX,CX");
    var reversedMov = (byte[])transferredBound.Clone();
    reversedMov[5] = 0x8B; reversedMov[6] = 0xD8; // MOV BX,AX alternate encoding
    var alternateMoveRegion = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(reversedMov)).CreateCodeRegion();
    Equal(Dos16AutomaticDispatchRecovery.Inspect(alternateMoveRegion,
        Dos16ControlFlowAnalysis.Analyze(alternateMoveRegion, 0x100))
        .GuardedTableCandidates, 1,
        "alternate exact MOV BX,AX encoding preserves unsigned bound");

    var imm16Guard = new byte[0x40];
    new byte[] { 0x81, 0xFB, 0x02, 0x00, 0x77, 0x07, 0xD1, 0xE3,
        0x2E, 0xFF, 0xA7, 0x20, 0x01, 0xC3 }.CopyTo(imm16Guard, 0);
    MzWord(imm16Guard, 0x20, 0x130);
    MzWord(imm16Guard, 0x22, 0x131);
    MzWord(imm16Guard, 0x24, 0x132);
    imm16Guard[0x30] = imm16Guard[0x31] = imm16Guard[0x32] = 0xC3;
    var imm16Region = DosComImage.LoadExplicit(BinaryArtifact.FromBytes(imm16Guard)).CreateCodeRegion();
    Equal(Dos16AutomaticDispatchRecovery.Inspect(imm16Region,
        Dos16ControlFlowAnalysis.Analyze(imm16Region, 0x100)).GuardedTableCandidates,
        1, "16-bit CMP BX,imm16 plus unsigned guard is supported");
    var signExtended = (byte[])automaticBytes.Clone();
    signExtended[2] = 0x80; // CMP BX,imm8 sign-extends negative immediate
    var signExtendedRegion = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(signExtended)).CreateCodeRegion();
    Equal(Dos16AutomaticDispatchRecovery.Inspect(signExtendedRegion,
        Dos16ControlFlowAnalysis.Analyze(signExtendedRegion, 0x100)).GuardedTableCandidates,
        0, "negative sign-extended immediate cannot prove a small unsigned BX bound");

    Equal(automaticGraph.Instructions.Any(x => x.Address.Offset == 0x130), false,
        "automatic tables do not enqueue inferred destinations in the CFG");
    var signedGuard = (byte[])automaticBytes.Clone();
    signedGuard[3] = 0x7F; // JG is signed, not the unsigned JA condition
    var signedRegion = DosComImage.LoadExplicit(BinaryArtifact.FromBytes(signedGuard)).CreateCodeRegion();
    Equal(Dos16AutomaticDispatchRecovery.Inspect(signedRegion,
        Dos16ControlFlowAnalysis.Analyze(signedRegion, 0x100)).GuardedTableCandidates,
        0, "signed comparison cannot validate unsigned BX table bounds");
    var unscaled = (byte[])automaticBytes.Clone();
    unscaled[5] = 0x90; unscaled[6] = 0x90;
    var unscaledRegion = DosComImage.LoadExplicit(BinaryArtifact.FromBytes(unscaled)).CreateCodeRegion();
    Equal(Dos16AutomaticDispatchRecovery.Inspect(unscaledRegion,
        Dos16ControlFlowAnalysis.Analyze(unscaledRegion, 0x100)).GuardedTableCandidates,
        0, "16-bit BX pointer table requires a proven stride transformation");
    var changedRegister = (byte[])automaticBytes.Clone();
    changedRegister[1] = 0xF8; // CMP AX,2 cannot bound BX
    var changedRegisterRegion = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(changedRegister)).CreateCodeRegion();
    Equal(Dos16AutomaticDispatchRecovery.Inspect(changedRegisterRegion,
        Dos16ControlFlowAnalysis.Analyze(changedRegisterRegion, 0x100)).GuardedTableCandidates,
        0, "bounds from a different register cannot be assigned to BX");
    var alternateModrm = (byte[])automaticBytes.Clone();
    alternateModrm[9] = 0xA6; // JMP word ptr CS:[BP+disp16], not [BX+disp16]
    var alternateModrmRegion = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(alternateModrm)).CreateCodeRegion();
    var alternateModrmGraph = Dos16ControlFlowAnalysis.Analyze(alternateModrmRegion, 0x100);
    Equal(Dos16AutomaticDispatchRecovery.Inspect(alternateModrmRegion,
        alternateModrmGraph).GuardedTableCandidates, 0,
        "different 16-bit ModRM base register cannot be interpreted as BX");
    var alternateManifest = new Dos16TableManifest(1, alternateModrmRegion.SourceSha256,
        new[] { new Dos16TableRequest(0x107, 0x120, 3, "near16", "synthetic") });
    Equal(Dos16DispatchTableRecovery.Inspect(alternateModrmRegion,
        alternateModrmGraph, alternateManifest).Candidates, 0,
        "manual manifest cannot override a non-BX ModRM encoding");
    var ambiguousGuard = (byte[])automaticBytes.Clone();
    ambiguousGuard[4] = 0x00; // JA taken and not taken both reach SHL
    var ambiguousRegion = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(ambiguousGuard)).CreateCodeRegion();
    Equal(Dos16AutomaticDispatchRecovery.Inspect(ambiguousRegion,
        Dos16ControlFlowAnalysis.Analyze(ambiguousRegion, 0x100)).GuardedTableCandidates,
        0, "multiple incoming flow edges prevent falsely claiming a unique guard");
    var failedFourthEntry = (byte[])automaticBytes.Clone();
    failedFourthEntry[2] = 0x03; // CMP BX,3 permits a fourth, invalid entry
    var failedFourthRegion = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(failedFourthEntry)).CreateCodeRegion();
    var failedFourthReport = Dos16AutomaticDispatchRecovery.Inspect(failedFourthRegion,
        Dos16ControlFlowAnalysis.Analyze(failedFourthRegion, 0x100));
    Equal(failedFourthReport.GuardedTableCandidates, 0,
        "one unverified table entry rejects the entire automatically bounded table");
    Equal(failedFourthReport.Sites[0].StoredTable, null,
        "partially checked tables must not expose an unsafe target list");
    Equal(failedFourthReport.Sites[0].EntryCount, (int?)null,
        "rejected table must not expose an unvalidated proposed entry count");
    Equal(failedFourthReport.Sites[0].Status,
        "rejected-guarded-table:rejected-unverified-entry",
        "site rejection must preserve the exact pointer validation failure");

    var overlapAuto = (byte[])automaticBytes.Clone();
    overlapAuto[10] = 0x07; overlapAuto[11] = 0x01;
    var overlapRegion = DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(overlapAuto)).CreateCodeRegion();
    var overlapGraph = Dos16ControlFlowAnalysis.Analyze(overlapRegion, 0x100);
    var overlapTable = new Dos16TableManifest(1, overlapRegion.SourceSha256,
        new[] { new Dos16TableRequest(0x107, 0x107, 3, "near16", "synthetic-overlap") });
    Equal(Dos16DispatchTableRecovery.Inspect(overlapRegion,
        overlapGraph, overlapTable).Findings[0].Status,
        "rejected-code-data-overlap",
        "decoded instructions cannot be silently reclassified as immutable pointer table storage");
    Equal(Dos16AutomaticDispatchRecovery.Inspect(overlapRegion,
        overlapGraph).GuardedTableCandidates, 0,
        "automatic guard cannot override an existing source-code/data overlap");
    var staleGuard = automaticGraph with {
        Instructions = automaticGraph.Instructions.Select(x =>
            x.Address.Offset == 0x103 ? x with { SourceFileOffset = long.MaxValue } : x).ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(automaticRegion,
        staleGuard).GuardedTableCandidates, 0,
        "stale branch physical locations cannot authorize automatic tables");
    var staleCompare = automaticGraph with {
        Instructions = automaticGraph.Instructions.Select(x =>
            x.Address.Offset == 0x100 ? x with { SourceFileOffset = long.MaxValue } : x).ToArray()
    };
    Equal(Dos16AutomaticDispatchRecovery.Inspect(automaticRegion,
        staleCompare).GuardedTableCandidates, 0,
        "stale CMP physical locations are rejected without unsafe byte reads");
    Equal(Dos16AutomaticDispatchRecovery.Inspect(automaticRegion,
        automaticGraph with { Truncated = true }).GuardedTableCandidates, 0,
        "truncated CFG cannot claim complete predecessor and guard evidence");
    Equal(Dos16AutomaticDispatchRecovery.Inspect(automaticRegion,
        automaticGraph with { Truncated = true }).AnalysisStatus.Contains(
            "incomplete-input-graph", StringComparison.Ordinal), true,
        "report status must explicitly identify an incomplete input graph");

    var manyIndirectSites = automaticGraph with {
        Instructions = automaticGraph.Instructions.Concat(
            Enumerable.Range(0, 129).Select(i =>
                automaticGraph.Instructions.Single(x => x.Address.Offset == 0x107)
                    with { Address = new Dos16Address(0, (ushort)(0x900 + i)) }))
            .ToArray()
    };
    var cappedSiteReport = Dos16AutomaticDispatchRecovery.Inspect(
        automaticRegion, manyIndirectSites);
    Equal(cappedSiteReport.IndirectSites, 130,
        "site-report budget retains total original discovered indirect-site count");
    Equal(cappedSiteReport.Sites.Count, 128,
        "site-report budget caps emitted per-site findings");
    Equal(cappedSiteReport.Truncated, true,
        "site-report cap is explicit rather than silent");
    Equal(cappedSiteReport.AnalysisStatus.Contains(
        "site-report-truncated", StringComparison.Ordinal), true,
        "report status exposes the reason sites were truncated");



    // Four-byte FAR entries require a SHL BX,2 and distinct MZ selector
    // fixups; a load segment is an explicit simulation assumption.
    var automaticFarBytes = (byte[])mzBytes.Clone();
    MzWord(automaticFarBytes, 8, 3); MzWord(automaticFarBytes, 6, 2);
    MzWord(automaticFarBytes, 0x1C, 0x32); MzWord(automaticFarBytes, 0x1E, 0);
    MzWord(automaticFarBytes, 0x20, 0x36); MzWord(automaticFarBytes, 0x22, 0);
    new byte[] { 0x83, 0xFB, 0x01, 0x77, 0x08, 0xC1, 0xE3, 0x02,
        0x2E, 0xFF, 0xAF, 0x30, 0x00, 0xC3 }.CopyTo(automaticFarBytes, 0x30);
    MzWord(automaticFarBytes, 0x60, 0x40); MzWord(automaticFarBytes, 0x62, 0);
    MzWord(automaticFarBytes, 0x64, 0x42); MzWord(automaticFarBytes, 0x66, 0);
    automaticFarBytes[0x70] = automaticFarBytes[0x72] = 0xC3;
    var automaticFarMz = DosMzImage.Load(BinaryArtifact.FromBytes(automaticFarBytes));
    var automaticFarRegion = automaticFarMz.CreateEntryCodeRegion();
    var automaticFarGraph = Dos16ControlFlowAnalysis.Analyze(automaticFarRegion, 0);
    var automaticFarResult = Dos16AutomaticDispatchRecovery.Inspect(automaticFarRegion,
        automaticFarGraph, automaticFarMz, automaticFarMz.SimulateRelocations(0x2000));
    Equal(automaticFarResult.GuardedTableCandidates, 1,
        "MZ FAR indexed table requires matching unsigned guard, BX stride and selectors");
    Equal(automaticFarResult.Sites[0].EntryCount, (int?)2,
        "unsigned CMP BX,1 bounds two FAR table entries");
    Equal(automaticFarResult.Sites[0].StoredTable?.Entries[1].TargetFileOffset, 0x72L,
        "MZ FAR table target preserves independently backed original file position");

    // Four-byte FAR pointer addressing also permits two independently
    // verified BX doublings. The far selector relocation requirements
    // are unchanged; only the explicitly guarded stride is generalized.
    byte[] MakeDoubleScaledFarBytes(byte first0, byte first1,
        byte second0, byte second1)
    {
        var source = (byte[])automaticFarBytes.Clone();
        new byte[] {
            0x83, 0xFB, 0x01, 0x77, 0x09,
            first0, first1, second0, second1,
            0x2E, 0xFF, 0xAF, 0x30, 0x00, 0xC3
        }.CopyTo(source, 0x30);
        return source;
    }

    Dos16GuardedDispatchReport AnalyzeDoubleScaledFar(byte[] bytes,
        bool useRelocations = true)
    {
        var image = DosMzImage.Load(BinaryArtifact.FromBytes(bytes));
        var region = image.CreateEntryCodeRegion();
        var graph = Dos16ControlFlowAnalysis.Analyze(region, image.InitialIp);
        return Dos16AutomaticDispatchRecovery.Inspect(region, graph,
            image, useRelocations ? image.SimulateRelocations(0x2000) : null);
    }

    var farTwoShifts = AnalyzeDoubleScaledFar(
        MakeDoubleScaledFarBytes(0xD1, 0xE3, 0xD1, 0xE3));
    Equal(farTwoShifts.GuardedTableCandidates, 1,
        "two uniquely connected SHL BX,1 instructions establish far16 stride four");
    Equal(farTwoShifts.Sites[0].EntryCount, (int?)2,
        "two doubled steps retain the unsigned CMP BX,1 table bound");
    Equal(farTwoShifts.Sites[0].StoredTable?.Entries.Count, 2,
        "four-byte FAR pointers still require each selector relocation");

    var farMixedDoubling = AnalyzeDoubleScaledFar(
        MakeDoubleScaledFarBytes(0xD1, 0xE3, 0x01, 0xDB));
    Equal(farMixedDoubling.GuardedTableCandidates, 1,
        "SHL BX,1 then ADD BX,BX is an exact four-byte FAR index stride");
    var farMixedAlternate = AnalyzeDoubleScaledFar(
        MakeDoubleScaledFarBytes(0x03, 0xDB, 0xD1, 0xE3));
    Equal(farMixedAlternate.GuardedTableCandidates, 1,
        "alternate ADD BX,BX followed by SHL BX,1 also proves far index scale");

    var farUnrelatedAdder = AnalyzeDoubleScaledFar(
        MakeDoubleScaledFarBytes(0xD1, 0xE3, 0x03, 0xCB));
    Equal(farUnrelatedAdder.GuardedTableCandidates, 0,
        "ADD BX,CX cannot serve as a second proven FAR-table scaling step");
    var farOnlyOneShift = AnalyzeDoubleScaledFar(
        MakeDoubleScaledFarBytes(0x90, 0x90, 0xD1, 0xE3));
    Equal(farOnlyOneShift.GuardedTableCandidates, 0,
        "single BX doubling cannot index a four-byte FAR pointer array");

    var farMissingDoubleFixups = AnalyzeDoubleScaledFar(
        MakeDoubleScaledFarBytes(0xD1, 0xE3, 0xD1, 0xE3),
        useRelocations: false);
    Equal(farMissingDoubleFixups.GuardedTableCandidates, 0,
        "proven FAR stride cannot override missing selector relocation proof");
    Console.WriteLine("PASS: exact one/two-step x86-16 pointer table scaling");

    Equal(Dos16AutomaticDispatchRecovery.Inspect(automaticFarRegion,
        automaticFarGraph, automaticFarMz).GuardedTableCandidates, 0,
        "no runtime load base means no FAR selector target recovery");
    Console.WriteLine("PASS: automatic BX unsigned-guard and proven near/FAR table stride");



    var crossReview = DosMzCrossSegmentAnalysis.Analyze(farImage,
        assumedLoadSegment: 0x2000, maxEntryGraphs: 4, instructionsPerEntry: 16);
    Equal(crossReview.SourceSha256, farImage.SourceSha256,
        "cross-segment review retains the actual original MZ hash");
    Equal(crossReview.IndependentEntryGraphs.Count, 2,
        "relocation-backed FAR CALL may seed one independent target graph");
    Equal(crossReview.CrossSegmentCandidates.Count, 1,
        "one uniquely relocated far transfer seeds candidate reachability");
    Equal(crossReview.CrossSegmentCandidates[0].Destination,
        new Dos16Address(0, 0x10), "cross-segment target retains relative CS:IP");
    Equal(crossReview.UniqueDecodedBytes, 7,
        "cross-segment ledger deduplicates decoded original physical bytes");
    Equal(crossReview.PhysicalConflicts.Count, 0,
        "separate nonoverlapping far target has no physical interpretation conflict");
    Equal(crossReview.Truncated, false,
        "bounded far CALL plus return target closes without reaching graph budget");
    var cappedCrossReview = DosMzCrossSegmentAnalysis.Analyze(farImage,
        0x2000, maxEntryGraphs: 1, instructionsPerEntry: 16);
    Equal(cappedCrossReview.Truncated, true,
        "unprocessed FAR target is reported when cross-segment budget is exhausted");
    Equal(cappedCrossReview.IndependentEntryGraphs.Count, 1,
        "cross-segment graph limit prevents unrestricted source traversal");
    var conflictingFarBytes = (byte[])farBytes.Clone();
    conflictingFarBytes[0x21] = 1; // FAR call now targets interior of itself
    var conflictImage = DosMzImage.Load(BinaryArtifact.FromBytes(conflictingFarBytes));
    var overlappingReview = DosMzCrossSegmentAnalysis.Analyze(conflictImage,
        0x2000, maxEntryGraphs: 2, instructionsPerEntry: 5);
    Equal(overlappingReview.PhysicalConflicts.Count > 0, true,
        "distinct segment/IP interpretations of the same source bytes are recorded");
    Equal(overlappingReview.PhysicalConflicts[0].Status.Contains(
        "neither-verified", StringComparison.Ordinal), true,
        "conflicting x86-16 decode hypotheses must never be silently promoted");
    var farJumpBytes = (byte[])farBytes.Clone();
    farJumpBytes[0x20] = 0xEA;
    var farJumpImage = DosMzImage.Load(BinaryArtifact.FromBytes(farJumpBytes));
    var farJumpGraph = Dos16ControlFlowAnalysis.Analyze(
        farJumpImage.CreateEntryCodeRegion(), farJumpImage.InitialIp);
    Equal(DosMzFarTargetRecovery.Inspect(farJumpImage, farJumpGraph,
        farJumpImage.SimulateRelocations(0x2000))[0].TransferKind, "far-immediate-jump",
        "unprefixed FAR JMP is classified separately from FAR CALL");
    var absentSelectorFixup = (byte[])farBytes.Clone();
    MzWord(absentSelectorFixup, 0x1C, 0x10);
    var noSelectorImage = DosMzImage.Load(BinaryArtifact.FromBytes(absentSelectorFixup));
    Equal(DosMzFarTargetRecovery.Inspect(noSelectorImage,
        Dos16ControlFlowAnalysis.Analyze(noSelectorImage.CreateEntryCodeRegion(), 0),
        noSelectorImage.SimulateRelocations(0x2000)).Count, 0,
        "far pointer with unrelated fixup cannot be asserted as known target");
    Fails(() => DosMzFarTargetRecovery.Inspect(mzImage, farGraph, farView),
        "foreign MZ binary cannot inherit another source's far-branch annotations");
    Console.WriteLine("PASS: non-mutating ordered MZ relocation and evidence-bound far-call/jump hypotheses");
    var invalidMz = (byte[])mzBytes.Clone();
    MzWord(invalidMz, 6, 2); // table would exceed 32-byte header
    Fails(() => DosMzImage.Load(BinaryArtifact.FromBytes(invalidMz)),
        "truncated/oversized MZ relocation table rejected");
    var lxBytes = (byte[])mzBytes.Clone();
    Dword(lxBytes, 0x3C, 0x80);
    lxBytes[0x80] = (byte)'L'; lxBytes[0x81] = (byte)'X';
    var lxArtifact = BinaryArtifact.FromBytes(lxBytes, "A32ADLIB.DLL");
    Equal(BinaryFormatDetector.Inspect(lxArtifact).FormatId, "lx",
        "LX audio driver is detected without PE interpretation");
    Fails(() => DosMzImage.Load(lxArtifact),
        "MZ parser must not decode an identified LX wrapper as flat DOS real-mode code");
    Fails(() => DosComImage.LoadExplicit(lxArtifact),
        "COM manual mode refuses an already identified LX executable");
    Fails(() => DosComImage.LoadExplicit(
        BinaryArtifact.FromBytes(System.Text.Encoding.ASCII.GetBytes("FBOV"))),
        "COM manual mode refuses an overlay container signature");
    Console.WriteLine("PASS: signature-only overlay/resource/LX detection and bounded 16-bit MZ metadata/decoding");

    // FBOV/TPOV containers are never recursively decoded. Only a
    // full-hash-bound explicit region can authorize requested linear decode.
    byte[] syntheticFbovBytes = new byte[24];
    System.Text.Encoding.ASCII.GetBytes("FBOV").CopyTo(syntheticFbovBytes, 0);
    Dword(syntheticFbovBytes, 4, 16); // payload excludes the eight-byte envelope
    new byte[] { 0xB8, 0x34, 0x12, 0xC3 }.CopyTo(syntheticFbovBytes, 8);
    var syntheticFbov = BinaryArtifact.FromBytes(syntheticFbovBytes, "GAME.OVR");
    var fbov = DosOverlayImage.LoadEnvelope(syntheticFbov);
    Equal(fbov.Envelope.Format, "fbov", "FBOV envelope validated from magic");
    Equal(fbov.Envelope.HeaderBytes, 8, "eight-byte FBOV header checked");
    Equal(fbov.Envelope.PayloadBytes, 16, "FBOV header excludes payload length");
    var mappedRegion = new DosOverlayRegion("entry", 8, 4, 0x200,
        0x100, "code-candidate", "synthetic-analyst-map");
    var regionManifest = new DosOverlayRegionManifest(1, syntheticFbov.Sha256,
        syntheticFbov.Length, "fbov", new[] { mappedRegion });
    Equal(fbov.ValidateRegions(regionManifest).Count, 1,
        "source-bound FBOV candidate region accepted without decoding the header");
    var mappedInstructions = fbov.DecodeExplicit(regionManifest, "entry", 0, 2);
    Equal(mappedInstructions.Count, 2, "only explicit region instructions decoded");
    Equal(mappedInstructions[0].OriginalBytes, "B83412", "overlay original 16-bit bytes");
    Equal(mappedInstructions[1].SourceFileOffset, 11L,
        "overlay code retains its original physical file offset");
    Equal(mappedInstructions[0].Segment, (ushort)0x200,
        "assumed overlay segment kept separate from file position");
    Fails(() => fbov.ValidateRegions(regionManifest with {
        SourceSha256 = new string('f', 64) }),
        "another game's overlay mapping cannot be applied by matching filename");
    Fails(() => fbov.ValidateRegions(regionManifest with {
        Regions = new[] { mappedRegion with { FileOffset = 4 } } }),
        "an overlay region must not decode envelope bytes");
    Fails(() => fbov.ValidateRegions(regionManifest with {
        Regions = new[] { mappedRegion,
            mappedRegion with { Id = "overlap", FileOffset = 10 } } }),
        "two overlapping candidate code maps cannot assign the same physical byte");
    Fails(() => fbov.ValidateRegions(regionManifest with {
        Regions = new[] { mappedRegion with { AssumedOffset = 0xFFFE } } }),
        "overlay region must fit the declared 16-bit address space");
    Fails(() => fbov.DecodeExplicit(regionManifest with {
        Regions = new[] { mappedRegion with { Role = "data-candidate" } } },
        "entry", 0, 2),
        "explicitly marked data must not be decoded as candidate code");

    byte[] malformedFbov = (byte[])syntheticFbovBytes.Clone();
    Dword(malformedFbov, 4, 15);
    Fails(() => DosOverlayImage.LoadEnvelope(BinaryArtifact.FromBytes(malformedFbov)),
        "incorrect FBOV payload size cannot be silently guessed");
    byte[] expandedFbov = new byte[24];
    System.Text.Encoding.ASCII.GetBytes("FBOV").CopyTo(expandedFbov, 0);
    Dword(expandedFbov, 4, 8);
    var extendedEnvelope = DosOverlayImage.LoadEnvelope(BinaryArtifact.FromBytes(expandedFbov));
    Equal(extendedEnvelope.Envelope.Variant, "fbov-sixteen-byte-envelope",
        "extended FBOV variant remains distinct from eight-byte envelope");

    var tpovArtifact = BinaryArtifact.FromBytes(
        System.Text.Encoding.ASCII.GetBytes("TPOV").Concat(new byte[] { 0x90, 0xC3 }).ToArray(),
        "GAME.OVR");
    var tpov = DosOverlayImage.LoadEnvelope(tpovArtifact);
    Equal(tpov.Envelope.Format, "tpov", "TPOV signature detected");
    Equal(tpov.Envelope.HeaderBytes, 4, "TPOV minimum signature envelope");
    Equal(tpov.Envelope.Confidence.Contains("unverified", StringComparison.Ordinal),
        true, "TPOV signature never promises known internal segmentation");
    Fails(() => tpov.ValidateRegions(regionManifest),
        "TPOV and FBOV sources cannot reuse each other's region maps");
    var tpovRegion = new DosOverlayRegionManifest(1, tpovArtifact.Sha256,
        tpovArtifact.Length, "tpov",
        new[] { new DosOverlayRegion("candidate", 4, 2, 0, 0, "code-candidate", "synthetic") });
    Equal(tpov.DecodeExplicit(tpovRegion, "candidate", 0, 2)[1].OriginalBytes,
        "C3", "TPOV explicitly mapped region can use shared x86-16 decoder");
    var regionRoundtrip = DosOverlayImage.ParseRegionManifest(
        JsonSerializer.Serialize(regionManifest, new JsonSerializerOptions(JsonSerializerDefaults.Web)));
    Equal(fbov.ValidateRegions(regionRoundtrip).Count, 1,
        "portable source-bound overlay map preserves its region boundaries");
    Console.WriteLine("PASS: conservative FBOV/TPOV envelope variants and source-bound explicit regions");

    // Resident Turbo Pascal overlay directory: no source-specific offsets,
    // synthetic EXE + separate OVR original bytes and exact chain closure.
    byte[] residentOverlayBytes = (byte[])mzBytes.Clone();
    const int residentDescriptor = 0x80;
    new byte[] { 0xCD, 0x3F, 0, 0 }.CopyTo(residentOverlayBytes, residentDescriptor);
    Dword(residentOverlayBytes, residentDescriptor + 4, 4); // first TPOV payload
    MzWord(residentOverlayBytes, residentDescriptor + 8, 4); // code length
    MzWord(residentOverlayBytes, residentDescriptor + 10, 2); // fixup length
    MzWord(residentOverlayBytes, residentDescriptor + 12, 1); // one stub
    MzWord(residentOverlayBytes, residentDescriptor + 14, 7); // opaque unit number
    residentOverlayBytes[residentDescriptor + 32] = 0xCD;
    residentOverlayBytes[residentDescriptor + 33] = 0x3F;
    var residentOverlayImage = DosMzImage.Load(
        BinaryArtifact.FromBytes(residentOverlayBytes, "START.EXE"));
    byte[] syntheticTpOverlay = { (byte)'T', (byte)'P', (byte)'O', (byte)'V',
        0xB8, 0x34, 0x12, 0xC3, 0xCD, 0x3F };
    var mappedTpOverlay = DosOverlayImage.LoadEnvelope(
        BinaryArtifact.FromBytes(syntheticTpOverlay, "GAME.OVR"));
    var resolvedDirectory = DosOverlayDirectoryRecovery.Recover(
        residentOverlayImage, mappedTpOverlay);
    Equal(resolvedDirectory.Units.Count, 1, "one complete resident-derived overlay unit");
    Equal(resolvedDirectory.Units[0].ResidentDescriptorFileOffset, 0x80L,
        "resident descriptor retains exact original EXE file offset");
    Equal(resolvedDirectory.Units[0].OverlayCodeFileOffset, 4,
        "resident descriptor's overlay code offset is preserved");
    Equal(resolvedDirectory.Units[0].OverlayFixupFileOffset, 8,
        "overlay relocation blob is not confused with candidate instructions");
    Equal(resolvedDirectory.Units[0].FixupByteLength, 2,
        "overlay fixup bytes remain separately accounted");
    Equal(resolvedDirectory.Units[0].ProcedureCount, 1,
        "procedure-trap stubs validated from resident MZ bytes");
    Equal(resolvedDirectory.CoveredPayloadBytes, 6,
        "stored code plus fixups must cover every overlay payload byte");
    var opaqueAudit = DosOverlayFixupAudit.Analyze(residentOverlayImage, mappedTpOverlay);
    Equal(opaqueAudit.TotalCodeBytes, 4, "overlay code remains a distinct stored span");
    Equal(opaqueAudit.TotalOpaqueFixupBytes, 2, "opaque fixup span is accounted for");
    Equal(opaqueAudit.Units[0].CodeSha256?.Length, 64, "stored code has a complete source SHA-256");
    Equal(opaqueAudit.Units[0].FixupSha256?.Length, 64, "opaque fixup span has source SHA-256");
    Fails(() => DosOverlayFixupAudit.Analyze(mzImage, mappedTpOverlay),
        "opaque fixup auditing cannot associate an unrelated MZ with an overlay");

    Equal(resolvedDirectory.ResidentSourceSha256, residentOverlayImage.SourceSha256,
        "resident original identity retained independently of overlay");
    Equal(resolvedDirectory.OverlaySourceSha256, mappedTpOverlay.Envelope.SourceSha256,
        "overlay original identity retained independently of resident");
    Fails(() => DosOverlayDirectoryRecovery.Recover(mzImage, mappedTpOverlay),
        "unrelated MZ without a resident directory cannot invent overlay boundaries");
    var missingDescriptor = (byte[])residentOverlayBytes.Clone();
    Dword(missingDescriptor, residentDescriptor + 4, 5);
    Fails(() => DosOverlayDirectoryRecovery.Recover(
        DosMzImage.Load(BinaryArtifact.FromBytes(missingDescriptor)), mappedTpOverlay),
        "overlay descriptor with a one-byte coverage gap must be rejected");
    var duplicateDescriptor = (byte[])residentOverlayBytes.Clone();
    Array.Copy(duplicateDescriptor, residentDescriptor, duplicateDescriptor, 0xE0, 37);
    Fails(() => DosOverlayDirectoryRecovery.Recover(
        DosMzImage.Load(BinaryArtifact.FromBytes(duplicateDescriptor)), mappedTpOverlay),
        "ambiguous duplicate resident descriptors must not select an arbitrary map");
    var badStub = (byte[])residentOverlayBytes.Clone();
    badStub[residentDescriptor + 32] = 0x90;
    Fails(() => DosOverlayDirectoryRecovery.Recover(
        DosMzImage.Load(BinaryArtifact.FromBytes(badStub)), mappedTpOverlay),
        "resident overlay descriptors require structurally matching trap stubs");
    var zeroProcedures = (byte[])residentOverlayBytes.Clone();
    MzWord(zeroProcedures, residentDescriptor + 12, 0);
    Fails(() => DosOverlayDirectoryRecovery.Recover(
        DosMzImage.Load(BinaryArtifact.FromBytes(zeroProcedures)), mappedTpOverlay),
        "trap-marker-only overlay candidates without checked procedure stubs are insufficient");
    var wrongFixupSize = (byte[])residentOverlayBytes.Clone();
    MzWord(wrongFixupSize, residentDescriptor + 10, 3);
    Fails(() => DosOverlayDirectoryRecovery.Recover(
        DosMzImage.Load(BinaryArtifact.FromBytes(wrongFixupSize)), mappedTpOverlay),
        "overlay directory cannot claim code/fixup bytes past source end");
    Fails(() => DosOverlayDirectoryRecovery.Recover(
        residentOverlayImage, DosOverlayImage.LoadEnvelope(BinaryArtifact.FromBytes(expandedFbov))),
        "16-byte FBOV variant cannot silently reuse TPOV manager descriptor layout");
    Console.WriteLine("PASS: resident MZ overlay descriptors form a unique gap-free TPOV directory");

    // P0 universal 16-bit candidate graph: direct calls, branch targets,
    // interrupts and indirect calls retain different uncertainty classes.
    var mz16Graph = Dos16ControlFlowAnalysis.Analyze(
        mzImage.CreateEntryCodeRegion(), mzImage.InitialIp);
    Equal(mz16Graph.Instructions.Count, 2, "MZ entry CS produces a bounded two-instruction graph");
    Equal(mz16Graph.Instructions[0].SourceFileOffset, 32L,
        "MZ flow nodes preserve original physical file positions");
    Equal(mz16Graph.AnalysisStatus.Contains("candidate-unverified", StringComparison.Ordinal),
        true, "16-bit graph never asserts independently verified code");

    byte[] cfg16Bytes = {
        0x83, 0xF8, 0x02, // 0100: cmp ax,2
        0x77, 0x05,       // 0103: ja 010A
        0xE8, 0x06, 0x00, // 0105: call 010E
        0xCD, 0x21,       // 0108: int 21
        0xFF, 0xD0,       // 010A: call ax
        0xC3,             // 010C: ret
        0x90,             // 010D: unknown / not traversed
        0xC3              // 010E: target ret
    };
    var cfg16Source = BinaryArtifact.FromBytes(cfg16Bytes, "some-headerless-input.bin");
    var cfg16Com = DosComImage.LoadExplicit(cfg16Source);
    var cfg16 = Dos16ControlFlowAnalysis.Analyze(cfg16Com.CreateCodeRegion(), 0x100);
    Equal(cfg16.SourceSha256, cfg16Source.Sha256,
        "one original source hash scopes the generic x86-16 graph");
    Equal(cfg16.Instructions.Count, 7,
        "recursive flow follows near calls and conditional branches, not gap sweep");
    Equal(cfg16.CandidateInstructionBytes, 14,
        "16-bit candidate ownership counts only decoded bytes");
    Equal(cfg16.UnknownFileBackedBytes, 1,
        "undecoded executable-region byte remains explicitly unknown");
    Equal(cfg16.Edges.Any(e => e.Kind == "branch-taken" &&
        e.Destination == new Dos16Address(0, 0x10A)), true,
        "16-bit near branch target is mapped to same segment");
    Equal(cfg16.Edges.Any(e => e.Kind == "call" &&
        e.Destination == new Dos16Address(0, 0x10E)), true,
        "16-bit direct near call destination is recovered");
    Equal(cfg16.Edges.Any(e => e.Kind == "unresolved-indirect-call" &&
        e.Destination is null), true, "dynamic x86-16 calls remain unresolved");
    Equal(cfg16.Edges.Any(e => e.Kind == "unresolved-interrupt" &&
        e.Destination is null), true, "DOS interrupt return semantics are not fabricated");
    Equal(cfg16.Edges.Any(e => e.Kind == "interrupt-return-candidate"), true,
        "possible post-interrupt continuation stays explicitly speculative");
    Equal(cfg16.Instructions.Any(x => x.Address.Offset == 0x10D), false,
        "unreachable filler is not automatically classified as code");

    var budgetGraph16 = Dos16ControlFlowAnalysis.Analyze(cfg16Com.CreateCodeRegion(),
        0x100, maxInstructions: 1);
    Equal(budgetGraph16.Truncated, true, "small 16-bit analysis budget reports truncation");
    Equal(budgetGraph16.Instructions.Count, 1,
        "instruction budget bounds recursive decode");

    var midTargetSource = BinaryArtifact.FromBytes(new byte[] {
        0xB8, 0x34, 0x12, // mov ax,1234h
        0xEB, 0xFC        // short jmp 0101, inside MOV
    }, "mid-target.com");
    var midGraph16 = Dos16ControlFlowAnalysis.Analyze(
        DosComImage.LoadExplicit(midTargetSource).CreateCodeRegion(), 0x100);
    Equal(midGraph16.Diagnostics.Any(x => x.Code == "mid-instruction-target"),
        true, "overlapping x86-16 branch target rejected rather than reinterpreting MOV");
    Equal(midGraph16.Instructions.Count, 2,
        "rejected mid-instruction target is not another decoded instruction");

    var escapedSource = BinaryArtifact.FromBytes(new byte[] { 0xEB, 0x7F },
        "unbacked-branch.com");
    var escapedGraph = Dos16ControlFlowAnalysis.Analyze(
        DosComImage.LoadExplicit(escapedSource).CreateCodeRegion(), 0x100);
    Equal(escapedGraph.Edges.Any(e => e.Kind == "out-of-region-branch"),
        true, "control transfer outside backed load window is unresolved");
    Equal(escapedGraph.Instructions.Count, 1,
        "out-of-region branch cannot create fictional code");

    var overlaid16 = Dos16ControlFlowAnalysis.Analyze(
        fbov.CreateCodeRegion(regionManifest, "entry"), 0x100);
    Equal(overlaid16.Instructions.Count, 2,
        "hash-bound overlay code region uses identical generic x86-16 CFG engine");
    Equal(overlaid16.Instructions[1].SourceFileOffset, 11L,
        "overlay graph retains original code bytes and physical source offsets");
    bool overlayDataGraphDenied = false;
    try { fbov.CreateCodeRegion(regionManifest with {
        Regions = new[] { mappedRegion with { Role = "data-candidate" } }
    }, "entry"); }
    catch (NotSupportedException) { overlayDataGraphDenied = true; }
    Equal(overlayDataGraphDenied, true,
        "data-marked overlay region cannot be promoted to code CFG implicitly");
    Console.WriteLine("PASS: universal MZ COM and overlay x86-16 candidate CFG/uncertainty controls");

    // Exact-byte identity catalogs label sources but never change decoding.
    string syntheticCatalogJson = JsonSerializer.Serialize(new {
        schemaVersion = 1,
        entries = new[] { new {
            sha256 = mzArtifact.Sha256, byteLength = mzArtifact.Length,
            gameId = "fixture", module = "MAIN.EXE",
            sourceEvidence = "synthetic-mz-header",
            software = "Generic test fixture"
        } }
    });
    var identityRegistry = SourceIdentityRegistry.FromJson(syntheticCatalogJson);
    Equal(identityRegistry.Count, 1, "one registry entry accepted");
    Equal(identityRegistry.Find(mzArtifact)?.GameId, "fixture",
        "catalog matches exact SHA-256 and byte length");
    Equal(FileIntelligence.Inspect(mzArtifact, registry: identityRegistry)["software"].Status,
        "external-catalog-exact-hash", "catalog facts are distinct from binary structural evidence");
    Equal(FileIntelligence.Inspect(mzArtifact).ContainsKey("software"), false,
        "no game identity is hardcoded into file intelligence");
    var catalogIdentity = SourceProvenance.ForArtifact(mzArtifact, gameId: "fixture",
        registry: identityRegistry);
    Equal(catalogIdentity.IdentityStatus, "catalog-matched-analyst-supplied",
        "catalog match is not automatic code verification");
    Equal(DosMzImage.Load(mzArtifact).Decode(0, 0, 2).Count, mzLines.Count,
        "presence of an identity registry cannot change decoded instructions");
    bool wrongCatalogGameRejected = false;
    try { SourceProvenance.ForArtifact(mzArtifact, gameId: "foreign",
        registry: identityRegistry); }
    catch (InvalidDataException) { wrongCatalogGameRejected = true; }
    Equal(wrongCatalogGameRejected, true,
        "catalog fingerprint prevents contradictory game labels");
    Fails(() => SourceIdentityRegistry.FromJson(JsonSerializer.Serialize(new {
        schemaVersion = 1,
        entries = new[] {
            new { sha256 = mzArtifact.Sha256, byteLength = mzArtifact.Length,
                gameId = "fixture", module = "MAIN.EXE", sourceEvidence = "first" },
            new { sha256 = mzArtifact.Sha256, byteLength = mzArtifact.Length,
                gameId = "other", module = "GAME.EXE", sourceEvidence = "second" }
        }
    })), "conflicting duplicate fingerprint catalog entries rejected");
    Console.WriteLine("PASS: optional hash/length catalogs, namespace isolation and decoder independence");

    // Format classification must discover bounded nested MZ/LE modules,
    // even when the outer DOS launcher has no standard e_lfanew.
    // This prevents a 16-bit loader from misreading the embedded module.
    foreach (bool embedded in new[] { false, true })
    {
        byte[] fixture = MakeFixture(embedded);
        var artifact = BinaryArtifact.FromBytes(fixture, "neutral-binary.dat");
        var signature = BinaryFormatDetector.Inspect(artifact);
        Equal(signature.FormatId, "le-signature",
            "nested and direct MZ-bound LE modules receive the same format family");
        Equal(signature.SignatureFileOffset, embedded ? 0x200L : 0x80L,
            "the extended header position must be source-physical, not guessed relative to outer MZ");
        Equal(signature.LoaderStatus, "le-validation-required",
            "LE signature alone is not proof of a validated object/page loader");
        Fails(() => DosMzImage.Load(artifact),
            "nested protected-mode image must not silently become a standalone 16-bit MZ");
        Equal(BinaryFormatDetector.Inspect(artifact, LeImage.Load(artifact)).FormatId,
            "le", "validated LE loader supersedes signature-only classification");
    }
    // Explicit DOS-launcher mode must not mix the outer 16-bit stub
    // with an independently MZ-bound protected-mode module.
    byte[] launcherBytes = MakeFixture(nested: true);
    MzWord(launcherBytes, 2, 0x100); // 256-byte original DOS MZ extent
    MzWord(launcherBytes, 4, 1);
    MzWord(launcherBytes, 6, 0); // no outer DOS fixups
    MzWord(launcherBytes, 8, 2); // 32-byte outer DOS header
    MzWord(launcherBytes, 0x14, 0); // DOS entry 0000:0000
    MzWord(launcherBytes, 0x16, 0);
    launcherBytes[0x20] = 0xC3; // independent real-mode RET fixture
    var launcherArtifact = BinaryArtifact.FromBytes(launcherBytes, "launcher-with-nested-module.bin");
    Equal(BinaryFormatDetector.Inspect(launcherArtifact).FormatId, "le-signature",
        "outer DOS launcher is still classified by the nested module signature");
    var dosLauncher = DosMzImage.LoadLauncherStub(launcherArtifact);
    Equal(dosLauncher.IsLauncherStub, true, "explicit launcher mode carries a separate evidence label");
    Equal(dosLauncher.DeclaredExecutableFileLength, 0x100,
        "outer DOS MZ declared extent is separate from later protected-mode bytes");
    Equal(dosLauncher.LoadImageFileOffset, 0x20,
        "DOS launcher header geometry remains a normal source-backed MZ module");
    Equal(dosLauncher.Decode(0, 0, 1)[0].OriginalBytes, "C3",
        "opt-in outer DOS launcher can decode its own declared real-mode entry");
    Equal(Dos16ControlFlowAnalysis.Analyze(dosLauncher.CreateEntryCodeRegion(), 0)
        .MappingEvidence.Contains("explicit-outer-DOS-MZ-launcher", StringComparison.Ordinal),
        true, "launcher CFG retains explicit DOS-stub scope rather than protected-mode identity");
    Fails(() => DosMzImage.Load(launcherArtifact),
        "default MZ loader never silently treats nested protected-mode inputs as 16-bit code");

    byte[] ambiguousOuterExtent = (byte[])launcherBytes.Clone();
    MzWord(ambiguousOuterExtent, 2, 0); // declared MZ extent now 512 bytes
    Fails(() => DosMzImage.LoadLauncherStub(
        BinaryArtifact.FromBytes(ambiguousOuterExtent)),
        "DOS launcher declared load extent cannot overlap an embedded MZ module");
    Fails(() => DosMzImage.LoadLauncherStub(
        BinaryArtifact.FromBytes(MakeFixture(false))),
        "a direct MZ/LE module has no separate outer DOS launcher to map");

    // Bare accidental LE bytes do not identify a nested module without a
    // corresponding well-bounded nested MZ and e_lfanew.
    var looseSignature = new byte[0x180];
    looseSignature[0] = (byte)'M'; looseSignature[1] = (byte)'Z';
    looseSignature[0x80] = (byte)'L'; looseSignature[0x81] = (byte)'E';
    Equal(BinaryFormatDetector.Inspect(BinaryArtifact.FromBytes(looseSignature)).FormatId,
        "mz", "a stray LE marker without a linked MZ header is not trusted");
    Dword(looseSignature, 0x3C, uint.MaxValue);
    Equal(BinaryFormatDetector.Inspect(BinaryArtifact.FromBytes(looseSignature)).FormatId,
        "mz", "overflow-sized e_lfanew is ignored rather than dereferenced");
    var mixedExtended = new byte[0x800];
    MakeFixture(false).CopyTo(mixedExtended, 0);
    var appendedLx = MakeFixture(false);
    appendedLx[0x80] = (byte)'L';
    appendedLx[0x81] = (byte)'X';
    Array.Copy(appendedLx, 0, mixedExtended, 0x400, 0x100);
    var mixedExtendedArtifact = BinaryArtifact.FromBytes(mixedExtended, "unlabeled-wrapped.bin");
    var mixedLocations = MzExtendedModuleLocator.Scan(mixedExtendedArtifact);
    Equal(mixedLocations.Count, 2,
        "shared locator finds both independently bounded MZ extended modules");
    Equal(mixedLocations[0].FormatId, "le-signature",
        "first source-relative MZ module retains its LE format");
    Equal(mixedLocations[1].FormatId, "lx",
        "second source-relative MZ module retains its LX format");
    Equal(BinaryFormatDetector.Inspect(mixedExtendedArtifact).FormatId,
        "ambiguous-mz-extended",
        "mixed extended formats are ambiguous rather than first-format-wins");
    Fails(() => LeImage.Load(mixedExtendedArtifact),
        "LE loader cannot ignore another valid MZ-bound extended module");
    Fails(() => DosMzImage.LoadLauncherStub(mixedExtendedArtifact),
        "a direct outer LE module must not become a fictional DOS launcher");

    Console.WriteLine("PASS: explicit DOS launcher isolation and shared nested MZ/LE locator");

    var ambiguousImage = new byte[0x800];
    MakeFixture(false).CopyTo(ambiguousImage, 0);
    byte[] extraMZ = MakeFixture(false);
    Array.Copy(extraMZ, 0, ambiguousImage, 0x400, 0x100);
    // Relocate the second MZ-bound LE header consistently with its
    // module-local e_lfanew, providing two distinct nested signatures.
    Equal(BinaryFormatDetector.Inspect(BinaryArtifact.FromBytes(ambiguousImage)).FormatId,
        "ambiguous-mz-extended",
        "multiple extended module candidates require manual selection");

    foreach (bool nested in new[] { false, true })
    {
        byte[] raw = MakeFixture(nested);
        var art = BinaryArtifact.FromBytes(raw);
        var image = LeImage.Load(art);
        Equal(image.ModuleOrigin, nested ? 0x180L : 0L, "nested module origin");
        Equal(image.Objects.Count, 1, "object count");
        Equal(image.ToFileOffset(1, 0), nested ? 0x380L : 0x200L, "file mapping");
        Equal(image.ToFileOffset(1, 4), (long?)null, "object out-of-range");
        Equal(Disassembly.Decode(image, 1, 0, 2)[1].Assembly, "ret", "decode RET");
        Equal(Disassembly.Decode(image, 1, 0, 2)[0].Bytes, "90", "encoded byte");
        raw[0] = 0; // original snapshot must not change
        Equal(art.Data[0], (byte)'M', "immutability");
    }
    // Internal absolute 32-bit fixup; original stored bytes remain immutable.
    var fixedPage = MakeFixture(false);
    Dword(fixedPage, 0x80 + 0x68, 0xe0);
    Dword(fixedPage, 0x80 + 0x6c, 0xe8);
    Dword(fixedPage, 0x80 + 0xe4, 7);
    new byte[] { 0x07, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00 }
        .CopyTo(fixedPage, 0x80 + 0xe8);
    var fixedImage = LeImage.Load(BinaryArtifact.FromBytes(fixedPage));
    Equal(fixedImage.Relocations.Count, 1, "synthetic fixup count");
    Equal(Convert.ToHexString(fixedImage.ReadStoredBytes(1, 0, 4)), "90C390C3", "original fixup bytes unchanged");
    Equal(Convert.ToHexString(fixedImage.ReadRelocatedBytes(1, 0, 4)), "02000100", "internal absolute fixup");

    // 32-bit target and additive values are parsed at their exact encoded widths.
    var additivePage = (byte[])fixedPage.Clone();
    Dword(additivePage, 0x80 + 0xe4, 13);
    new byte[] { 0x07, 0x34, 0x00, 0x00, 0x01, 0x05, 0x00,
                 0x00, 0x00, 0x06, 0x00, 0x00, 0x00 }
        .CopyTo(additivePage, 0x80 + 0xe8);
    Equal(Convert.ToHexString(LeImage.Load(BinaryArtifact.FromBytes(additivePage))
        .ReadRelocatedBytes(1, 0, 4)), "0B000100", "32-bit target plus additive");
    var relativePage = (byte[])fixedPage.Clone();
    relativePage[0x80 + 0xe8] = 0x08;
    relativePage[0x80 + 0xe8 + 5] = 0x0a;
    Equal(Convert.ToHexString(LeImage.Load(BinaryArtifact.FromBytes(relativePage))
        .ReadRelocatedBytes(1, 0, 4)), "06000000", "self-relative fixup");

    // LE fixup source lists are compact and can patch multiple sites.
    var sources = MakeFixture(false);
    Dword(sources, 0x80 + 0xc4, 12);
    Dword(sources, 0x80 + 0x2c, 12);
    Dword(sources, 0x80 + 0x68, 0xe0);
    Dword(sources, 0x80 + 0x6c, 0xe8);
    Dword(sources, 0x80 + 0xe4, 10);
    new byte[] { 0x27, 0x00, 0x02, 0x01, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00 }
        .CopyTo(sources, 0x80 + 0xe8);
    var listed = LeImage.Load(BinaryArtifact.FromBytes(sources));
    Equal(listed.Relocations.Count, 2, "source list count");
    Equal(Convert.ToHexString(listed.ReadRelocatedBytes(1, 0, 8)),
        "0200010002000100", "source list targets");

    // Signed source offsets can point back across a page boundary.
    var crossed = MakeFixture(false);
    Dword(crossed, 0x80 + 0x14, 2);
    Dword(crossed, 0x80 + 0x28, 16);
    Dword(crossed, 0x80 + 0x2c, 16);
    Dword(crossed, 0x80 + 0xc4, 32);
    Dword(crossed, 0x80 + 0xd4, 2);
    crossed[0x80 + 0xe0 + 2] = 2;
    Dword(crossed, 0x80 + 0x68, 0xe4);
    Dword(crossed, 0x80 + 0x6c, 0xf0);
    Dword(crossed, 0x80 + 0xe4 + 8, 7);
    new byte[] { 0x07, 0x00, 0xfe, 0xff, 0x01, 0x04, 0x00 }
        .CopyTo(crossed, 0x80 + 0xf0);
    var across = LeImage.Load(BinaryArtifact.FromBytes(crossed));
    Equal(across.Relocations.Count, 1, "cross-boundary record count");
    Equal(across.Relocations[0].SourceOffset, 14L, "negative signed source offset");
    Equal(Convert.ToHexString(across.ReadRelocatedBytes(1, 14, 4)),
        "04000100", "cross-page relocated bytes");
    var truncatedFixup = (byte[])fixedPage.Clone();
    Dword(truncatedFixup, 0x80 + 0xe4, 6);
    Fails(() => LeImage.Load(BinaryArtifact.FromBytes(truncatedFixup)), "truncated fixup record");
    var invalidFixupPage = (byte[])fixedPage.Clone();
    Dword(invalidFixupPage, 0x80 + 0xe4, 0xffffffff);
    Fails(() => LeImage.Load(BinaryArtifact.FromBytes(invalidFixupPage)), "invalid fixup page boundary");
    var unsupportedFixup = (byte[])fixedPage.Clone();
    unsupportedFixup[0x80 + 0xe8] = 0x02;
    Fails(() => LeImage.Load(BinaryArtifact.FromBytes(unsupportedFixup)), "unsupported selector fixup");
    var invalidMap = MakeFixture(false);
    invalidMap[0x80 + 0xdc + 3] = 5;
    Fails(() => LeImage.Load(BinaryArtifact.FromBytes(invalidMap)), "unsupported page kind");
    var invalidHeader = MakeFixture(false);
    Dword(invalidHeader, 0x80 + 0x40, uint.MaxValue);
    Fails(() => LeImage.Load(BinaryArtifact.FromBytes(invalidHeader)), "invalid object offset");
    var tooShort = new byte[] { (byte)'M', (byte)'Z' };
    Fails(() => LeImage.Load(BinaryArtifact.FromBytes(tooShort)), "truncated MZ");
    // Part 16: only reachable code is proposed. Literal gaps stay unknown,
    // while explicit analyst-protected data cannot be decoded accidentally.
    var graphBytes = MakeFixture(false);
    Dword(graphBytes, 0x80 + 0xc4, 32);
    Dword(graphBytes, 0x80 + 0x2c, 32);
    int dataStart = 0x200;
    new byte[] { 0xE8, 0x0B, 0, 0, 0, 0xC3 }.CopyTo(graphBytes, dataStart);
    new byte[] { 0xDE, 0xAD, 0xBE, 0xEF }.CopyTo(graphBytes, dataStart + 6);
    new byte[] { 0x75, 0xF3, 0xFF, 0xE0 }.CopyTo(graphBytes, dataStart + 0x10);
    var graphImage = LeImage.Load(BinaryArtifact.FromBytes(graphBytes));
    using (var cancelled = new CancellationTokenSource())
    {
        cancelled.Cancel();
        bool cancelledAsRequested = false;
        try { ControlFlowAnalysis.Analyze(graphImage, cancellationToken: cancelled.Token); }
        catch (OperationCanceledException) { cancelledAsRequested = true; }
        Equal(cancelledAsRequested, true, "cancelled analysis must stop without publishing partial findings");
    }
    var graph = ControlFlowAnalysis.Analyze(graphImage, protectedData: new[] {
        new ProtectedDataRange(1, 6, 10, "synthetic-known-data") });
    // Part 32: opt-in evidence must not alter the existing CFG analysis.
    var trace = new AnalysisDecisionTrace();
    var traced = ControlFlowAnalysis.Analyze(graphImage, protectedData: new[] {
        new ProtectedDataRange(1, 6, 10, "synthetic-known-data") }, decisionTrace: trace);
    Equal(traced.Instructions.Count, graph.Instructions.Count, "tracing preserves decoded candidates");
    Equal(traced.Edges.Count, graph.Edges.Count, "tracing preserves CFG edges");
    Equal(trace.Snapshot().Any(x => x.Outcome == AnalysisDecisionOutcome.DirectTargetAlreadyKnown &&
        x.Destination == new FlowAddress(1, 5)), true,
        "shared-return edge is not mislabeled as a newly queued address");
    Equal(trace.ArtifactSha256, graphImage.ArtifactSha256, "trace source identity");
    Equal(trace.Snapshot().Any(x => x.Outcome == AnalysisDecisionOutcome.ProtectedRangeDeclared),
        true, "protected range is evidence-linked");
    Equal(trace.Snapshot().Any(x => x.Outcome == AnalysisDecisionOutcome.DecodedCandidate &&
        x.Address == new FlowAddress(1, 0) && x.FileOffset == graphImage.ToFileOffset(1, 0)),
        true, "candidate has original file and object offsets");
    using (var traceJson = JsonDocument.Parse(trace.BuildJson()))
    {
        Equal(traceJson.RootElement.GetProperty("schemaVersion").GetInt32(), 1, "trace schema version");
        Equal(traceJson.RootElement.GetProperty("analysisCompleted").GetBoolean(), true,
            "normally returned analysis is recorded as completed");
        Equal(traceJson.RootElement.GetProperty("analysisTruncated").GetBoolean(), false,
            "full bounded fixture is not marked as instruction-budget truncated");
        Equal(traceJson.RootElement.GetProperty("sourceSha256").GetString(),
            graphImage.ArtifactSha256, "trace JSON source identity");
        Equal(traceJson.RootElement.GetProperty("privacy").GetProperty("originalBytesIncluded").GetBoolean(),
            false, "trace contains no original opcode bytes");
    }
    Equal(trace.BuildJson().Contains("synthetic-known-data", StringComparison.Ordinal),
        false, "analyst prose is not included in trace");

    // Part 33: local decision trace export is opt-in, atomic, bounded
    // and never overwrites analyst evidence.
    string traceFolder = Path.Combine(Path.GetTempPath(),
        "dosdis-decision-trace-" + Guid.NewGuid().ToString("N"));
    Directory.CreateDirectory(traceFolder);
    try
    {
        string tracePath = Path.Combine(traceFolder, "decisions.json");
        trace.WriteNew(tracePath);
        string previous = File.ReadAllText(tracePath);
        Equal(previous, trace.BuildJson(), "saved trace matches evidence snapshot");
        Equal(previous.Contains("synthetic-known-data", StringComparison.Ordinal),
            false, "export excludes private analyst provenance");
        bool overwriteDenied = false;
        try { trace.WriteNew(tracePath); }
        catch (IOException) { overwriteDenied = true; }
        Equal(overwriteDenied, true, "decision trace cannot overwrite previous export");
        Equal(File.ReadAllText(tracePath), previous, "prior trace preserved on refused overwrite");
        bool extensionDenied = false;
        try { trace.WriteNew(Path.Combine(traceFolder, "decisions.txt")); }
        catch (ArgumentException) { extensionDenied = true; }
        Equal(extensionDenied, true, "decision trace rejects unrelated export extensions");
        bool emptyDenied = false;
        try { new AnalysisDecisionTrace().WriteNew(Path.Combine(traceFolder, "empty.json")); }
        catch (InvalidOperationException) { emptyDenied = true; }
        Equal(emptyDenied, true, "unstarted trace cannot be exported");
        Equal(Directory.EnumerateFiles(traceFolder, "*.part").Any(), false,
            "decision trace staging residue cleaned");
        Equal(graphImage.ArtifactSha256, graph.ArtifactSha256,
            "export leaves original source identity unchanged");
    }
    finally { Directory.Delete(traceFolder, recursive: true); }
    Console.WriteLine("PASS: opt-in atomic local decision trace export and overwrite protection");
    // P0: independently supplied known-answer expectations are constrained to
    // a full source SHA-256/length; they never rewrite candidate findings.
    var expectations = new KnownAnswerManifest(
        1, "synthetic", "CODE.1", graphImage.ArtifactSha256, graphBytes.Length,
        "LE", "synthetic-fixture-research",
        new[] {
            new KnownInstructionExpectation(new FlowAddress(1, 0), "E80B000000", "verified-call"),
            new KnownInstructionExpectation(new FlowAddress(1, 0x1D), "C3", "missing-entry")
        },
        new[] { new KnownNonCodeExpectation(1, 6, 10, "literal-data") },
        new[] { new KnownEdgeExpectation(new FlowAddress(1, 0),
            new FlowAddress(1, 0x10), "call", "verified-call-target") },
        Array.Empty<KnownFixupExpectation>());
    var validation = KnownAnswerValidator.Validate(graph, graphImage,
        BinaryArtifact.FromBytes(graphBytes), expectations);
    Equal(validation.GameId, "synthetic", "known-answer result keeps game namespace");
    Equal(validation.Matched, 3, "one instruction, one data protection and one flow edge");
    Equal(validation.NotObserved, 1, "unreached known entry does not become verified code");
    Equal(validation.Conflicting, 0, "synthetic graph has no contradiction");
    Equal(validation.UnbenchmarkedCandidateInstructions, 3,
        "partial fixture does not certify other decoded instructions");
    using (var validationJson = JsonDocument.Parse(KnownAnswerValidator.BuildJson(validation)))
    {
        Equal(validationJson.RootElement.GetProperty("analysisStatus").GetString(),
            "candidate-comparison-not-certification", "reference comparison never certifies entire analysis");
        Equal(validationJson.RootElement.GetProperty("findings").GetArrayLength(), 4,
            "all independent expectations retain explicit results");
    }
    bool rejectedForeignSource = false;
    try
    {
        KnownAnswerValidator.Validate(graph, graphImage, BinaryArtifact.FromBytes(graphBytes),
            expectations with { SourceSha256 = new string('0', 64), GameId = "stone" });
    }
    catch (InvalidDataException) { rejectedForeignSource = true; }
    Equal(rejectedForeignSource, true, "a Stone/Menzo manifest cannot silently validate Strahd or synthetic bytes");
    // Known-answer comparison cannot be tricked by a restored
    // graph with the same advertised artifact SHA but forged row
    // bytes: the independently reopened original LE page controls.
    var originalInstruction = graph.Instructions.Single(i =>
        i.Address == new FlowAddress(1, 0));
    var forgedRowGraph = graph with {
        Instructions = graph.Instructions.Select(i =>
            i.Address == new FlowAddress(1, 0)
                ? i with { OriginalBytes = "9090909090" }
                : i).ToArray()
    };
    var forgedRowManifest = expectations with {
        VerifiedInstructions = new[] {
            new KnownInstructionExpectation(new FlowAddress(1, 0),
                "9090909090", "forged-evidence")
        },
        VerifiedNonCode = Array.Empty<KnownNonCodeExpectation>(),
        VerifiedEdges = Array.Empty<KnownEdgeExpectation>()
    };
    var forgedRowCheck = KnownAnswerValidator.Validate(forgedRowGraph,
        graphImage, BinaryArtifact.FromBytes(graphBytes), forgedRowManifest);
    Equal(forgedRowCheck.Conflicting, 1,
        "same-sha forged graph row and manifest cannot override original LE bytes");
    Equal(forgedRowCheck.Matched, 0,
        "forged matching hex strings alone cannot claim known-answer match");

    var wrongLengthGraph = graph with {
        Instructions = graph.Instructions.Select(i =>
            i.Address == new FlowAddress(1, 0)
                ? i with { Length = originalInstruction.Length - 1 }
                : i).ToArray()
    };
    var wrongLengthCheck = KnownAnswerValidator.Validate(wrongLengthGraph,
        graphImage, BinaryArtifact.FromBytes(graphBytes), expectations);
    Equal(wrongLengthCheck.Conflicting, 1,
        "expected instruction byte-length mismatch is an explicit conflict");
    var flowExpectation = expectations with {
        VerifiedInstructions = new[] {
            new KnownInstructionExpectation(new FlowAddress(1, 0),
                "E80B000000", "external-flow-call",
                ExpectedFlowControl: "Call")
        }
    };
    var correctFlow = KnownAnswerValidator.Validate(graph, graphImage,
        BinaryArtifact.FromBytes(graphBytes), flowExpectation);
    Equal(correctFlow.Matched, 3,
        "expected source-backed flow class agrees with original CALL encoding");
    var forgedFlowGraph = graph with {
        Instructions = graph.Instructions.Select(i =>
            i.Address == new FlowAddress(1, 0)
                ? i with { FlowControl = "Return" }
                : i).ToArray()
    };
    var wrongFlow = KnownAnswerValidator.Validate(forgedFlowGraph,
        graphImage, BinaryArtifact.FromBytes(graphBytes), flowExpectation);
    Equal(wrongFlow.Conflicting, 1,
        "known-answer external flow category rejects incorrect graph classification");

    var invalidFlowExpectation = flowExpectation with {
        VerifiedInstructions = new[] {
            new KnownInstructionExpectation(new FlowAddress(1, 0),
                "E80B000000", "invalid-flow-class",
                ExpectedFlowControl: "TrustMeVerified")
        }
    };
    Fails(() => KnownAnswerValidator.Validate(graph, graphImage,
        BinaryArtifact.FromBytes(graphBytes), invalidFlowExpectation),
        "unknown external flow-control categories cannot claim comparison success");

    var numericFlowExpectation = flowExpectation with {
        VerifiedInstructions = new[] {
            new KnownInstructionExpectation(new FlowAddress(1, 0),
                "E80B000000", "numeric-enum-not-a-named-oracle",
                ExpectedFlowControl: ((int)Iced.Intel.FlowControl.Call).ToString())
        }
    };
    Fails(() => KnownAnswerValidator.Validate(graph, graphImage,
        BinaryArtifact.FromBytes(graphBytes), numericFlowExpectation),
        "numeric flow-category aliases cannot replace named external expectations");

    var duplicateInstructionGraph = graph with {
        Instructions = graph.Instructions.Concat(new[] {
            originalInstruction
        }).ToArray()
    };
    Fails(() => KnownAnswerValidator.Validate(duplicateInstructionGraph,
        graphImage, BinaryArtifact.FromBytes(graphBytes), expectations),
        "known-answer validation rejects duplicate decoded instruction starts");

    Equal(forgedRowCheck.Findings.Single().DetailCode,
        "oracle-hex-disagrees-with-original-file",
        "byte-level independent oracle discrepancy has a stable review code");
    Equal(wrongLengthCheck.Findings.First().DetailCode,
        "decoded-instruction-length-mismatch",
        "byte count error distinguished from an incorrect original source");
    Equal(wrongFlow.Findings.First().DetailCode,
        "decoded-flow-class-disagrees-with-oracle",
        "independent flow category discrepancy remains separately reviewable");

    var duplicateExpectedInstruction = expectations with {
        VerifiedInstructions = expectations.VerifiedInstructions.Concat(new[] {
            expectations.VerifiedInstructions[0]
        }).ToArray()
    };
    Fails(() => KnownAnswerValidator.Validate(graph, graphImage,
        BinaryArtifact.FromBytes(graphBytes), duplicateExpectedInstruction),
        "duplicate independently claimed instruction cannot inflate match denominator");

    var duplicateExpectedEdge = expectations with {
        VerifiedEdges = expectations.VerifiedEdges.Concat(new[] {
            expectations.VerifiedEdges[0]
        }).ToArray()
    };
    Fails(() => KnownAnswerValidator.Validate(graph, graphImage,
        BinaryArtifact.FromBytes(graphBytes), duplicateExpectedEdge),
        "duplicate flow-edge claim cannot inflate verified result count");

    var overlappingExpectedData = expectations with {
        VerifiedNonCode = new[] {
            new KnownNonCodeExpectation(1, 6, 10, "first-noncode"),
            new KnownNonCodeExpectation(1, 9, 12, "overlapping-noncode")
        }
    };
    Fails(() => KnownAnswerValidator.Validate(graph, graphImage,
        BinaryArtifact.FromBytes(graphBytes), overlappingExpectedData),
        "overlapping reference data ranges cannot double-count their common bytes");

    var contradictoryExpectedData = expectations with {
        VerifiedNonCode = new[] {
            new KnownNonCodeExpectation(1, 4, 8, "conflicts-with-known-call")
        }
    };
    Fails(() => KnownAnswerValidator.Validate(graph, graphImage,
        BinaryArtifact.FromBytes(graphBytes), contradictoryExpectedData),
        "reference manifest cannot declare expected instruction bytes as non-code");

    var duplicateExpectedFixup = expectations with {
        VerifiedFixups = new[] {
            new KnownFixupExpectation(new FlowAddress(1, 0),
                new FlowAddress(1, 0x10), "fixup-evidence"),
            new KnownFixupExpectation(new FlowAddress(1, 0),
                new FlowAddress(1, 0x10), "duplicate-fixup")
        }
    };
    Fails(() => KnownAnswerValidator.Validate(graph, graphImage,
        BinaryArtifact.FromBytes(graphBytes), duplicateExpectedFixup),
        "duplicate relocation reference cannot count as separate independently verified findings");
    var wrongByteExpectation = expectations with {
        VerifiedInstructions = new[] {
            new KnownInstructionExpectation(new FlowAddress(1, 0), "9090909090", "wrong-hex")
        },
        VerifiedNonCode = Array.Empty<KnownNonCodeExpectation>(),
        VerifiedEdges = Array.Empty<KnownEdgeExpectation>()
    };
    var wrongByteReport = KnownAnswerValidator.Validate(graph, graphImage,
        BinaryArtifact.FromBytes(graphBytes), wrongByteExpectation);
    Equal(wrongByteReport.Conflicting, 1, "byte disagreement reported as conflict");
    string validationFolder = Path.Combine(Path.GetTempPath(), "dosdis-known-" + Guid.NewGuid().ToString("N"));
    Directory.CreateDirectory(validationFolder);
    try
    {
        string manifestPath = Path.Combine(validationFolder, "expectations.json");
        File.WriteAllText(manifestPath, JsonSerializer.Serialize(expectations,
            new JsonSerializerOptions(JsonSerializerDefaults.Web)));
        var restored = KnownAnswerValidator.ReadManifest(manifestPath);
        Equal(restored.SourceSha256, graphImage.ArtifactSha256,
            "known-answer manifest round-trip preserves original SHA-256");
        string resultPath = Path.Combine(validationFolder, "result.json");
        KnownAnswerValidator.WriteNew(resultPath, validation);
        string resultBytes = File.ReadAllText(resultPath);
        bool overwriteRejected = false;
        try { KnownAnswerValidator.WriteNew(resultPath, validation); }
        catch (IOException) { overwriteRejected = true; }
        Equal(overwriteRejected, true, "known-answer report never overwrites older comparison");
        Equal(File.ReadAllText(resultPath), resultBytes, "older validation report preserved");
        Equal(Directory.EnumerateFiles(validationFolder, "*.part").Any(), false,
            "validation export leaves no temporary files");
    }
    finally { Directory.Delete(validationFolder, recursive: true); }
    Console.WriteLine("PASS: source-bound known-answer validation, uncertainty and nonoverwriting export");
    // Part 25: user-triggered offline exports preserve evidence classification,
    // omit potentially proprietary instruction bytes by default, and never
    // overwrite an existing analyst report.
    var unknownSourceProvenance = SourceProvenance.ForArtifact(
        BinaryArtifact.FromBytes(graphBytes, "CODE.1"), graphImage, "stone",
        sourceArchive: "StoneProphet.zip", internalPath: "stone/CODE.1");
    Equal(unknownSourceProvenance.ResearchNamespace,
        "stone/" + graphImage.ArtifactSha256, "source namespace tied to full hash and game");
    Equal(unknownSourceProvenance.IdentityStatus, "analyst-declared-game-unverified",
        "unknown game identification is not promoted to verified fingerprint");
    Equal(unknownSourceProvenance.SourceArchive, "StoneProphet.zip",
        "source archive explicitly preserved without absolute host path");
    var menzoProvenance = SourceProvenance.ForArtifact(
        BinaryArtifact.FromBytes(graphBytes, "CODE.1"), graphImage, "menzo");
    Equal(menzoProvenance.ResearchNamespace == unknownSourceProvenance.ResearchNamespace,
        false, "matching filenames and bytes never merge research namespaces");
    bool rejectedRelativeTraversal = false;
    try { SourceProvenance.ForArtifact(BinaryArtifact.FromBytes(graphBytes, "CODE.1"),
        graphImage, "stone", internalPath: "../other/CODE.1"); }
    catch (ArgumentException) { rejectedRelativeTraversal = true; }
    Equal(rejectedRelativeTraversal, true, "provenance rejects parent-directory traversal");
    bool rejectedUnmatchedReport = false;
    try { AnalysisReportExport.BuildJson(graph, provenance: unknownSourceProvenance
        with { SourceSha256 = new string('0', 64) }); }
    catch (InvalidDataException) { rejectedUnmatchedReport = true; }
    Equal(rejectedUnmatchedReport, true, "reports cannot label another artifact's identity");
    string exportJson = AnalysisReportExport.BuildJson(graph, provenance: unknownSourceProvenance);
    using (var exported = JsonDocument.Parse(exportJson))
    {
        var root = exported.RootElement;
        Equal(root.GetProperty("format").GetString(), "dosdis-analysis-report", "versioned report identity");
        Equal(root.GetProperty("analysisStatus").GetString(), "candidate-unverified", "export cannot promote candidates");
        Equal(root.GetProperty("provenance").GetProperty("GameId").GetString(), "stone",
            "report embeds explicit game namespace");
        Equal(root.GetProperty("provenance").GetProperty("SourceSha256").GetString(), graph.ArtifactSha256,
            "report source provenance includes full SHA-256");
        Equal(root.GetProperty("provenance").GetProperty("IdentityStatus").GetString(),
            "analyst-declared-game-unverified", "game label is not verified by filename alone");
        Equal(root.GetProperty("counts").GetProperty("indirectCallTables").GetInt32(), 0,
            "ordinary graph has no candidate call table");
        Equal(root.GetProperty("counts").GetProperty("indirectJumpTables").GetInt32(), 0,
            "ordinary graph has no candidate jump table");
        Equal(root.GetProperty("counts").GetProperty("unresolvedIndirectJumps").GetInt32(), 1,
            "report tallies unresolved jumps independent of diagnostics event cap");
        Equal(root.GetProperty("counts").GetProperty("unresolvedIndirectCalls").GetInt32(), 0,
            "report distinguishes indirect calls from jumps");
        Equal(root.GetProperty("source").GetProperty("sha256").GetString(), graph.ArtifactSha256, "export source hash");
        Equal(root.GetProperty("source").GetProperty("sourceBinaryIncluded").GetBoolean(), false, "no complete original in report");
        Equal(root.GetProperty("source").GetProperty("sourcePathIncluded").GetBoolean(), false, "no original path in report");
        Equal(root.GetProperty("source").GetProperty("instructionBytesIncluded").GetBoolean(), false, "machine bytes opt-in only");
        Equal(root.GetProperty("instructions").GetArrayLength(), graph.Instructions.Count, "candidate instruction count exported");
        Equal(root.GetProperty("instructions")[0].TryGetProperty("originalBytes", out _), false,
            "default JSON unexpectedly embeds original machine bytes");
        Equal(root.GetProperty("dataReferences").ValueKind, JsonValueKind.Array, "evidence references included");
        Equal(root.GetProperty("jumpTableCandidates").ValueKind, JsonValueKind.Array, "candidate jump tables included");
    }
    using (var optedIn = JsonDocument.Parse(AnalysisReportExport.BuildJson(graph, includeInstructionBytes: true)))
        Equal(optedIn.RootElement.GetProperty("instructions")[0].GetProperty("originalBytes").GetString(),
            graph.Instructions[0].OriginalBytes, "explicit byte opt-in includes correct original instruction bytes");
    string exportText = AnalysisReportExport.BuildText(graph);
    Equal(exportText.Contains("CANDIDATE / UNVERIFIED", StringComparison.Ordinal), true,
        "text report must explicitly mark uncertainty");
    Equal(exportText.Contains("[DIAGNOSTICS]", StringComparison.Ordinal), true,
        "text report includes diagnostics section");
    string originalHash = graphImage.ArtifactSha256;
    string tempExportDir = Path.Combine(Path.GetTempPath(), "dosdis-export-" + Guid.NewGuid().ToString("N"));
    Directory.CreateDirectory(tempExportDir);
    try
    {
        string jsonPath = Path.Combine(tempExportDir, "report.json");
        AnalysisReportExport.WriteNew(jsonPath, graph, asJson: true);
        Equal(File.ReadAllText(jsonPath).Contains("dosdis-analysis-report", StringComparison.Ordinal), true,
            "atomic JSON report publication");
        bool refusedToOverwrite = false;
        try { AnalysisReportExport.WriteNew(jsonPath, graph, asJson: true); }
        catch (IOException) { refusedToOverwrite = true; }
        Equal(refusedToOverwrite, true, "report writer must refuse to overwrite prior evidence");
        Equal(Directory.EnumerateFiles(tempExportDir, "*.part", SearchOption.TopDirectoryOnly).Any(), false,
            "report writer leaked temporary data");
        AnalysisReportExport.WriteNew(Path.Combine(tempExportDir, "report.txt"), graph, asJson: false);
        Equal(graphImage.ArtifactSha256, originalHash, "export must never change original artifact identity");
    }
    finally { Directory.Delete(tempExportDir, recursive: true); }
    Console.WriteLine("PASS: candidate-only JSON/text export, machine-byte opt-in, atomic report publication");
    Equal(graph.ArtifactSha256, BinaryArtifact.FromBytes(graphBytes).Sha256, "graph source identity");
    Equal(graph.Instructions.Count, 4, "reachable-only decoded starts");
    Equal(graph.Blocks.Count, 4, "basic blocks and branch leaders");
    Equal(graph.FunctionCandidates.Count, 2, "call target discovers candidate callee");
    Equal(graph.Edges.Any(e => e.Kind == "call" && e.Destination?.Offset == 0x10), true, "direct call reference");
    Equal(graph.Edges.Any(e => e.Kind == "branch-taken" && e.Destination?.Offset == 5), true, "backward branch into shared RET");
    Equal(graph.Edges.Any(e => e.Kind == "unresolved-indirect-jump" && e.Destination is null), true, "indirect branch stays unresolved");
    Equal(graph.Diagnostics.Any(d => d.Code == "unresolved-indirect-jump" &&
        d.Address == new FlowAddress(1, 0x12)), true,
        "unresolved indirect branches appear explicitly in diagnostics");
    Equal(AnalysisReportExport.BuildText(graph).Contains("Unresolved indirect jumps: 1", StringComparison.Ordinal),
        true, "human-readable report counts unresolved transfers");
    Equal(graph.FunctionCandidates.All(f => f.Blocks.Contains(new FlowAddress(1, 5))), true,
        "shared return block may belong to multiple candidates");
    // P0: unknown executable bytes are ranked, not silently classified as code.
    var gapLedger = ExecutableGapAnalysis.Analyze(graph, graphImage,
        minimumGapBytes: 1, maximumGaps: 5);
    Equal(gapLedger.ExecutableObjectBytes, graphImage.Objects
        .Where(o => (o.Flags & 4u) != 0).Sum(o => (long)o.VirtualSize),
        "gap ledger covers the entire executable object");
    Equal(gapLedger.ExecutableObjectBytes,
        gapLedger.CandidateInstructionBytes + gapLedger.AnalystProtectedBytes +
        gapLedger.UnknownFileBackedBytes + gapLedger.UnbackedVirtualBytes,
        "gap classifications close exactly without invented byte ownership");
    Equal(gapLedger.UnknownFileBackedBytes > 0, true,
        "analyzer retains unknown file-backed bytes");
    int expectedShortSpans = graph.Ranges.Count(r =>
        r.Classification == "unknown-file-backed" &&
        (long)r.EndExclusive - r.Begin is >= 64 and <= 255);
    long expectedShortBytes = graph.Ranges.Where(r =>
        r.Classification == "unknown-file-backed" &&
        (long)r.EndExclusive - r.Begin is >= 64 and <= 255)
        .Sum(r => (long)r.EndExclusive - r.Begin);
    Equal(gapLedger.UnknownSpans64To255, expectedShortSpans,
        "short unknown span counter reconciles to classified source ranges");
    Equal(gapLedger.UnknownBytes64To255, expectedShortBytes,
        "short unknown byte total reconciles independently to source ranges");
    Equal(gapLedger.SpansBelowMinimumGap, 0,
        "minimum gap one cannot suppress any unknown file-backed span");
    var defaultGapLedger = ExecutableGapAnalysis.Analyze(graph, graphImage,
        minimumGapBytes: 256, maximumGaps: 5);
    Equal(defaultGapLedger.UnknownSpans64To255, expectedShortSpans,
        "default 256-byte cutoff retains short unknown summary statistics");
    Equal(defaultGapLedger.SpansBelowMinimumGap,
        graph.Ranges.Count(r => r.Classification == "unknown-file-backed" &&
            (long)r.EndExclusive - r.Begin < 256),
        "filtered unknown spans are counted, not silently removed");
    Equal(defaultGapLedger.CandidateInstructionBytes,
        gapLedger.CandidateInstructionBytes,
        "gap reporting threshold cannot change decoded code byte coverage");
    // A total byte sum can be forged without covering the correct
    // addresses: overlap one byte, omit another, and keep the same
    // aggregate length. Reject before publishing ranked gap evidence.
    var partitioned = ExecutableGapAnalysis.Analyze(
        graph with { Ranges = graph.Ranges.Reverse().ToArray() },
        graphImage, minimumGapBytes: 1, maximumGaps: 5);
    Equal(partitioned.UnknownFileBackedBytes, gapLedger.UnknownFileBackedBytes,
        "range serialization order cannot change executable unknown accounting");
    var wideUnknown = graph.Ranges.First(r =>
        r.Classification == "unknown-file-backed" &&
        (long)r.EndExclusive - r.Begin >= 8);
    uint midpoint = wideUnknown.Begin +
        (wideUnknown.EndExclusive - wideUnknown.Begin) / 2;
    var forgedRangeInventory = graph.Ranges
        .Where(r => r != wideUnknown)
        .Concat(new[] {
            wideUnknown with { EndExclusive = midpoint + 1 },
            wideUnknown with {
                Begin = midpoint, EndExclusive = wideUnknown.EndExclusive - 1
            }
        }).ToArray();
    Equal(forgedRangeInventory.Sum(r =>
        (long)r.EndExclusive - r.Begin),
        graph.Ranges.Sum(r => (long)r.EndExclusive - r.Begin),
        "forged overlap and hole deliberately conserve the byte-count total");
    bool forgedRangesDenied = false;
    try
    {
        ExecutableGapAnalysis.Analyze(graph with {
            Ranges = forgedRangeInventory
        }, graphImage, minimumGapBytes: 1);
    }
    catch (InvalidDataException) { forgedRangesDenied = true; }
    Equal(forgedRangesDenied, true,
        "matching aggregate totals never authorize overlapping or missing regions");

    Equal(gapLedger.Gaps.Any(g => g.Begin.Object == 1 &&
        g.FirstFileOffset == graphImage.ToFileOffset(g.Begin.Object, g.Begin.Offset)),
        true, "gaps retain original-source file offsets");
    Equal(gapLedger.Gaps.All(g => g.Classification == "unknown-file-backed"),
        true, "gap ranking never promotes unknown bytes to code");
    using (var gapReportJson = JsonDocument.Parse(AnalysisReportExport.BuildJson(
        graph, gaps: gapLedger)))
    {
        var gapSection = gapReportJson.RootElement.GetProperty("gapAnalysis");
        Equal(gapSection.GetProperty("SourceSha256").GetString(), graph.ArtifactSha256,
            "candidate report cannot detach gap ledger from original source");
        Equal(gapSection.GetProperty("UnknownFileBackedBytes").GetInt64(),
            gapLedger.UnknownFileBackedBytes, "report exposes full unknown byte total");
        Equal(gapSection.GetProperty("UnknownSpans64To255").GetInt32(),
            expectedShortSpans, "JSON exports disclose 64..255-byte unknown spans");
        Equal(gapSection.GetProperty("UnknownBytes64To255").GetInt64(),
            expectedShortBytes, "JSON exports disclose short unknown byte totals");
        Equal(gapSection.GetProperty("SpansBelowMinimumGap").GetInt32(),
            0, "JSON exports preserve explicit threshold-filter count");
    }
    Equal(AnalysisReportExport.BuildText(graph, gaps: gapLedger)
        .Contains("[EXECUTABLE OBJECT GAPS", StringComparison.Ordinal), true,
        "text report carries ranked unknown gap evidence");
    Equal(AnalysisReportExport.BuildText(graph, gaps: defaultGapLedger)
        .Contains("Short unknown spans (64..255 bytes): " +
            expectedShortSpans, StringComparison.Ordinal), true,
        "text report visibly discloses short spans omitted by default ranking");
    bool foreignLedgerDenied = false;
    try { AnalysisReportExport.BuildJson(graph, gaps: gapLedger with {
        SourceSha256 = new string('f', 64) }); }
    catch (InvalidDataException) { foreignLedgerDenied = true; }
    Equal(foreignLedgerDenied, true, "candidate report rejects gaps from another source");
    Equal(gapLedger.Gaps.All(g => g.Begin.Offset >= 10 || g.EndExclusive <= 6),
        true, "analyst-protected data never reappears as unknown gap");
    bool gapForeignSourceRejected = false;
    try { ExecutableGapAnalysis.Analyze(graph with {
        ArtifactSha256 = new string('0', 64) }, graphImage); }
    catch (InvalidDataException) { gapForeignSourceRejected = true; }
    Equal(gapForeignSourceRejected, true, "gap ledger rejects another original source identity");
    string gapDir = Path.Combine(Path.GetTempPath(), "dosdis-gaps-" + Guid.NewGuid().ToString("N"));
    Directory.CreateDirectory(gapDir);
    try
    {
        string gapPath = Path.Combine(gapDir, "gaps.json");
        ExecutableGapAnalysis.WriteNew(gapPath, gapLedger);
        Equal(File.ReadAllText(gapPath).Contains("dosdis-executable-gap-ledger",
            StringComparison.Ordinal), true, "exported gap ledger uses named schema");
        bool gapOverwriteDenied = false;
        try { ExecutableGapAnalysis.WriteNew(gapPath, gapLedger); }
        catch (IOException) { gapOverwriteDenied = true; }
        Equal(gapOverwriteDenied, true, "ranked gap exports cannot replace prior evidence");
    }
    finally { Directory.Delete(gapDir, recursive: true); }
    // An original-file-backed routine without an incoming candidate
    // edge must remain unknown until explicitly analyst-seeded. The
    // default long-gap ranking may hide it, but the short-span summary
    // and the --min-gap-bytes override must reveal its existence.
    // This is a generic synthetic LE code-island test, not a copy of
    // any game's instruction sequence.
    var islandBytes = MakeFixture(false);
    Dword(islandBytes, 0x80 + 0xc4, 0x100);
    Dword(islandBytes, 0x80 + 0x2c, 0x100);
    new byte[] {
        0xB8, 0x01, 0, 0, 0,       // 1:0040 MOV EAX,1
        0xE8, 0x26, 0, 0, 0,       // 1:0045 CALL 1:0070
        0xC3                         // 1:004A RET
    }.CopyTo(islandBytes, 0x200 + 0x40);
    islandBytes[0x200 + 0x70] = 0xC3;
    var islandImage = LeImage.Load(BinaryArtifact.FromBytes(islandBytes));
    var unseededIsland = ControlFlowAnalysis.Analyze(islandImage);
    Equal(unseededIsland.Instructions.Any(i =>
        i.Address == new FlowAddress(1, 0x40)), false,
        "orphan LE routine is not promoted by scanning unknown bytes");
    var hiddenIsland = ExecutableGapAnalysis.Analyze(
        unseededIsland, islandImage, minimumGapBytes: 256);
    Equal(hiddenIsland.Gaps.Count, 0,
        "default 256-byte minimum does not rank the short unknown island");
    Equal(hiddenIsland.UnknownSpans64To255 > 0, true,
        "short unknown spans remain visible in default report statistics");
    Equal(hiddenIsland.UnknownBytes64To255 >= 0x4B, true,
        "short unknown byte totals include the complete unseeded routine");
    var visibleIsland = ExecutableGapAnalysis.Analyze(
        unseededIsland, islandImage, minimumGapBytes: 64);
    Equal(visibleIsland.Gaps.Any(g => g.Begin.Offset <= 0x40 &&
        g.EndExclusive > 0x4A), true,
        "optional 64-byte minimum ranks the otherwise hidden code island");

    var seededIsland = ControlFlowAnalysis.Analyze(islandImage, seeds: new[]
    {
        new FlowAddress(1, 0), new FlowAddress(1, 0x40)
    });
    Equal(seededIsland.Instructions.Any(i =>
        i.Address == new FlowAddress(1, 0x45) &&
        i.OriginalBytes == "E826000000"), true,
        "explicit source-backed LE island seed decodes its real E8 bytes");
    Equal(seededIsland.Edges.Any(e => e.Kind == "call" &&
        e.Source == new FlowAddress(1, 0x45) &&
        e.Destination == new FlowAddress(1, 0x70)), true,
        "orphan-island near call destination is independently decoded");
    Equal(seededIsland.Seeds.Count, 2,
        "analyst seed remains a visible distinct code-discovery hypothesis");
    Equal(seededIsland.Instructions.Any(i =>
        i.Address == new FlowAddress(1, 0x40) &&
        i.Evidence.Contains("candidate")), true,
        "explicitly seeded orphan is still candidate, not proven runtime code");
    Console.WriteLine("PASS: ranked unknown executable gap accounting and source-backed evidence");
    Equal(graph.Ranges.Any(r => r.Begin <= 6 && r.EndExclusive >= 10 && r.Classification == "protected-data"), true,
        "analyst protected literal preserved");
    Equal(graph.Ranges.Any(r => r.Begin <= 10 && r.EndExclusive >= 0x10 && r.Classification == "unknown-file-backed"), true,
        "unvisited bytes remain unknown, not guessed data or code");
    Equal(graph.Instructions.All(x => x.Evidence == "candidate-reachable-from-seed"), true,
        "reachability isn't promoted to proof");
    Equal(graph.ProtectedData[0].Provenance, "synthetic-known-data", "protected range provenance retained");
    bool invalidRangeRejected = false;
    try { ControlFlowAnalysis.Analyze(graphImage, protectedData: new[] {
        new ProtectedDataRange(1, 31, 35) }); }
    catch (ArgumentException) { invalidRangeRejected = true; }
    Equal(invalidRangeRejected, true, "protected data out of object bounds rejected");
    var overlapTrace = new AnalysisDecisionTrace();
    var overlapping = ControlFlowAnalysis.Analyze(graphImage, new[] {
        new FlowAddress(1, 0), new FlowAddress(1, 2) }, decisionTrace: overlapTrace);
    Equal(overlapTrace.Snapshot().Any(x => x.Outcome == AnalysisDecisionOutcome.MidInstructionRejected),
        true, "mid-instruction conflict has typed reason");
    Equal(overlapping.Diagnostics.Any(d => d.Code == "mid-instruction-target"), true,
        "mid-instruction overlapping seed reported");
    var overlapDetailGraph = ControlFlowAnalysis.Analyze(graphImage, new[] {
        new FlowAddress(1, 2), new FlowAddress(1, 0) });
    Equal(overlapDetailGraph.Diagnostics.Any(d => d.Code == "overlapping-interpretation" &&
        d.Conflict is { ProposedAssembly.Length: > 0, RetainedAssembly: not null,
            Disposition: "candidate-rejected", RemainsUnresolved: true }),
        true, "overlap diagnostic retains both interpretations and rejected status");
    Equal(overlapping.Instructions.Any(x => x.Address.Offset == 2), false,
        "mid-instruction seed not emitted as verified instruction");
    var protectedTrace = new AnalysisDecisionTrace();
    var blocked = ControlFlowAnalysis.Analyze(graphImage, new[] { new FlowAddress(1, 0x10) },
        new[] { new ProtectedDataRange(1, 5, 6) }, decisionTrace: protectedTrace);
    Equal(protectedTrace.Snapshot().Any(x => x.Outcome == AnalysisDecisionOutcome.ProtectedTargetRejected &&
        x.Address == new FlowAddress(1, 5)), true, "protected target rejection traced");
    Equal(blocked.Diagnostics.Any(d => d.Code == "protected-data-target"), true,
        "branch targeting protected data is halted");
    Equal(blocked.Instructions.Any(x => x.Address.Offset == 5), false,
        "protected byte not decoded");
    var budgetTrace = new AnalysisDecisionTrace();
    var limited = ControlFlowAnalysis.Analyze(graphImage, maxInstructions: 1, decisionTrace: budgetTrace);
    Equal(budgetTrace.Snapshot().Any(x => x.Outcome == AnalysisDecisionOutcome.BudgetExceeded),
        true, "instruction budget stop is inspectable");
    using (var budgetJson = JsonDocument.Parse(budgetTrace.BuildJson()))
    {
        Equal(budgetJson.RootElement.GetProperty("analysisCompleted").GetBoolean(), true,
            "budget-stopped graph successfully returned despite truncation");
        Equal(budgetJson.RootElement.GetProperty("analysisTruncated").GetBoolean(), true,
            "instruction budget truncation is recorded in trace metadata");
    }
    // The public replay orchestrator must not enumerate a one-shot
    // caller input more than once, including the final traced pass.
    // Oversized duplicate rows must fail before consuming the trace.
    int seedEnumerations = 0;
    int rangeEnumerations = 0;
    IEnumerable<FlowAddress> OneShotLeSeeds()
    {
        seedEnumerations++;
        if (seedEnumerations != 1)
            throw new InvalidOperationException("source seeds were re-enumerated");
        yield return new FlowAddress(1, 0);
    }
    IEnumerable<ProtectedDataRange> OneShotLeRanges()
    {
        rangeEnumerations++;
        if (rangeEnumerations != 1)
            throw new InvalidOperationException("protected ranges were re-enumerated");
        yield return new ProtectedDataRange(1, 6, 10, "synthetic-known-data");
    }
    var oneShotTrace = new AnalysisDecisionTrace();
    var oneShotReport = ControlFlowAnalysis.Analyze(graphImage,
        seeds: OneShotLeSeeds(), protectedData: OneShotLeRanges(),
        decisionTrace: oneShotTrace);
    Equal(seedEnumerations, 1,
        "replayed traced analysis enumerates original seed input only once");
    Equal(rangeEnumerations, 1,
        "replayed traced analysis enumerates protection input only once");
    Equal(oneShotReport.ArtifactSha256, graph.ArtifactSha256,
        "one-shot seed source and final traced report agree");

    var oversizedSeedTrace = new AnalysisDecisionTrace();
    bool oversizedSeedDenied = false;
    try
    {
        ControlFlowAnalysis.Analyze(graphImage,
            seeds: Enumerable.Repeat(new FlowAddress(1, 0), 4097),
            decisionTrace: oversizedSeedTrace);
    }
    catch (ArgumentOutOfRangeException) { oversizedSeedDenied = true; }
    Equal(oversizedSeedDenied, true,
        "4097 duplicate supplied LE seed rows cannot bypass input budget");
    Equal(oversizedSeedTrace.ArtifactSha256 is null, true,
        "input cardinality failure does not consume one-shot decision trace");

    var oversizedRangeTrace = new AnalysisDecisionTrace();
    bool oversizedRangesDenied = false;
    try
    {
        ControlFlowAnalysis.Analyze(graphImage,
            protectedData: Enumerable.Repeat(
                new ProtectedDataRange(1, 6, 10), 100001),
            decisionTrace: oversizedRangeTrace);
    }
    catch (ArgumentOutOfRangeException) { oversizedRangesDenied = true; }
    Equal(oversizedRangesDenied, true,
        "100001 duplicate protection rows cannot bypass input budget");
    Equal(oversizedRangeTrace.ArtifactSha256 is null, true,
        "oversized range input preserves unused trace for a valid retry");
    var invalidSeedTrace = new AnalysisDecisionTrace();
    bool invalidSeed = false;
    try
    {
        ControlFlowAnalysis.Analyze(graphImage, seeds: new[] { new FlowAddress(1, 0x100) },
            decisionTrace: invalidSeedTrace);
    }
    catch (ArgumentException) { invalidSeed = true; }
    Equal(invalidSeed, true, "out-of-object seed is rejected");
    Equal(invalidSeedTrace.ArtifactSha256 is null, true,
        "rejected seed does not consume the single-use trace");
    using (var rejectedJson = JsonDocument.Parse(invalidSeedTrace.BuildJson()))
        Equal(rejectedJson.RootElement.GetProperty("analysisCompleted").GetBoolean(),
            false, "rejected analysis is never called completed");
    var recoveredTraceGraph = ControlFlowAnalysis.Analyze(graphImage,
        decisionTrace: invalidSeedTrace);
    Equal(recoveredTraceGraph.Instructions.Count, graph.Instructions.Count,
        "trace remains usable after rejected input");

    // Object offset 0x30 is either outside the 0x20-byte executable region,
    // or virtual-but-unbacked when the object is extended to 0x40 bytes.
    var outOfScopeBytes = (byte[])graphBytes.Clone();
    outOfScopeBytes[dataStart + 0x10] = 0xEB; // jmp short +0x1e -> 0x30
    outOfScopeBytes[dataStart + 0x11] = 0x1E;
    var nonExecutableImage = LeImage.Load(BinaryArtifact.FromBytes(outOfScopeBytes));
    var nonExecutableTrace = new AnalysisDecisionTrace();
    ControlFlowAnalysis.Analyze(nonExecutableImage, decisionTrace: nonExecutableTrace);
    Equal(nonExecutableTrace.Snapshot().Any(x =>
        x.Outcome == AnalysisDecisionOutcome.NonExecutableTargetRejected &&
        x.Destination == new FlowAddress(1, 0x30)), true,
        "out-of-executable target is not mislabeled as unbacked virtual memory");
    Dword(outOfScopeBytes, 0x80 + 0xc4, 0x40);
    var unbackedImage = LeImage.Load(BinaryArtifact.FromBytes(outOfScopeBytes));
    var unbackedTrace = new AnalysisDecisionTrace();
    ControlFlowAnalysis.Analyze(unbackedImage, decisionTrace: unbackedTrace);
    Equal(unbackedTrace.Snapshot().Any(x =>
        x.Outcome == AnalysisDecisionOutcome.UnbackedFlowTargetRejected &&
        x.Destination == new FlowAddress(1, 0x30)), true,
        "virtual-but-unbacked target has distinct evidence");
    Equal(unbackedTrace.Snapshot().Any(x =>
        x.Outcome == AnalysisDecisionOutcome.UnbackedFlowTargetRejected &&
        x.DestinationFileOffset is null), true,
        "unbacked virtual destination has no invented file offset");
    Equal(limited.Truncated, true, "analysis budget truncation is disclosed");
    Equal(limited.Instructions.Count, 1, "analysis respects budget");
    // P0: a range-guarded switch can contain literal dword targets without
    // LE fixups. Require the exact CMP reg,6; JA default; indexed JMP
    // chain and seven fully backed executable destinations.
    var boundedBytes = MakeFixture(false);
    Dword(boundedBytes, 0x80 + 0xc4, 0x80);
    Dword(boundedBytes, 0x80 + 0x2c, 0x80);
    new byte[] {
        0x83, 0xF8, 0x06,             // cmp eax,6
        0x77, 0x07,                   // ja default at +0x0C
        0xFF, 0x24, 0x85, 0x40, 0x00, 0x01, 0x00, // jmp [eax*4+0x10040]
        0xC3                          // default ret
    }.CopyTo(boundedBytes, 0x200);
    for (int i = 0; i < 7; i++)
    {
        uint target = (uint)(0x20 + i * 4);
        boundedBytes[0x200 + (int)target] = 0xC3;
        Dword(boundedBytes, 0x200 + 0x40 + i * 4, 0x10000 + target);
    }
    var boundedImage = LeImage.Load(BinaryArtifact.FromBytes(boundedBytes));
    var boundedGraph = ControlFlowAnalysis.Analyze(boundedImage);
    Equal(boundedGraph.JumpTables.Count, 1, "unsigned guarded literal table found");
    Equal(boundedGraph.JumpTables[0].CandidateTargets.Count, 7,
        "CMP eax,6 bounds seven possible entries");
    Equal(boundedGraph.JumpTables[0].DispatchKind, "indirect-jump-table",
        "guarded jump tables retain dispatch type");
    Equal(boundedGraph.JumpTables[0].Evidence.Contains("no-relocation-proof"),
        true, "unrelocated table remains a candidate");
    Equal(boundedGraph.Edges.Count(e => e.Kind == "candidate-indirect-jump"), 7,
        "seven candidate jump targets retained");
    Equal(boundedGraph.Edges.Any(e => e.Kind == "unresolved-indirect-jump"),
        true, "unproven runtime control flow remains unresolved");
    Equal(boundedGraph.Instructions.Count, 11,
        "default and seven literal case targets are discovered without a gap sweep");
    // A separately seeded direct indexed transfer bypasses the
    // upstream CMP/JA, even though the decoder discovers both byte
    // sequences from the same immutable LE source.
    foreach (bool bypassFirst in new[] { false, true })
    {
        FlowAddress directJump = new(1, 5);
        var seeds = bypassFirst
            ? new[] { directJump, new FlowAddress(1, 0) }
            : new[] { new FlowAddress(1, 0), directJump };
        var unguardedSeedGraph = ControlFlowAnalysis.Analyze(
            boundedImage, seeds: seeds);
        Equal(unguardedSeedGraph.JumpTables.Count, 0,
            "explicit direct-JMP entry never borrows unsigned JA index proof");
    }
    var nonguardedBytes = (byte[])boundedBytes.Clone();
    nonguardedBytes[0x200 + 3] = 0x75; // jne != unsigned JA
    Equal(ControlFlowAnalysis.Analyze(LeImage.Load(BinaryArtifact.FromBytes(nonguardedBytes)))
        .JumpTables.Count, 0, "other conditional tests cannot establish unsigned table bounds");
    var invalidPointerBytes = (byte[])boundedBytes.Clone();
    Dword(invalidPointerBytes, 0x200 + 0x40 + 3 * 4, 0xDEADBEEF);
    Equal(ControlFlowAnalysis.Analyze(LeImage.Load(BinaryArtifact.FromBytes(invalidPointerBytes)))
        .JumpTables.Count, 0, "one invalid literal target rejects the entire table");
    var objectOffsetBytes = (byte[])boundedBytes.Clone();
    Dword(objectOffsetBytes, 0x200 + 0x40, 0x20);
    Equal(ControlFlowAnalysis.Analyze(LeImage.Load(BinaryArtifact.FromBytes(objectOffsetBytes)))
        .JumpTables.Count, 0, "object-relative-looking literal is not assumed runtime linear");
    // Every four-byte fixup origin in [-3,+3] overlaps the first
    // nominally literal DWORD, even if it does not start on the
    // aligned source pointer. Neither an adjacent write to preceding
    // bytes nor an interior fixup can be mistaken for stable storage.
    for (int delta = -3; delta <= 3; delta++)
    {
        var overlappingLiteralFixup = (byte[])boundedBytes.Clone();
        Dword(overlappingLiteralFixup, 0x80 + 0x68, 0xE0);
        Dword(overlappingLiteralFixup, 0x80 + 0x6C, 0xE8);
        Dword(overlappingLiteralFixup, 0x80 + 0xE4, 7);
        new byte[] {
            0x07, 0x00, (byte)(0x40 + delta), 0x00,
            0x01, 0x20, 0x00
        }.CopyTo(overlappingLiteralFixup, 0x80 + 0xE8);
        var overlapLiteralGraph = ControlFlowAnalysis.Analyze(
            LeImage.Load(BinaryArtifact.FromBytes(overlappingLiteralFixup)));
        Equal(overlapLiteralGraph.JumpTables.Count, 0,
            "overlapping literal DWORD fixup delta=" + delta);
        Equal(overlapLiteralGraph.Edges.Any(e =>
            e.Kind == "unresolved-indirect-jump"), true,
            "overlapping literal relocation retains runtime uncertainty");
    }

    Console.WriteLine("PASS: guarded unsigned literal jump-table candidates and negative controls");

    // Generic LE/x86-32 guarded byte switch:
    // SUB AL,2; CMP AL,9; JA default; MOVZX EAX,AL;
    // JMP DWORD PTR CS:[EAX*4+0040h].
    // This deliberately models *no* Strahd addresses or game-specific
    // pointers. CMP overrides the SUB flags, so the post-subtraction
    // value alone is bounded (0..9) by unsigned JA and zero extension.
    var byteSwitchBytes = MakeFixture(false);
    Dword(byteSwitchBytes, 0x80 + 0xc4, 0x100);
    Dword(byteSwitchBytes, 0x80 + 0x2c, 0x100);
    new byte[] {
        0x2C, 0x02,       // SUB AL,2: 8-bit wrap is retained as context
        0x3C, 0x09,       // CMP AL,9: compare post-subtraction byte
        0x77, 0x0B,       // JA default RET at offset 0x11
        0x0F, 0xB6, 0xC0, // MOVZX EAX,AL: exact 8-to-32 zero extension
        0x2E, 0xFF, 0x24, 0x85, 0x40, 0, 0, 0, // JMP CS:[EAX*4+40h]
        0xC3              // default RET
    }.CopyTo(byteSwitchBytes, 0x200);
    for (int entry = 0; entry < 10; entry++)
    {
        Dword(byteSwitchBytes, 0x200 + 0x40 + entry * 4,
            (uint)(0x80 + entry));
        byteSwitchBytes[0x200 + 0x80 + entry] = 0xC3;
    }
    var byteSwitchGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(byteSwitchBytes)));
    // The source-byte oracle prevents a seemingly passing pattern
    // fixture from silently decoding a different opcode or crossing
    // a prefix/branch boundary after future refactors.
    foreach (var exact in new[] {
        (Offset: 0u, Bytes: "2C02"),
        (Offset: 2u, Bytes: "3C09"),
        (Offset: 4u, Bytes: "770B"),
        (Offset: 6u, Bytes: "0FB6C0"),
        (Offset: 9u, Bytes: "2EFF248540000000")
    })
    {
        Equal(byteSwitchGraph.Instructions.Single(insn =>
            insn.Address == new FlowAddress(1, exact.Offset)).OriginalBytes,
            exact.Bytes,
            "guarded byte switch keeps original instruction at " +
                exact.Offset.ToString("X"));
    }
    Equal(byteSwitchGraph.JumpTables.Count, 1,
        "unsigned CMP AL/JA/MOVZX EAX guarded LE table is recovered");
    Equal(byteSwitchGraph.JumpTables[0].CandidateTargets.Count, 10,
        "post-8-bit-subtraction bound selects exactly ten candidate pointers");
    Equal(byteSwitchGraph.JumpTables[0].CandidateTargets[0],
        new FlowAddress(1, 0x80),
        "first generic guarded candidate remains object relative");
    Equal(byteSwitchGraph.JumpTables[0].CandidateTargets[9],
        new FlowAddress(1, 0x89),
        "last generic guarded candidate remains object relative");
    Equal(byteSwitchGraph.JumpTables[0].Evidence.Contains(
        "byte-register-MOVZX-zeroextend"), true,
        "zero extension proof is separately labeled in table evidence");
    Equal(byteSwitchGraph.JumpTables[0].Evidence.Contains(
        "unsigned-ja-bound"), true,
        "literal table evidence preserves source-backed unsigned JA count");
    Equal(byteSwitchGraph.Edges.Count(e => e.Kind == "candidate-indirect-jump"), 10,
        "ten candidate cases retained without asserting runtime execution");
    Equal(byteSwitchGraph.Edges.Any(e =>
        e.Kind == "unresolved-indirect-jump"), true,
        "guarded switch keeps the unresolved runtime indirect jump");
    Equal(byteSwitchGraph.Edges.Any(e =>
        e.Kind == "branch-taken" &&
        e.Source == new FlowAddress(1, 4) &&
        e.Destination == new FlowAddress(1, 0x11)), true,
        "unsigned guard retains its original out-of-range default branch");

    // An independently seeded MOVZX or indirect-jump site is a
    // competing entry hypothesis, not evidence that the predecessor
    // JA dominates its execution. Both must remain unbounded in this
    // non-dominance-aware candidate analyzer.
    var byteSwitchImage = LeImage.Load(
        BinaryArtifact.FromBytes(byteSwitchBytes));
    var bypassExtension = ControlFlowAnalysis.Analyze(byteSwitchImage,
        seeds: new[] { new FlowAddress(1, 0), new FlowAddress(1, 6) });
    Equal(bypassExtension.JumpTables.Any(t => t.Evidence.Contains(
        "byte-register-MOVZX-zeroextend")), false,
        "independent MOVZX entry seed cannot inherit another path's JA bound");
    var bypassIndexedJump = ControlFlowAnalysis.Analyze(byteSwitchImage,
        seeds: new[] { new FlowAddress(1, 0), new FlowAddress(1, 9) });
    Equal(bypassIndexedJump.JumpTables.Any(t => t.Evidence.Contains(
        "byte-register-MOVZX-zeroextend")), false,
        "independent indexed-JMP seed cannot borrow upstream unsigned guard");

    // A near-form unsigned JA must have exactly the same semantics as
    // its short encoding. Exercise the six-byte 0F87 rel32 guard, not
    // just a mnemonic decoded from a two-byte short conditional.
    var nearJaSwitch = (byte[])byteSwitchBytes.Clone();
    new byte[] {
        0x0F, 0x87, 0x0B, 0, 0, 0, // JA near +11, default at 1:0x15
        0x0F, 0xB6, 0xC0,          // MOVZX EAX,AL
        0x2E, 0xFF, 0x24, 0x85, 0x40, 0, 0, 0,
        0xC3
    }.CopyTo(nearJaSwitch, 0x200 + 4);
    var nearJaGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(nearJaSwitch)));
    Equal(nearJaGraph.Instructions.Single(insn =>
        insn.Address == new FlowAddress(1, 4)).OriginalBytes,
        "0F870B000000",
        "six-byte JA rel32 must be decoded from exact original source");
    Equal(nearJaGraph.Instructions.Single(insn =>
        insn.Address == new FlowAddress(1, 0x0D)).OriginalBytes,
        "2EFF248540000000",
        "long-form switch changes only control-flow offsets, not indexed operand");
    Equal(nearJaGraph.JumpTables.Count, 1,
        "near JA/CMP byte/MOVZX must preserve the same ten-way guard");
    Equal(nearJaGraph.JumpTables[0].CandidateTargets.Count, 10,
        "near JA proof never depends on short-branch encoding width");
    Equal(nearJaGraph.Edges.Any(e =>
        e.Kind == "branch-taken" &&
        e.Source == new FlowAddress(1, 4) &&
        e.Destination == new FlowAddress(1, 0x15)), true,
        "near JA retains exact default destination under changed code width");

    // The long-form conditional is subject to the same forward-skip
    // proof as JA rel8. A rel32 that lands on the indexed transfer
    // cannot provide an out-of-range default path.
    var nearJaBypass = (byte[])nearJaSwitch.Clone();
    nearJaBypass[0x200 + 6] = 0x03; // next IP=0x0A, JMP begins at 0x0D
    var nearBypassGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(nearJaBypass)));
    Equal(nearBypassGraph.JumpTables.Count, 0,
        "near JA rel32 into dispatch cannot license unsigned index bounds");

    var nearSignedGuard = (byte[])nearJaSwitch.Clone();
    nearSignedGuard[0x200 + 5] = 0x8F; // 0F 8F = JG, not unsigned JA
    var nearSignedGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(nearSignedGuard)));
    Equal(nearSignedGraph.JumpTables.Count, 0,
        "near signed JG cannot satisfy unsigned guard proof");

    // Entrypoint hypotheses are unordered: neither an explicit MOVZX
    // seed nor an explicit indexed-JMP seed may inherit the guard in
    // either traversal order. This is independent of table contents.
    foreach (uint bypass in new uint[] { 6, 9 })
        foreach (bool extraFirst in new[] { false, true })
        {
            FlowAddress extra = new(1, bypass);
            var alternativeSeeds = extraFirst
                ? new[] { extra, new FlowAddress(1, 0) }
                : new[] { new FlowAddress(1, 0), extra };
            var withAlternative = ControlFlowAnalysis.Analyze(
                byteSwitchImage, seeds: alternativeSeeds);
            Equal(withAlternative.JumpTables.Any(t =>
                t.Evidence.Contains("byte-register-MOVZX-zeroextend")), false,
                "alternative seeded x86-32 entry cannot gain unsigned JA proof");
        }

    // A separately entered JA uses flags from an unknown caller:
    // CMP AL,9 does not dominate the guard and cannot bound the
    // indirect transfer, even if its original fallthrough edge exists.
    foreach (bool guardFirst in new[] { false, true })
    {
        var guardSeeds = guardFirst
            ? new[] { new FlowAddress(1, 4), new FlowAddress(1, 0) }
            : new[] { new FlowAddress(1, 0), new FlowAddress(1, 4) };
        var independentGuard = ControlFlowAnalysis.Analyze(
            byteSwitchImage, seeds: guardSeeds);
        Equal(independentGuard.JumpTables.Count, 0,
            "separately seeded unsigned JA cannot inherit CMP flags");
    }

    // Out-of-range JA must really escape *past* the indexed transfer.
    // Jumping into MOVZX, directly into JMP, or backwards allows an
    // uncontrolled index along the nominally out-of-range path.
    foreach (var badGuard in new[] {
        (Displacement: (byte)0x00, Label: "JA-to-MOVZX"),
        (Displacement: (byte)0x03, Label: "JA-to-indirect-JMP"),
        (Displacement: (byte)0xFC, Label: "JA-backwards-to-CMP")
    })
    {
        var mutated = (byte[])byteSwitchBytes.Clone();
        mutated[0x200 + 5] = badGuard.Displacement;
        var unsafeGraph = ControlFlowAnalysis.Analyze(
            LeImage.Load(BinaryArtifact.FromBytes(mutated)));
        Equal(unsafeGraph.JumpTables.Count, 0,
            badGuard.Label + " cannot prove a bounded indirect index");
    }

    // Exhaustive local displacement sweep: all short-JA targets from
    // MOVZX through the last byte of the indexed JMP are bypasses.
    // A 'successful' table recovered from any of those would be a
    // false upper-bound proof, irrespective of pointer plausibility.
    for (byte displacement = 0; displacement < 11; displacement++)
    {
        var insideDispatch = (byte[])byteSwitchBytes.Clone();
        insideDispatch[0x200 + 5] = displacement;
        var blocked = ControlFlowAnalysis.Analyze(
            LeImage.Load(BinaryArtifact.FromBytes(insideDispatch)));
        Equal(blocked.JumpTables.Any(table =>
            table.Evidence.Contains("unsigned-ja-bound") ||
            table.Evidence.Contains(
                "index-bounded-by-source-backed-unsigned-ja")), false,
            "JA bypass into dispatch byte offset +" + displacement +
            " cannot authorize a bounded table");
    }

    // Late-discovered CFG ingress: the out-of-range default initially
    // passes the indirect JMP, then a NOP and short backwards JMP add
    // a second path into that indexed transfer AFTER provisional table
    // discovery. The final result must be RECOMPUTED without the
    // unproven table: no phantom candidate edges or decoded case code.
    foreach (var (target, rel8) in new[] {
        (Target: 9u, Rel8: (byte)0xF5), // 0x12 EB F5 -> indexed JMP at 0x09
        (Target: 6u, Rel8: (byte)0xF2), // 0x12 EB F2 -> MOVZX at 0x06
        (Target: 4u, Rel8: (byte)0xF0), // 0x12 EB F0 -> unsigned JA at 0x04
        (Target: 3u, Rel8: (byte)0xEF), // alternate byte inside CMP
        (Target: 5u, Rel8: (byte)0xF1), // alternate byte inside JA
        (Target: 7u, Rel8: (byte)0xF3), // alternate byte inside MOVZX
        (Target: 10u, Rel8: (byte)0xF6) // alternate byte inside indexed JMP
    })
    {
        var lateBypass = (byte[])byteSwitchBytes.Clone();
        lateBypass[0x200 + 0x11] = 0x90; // default NOP: explored after MOVZX
        lateBypass[0x200 + 0x12] = 0xEB; // JMP rel8: explored after indexed JMP
        lateBypass[0x200 + 0x13] = rel8;
        var lateImage = LeImage.Load(BinaryArtifact.FromBytes(lateBypass));
        var lateTrace = new AnalysisDecisionTrace(capacity: 64);
        var lateGraph = ControlFlowAnalysis.Analyze(lateImage,
            decisionTrace: lateTrace);
        Equal(lateGraph.JumpTables.Count, 0,
            "late indirect-switch bypass must retract literal table evidence");
        Equal(lateGraph.Edges.Any(e => e.Kind == "jump" &&
            e.Source == new FlowAddress(1, 0x12) &&
            e.Destination == new FlowAddress(1, target)), true,
            "late alternate branch is decoded from original LE source");
        Equal(lateGraph.Edges.Count(e =>
            e.Kind == "candidate-indirect-jump"), 0,
            "replayed CFG cannot retain phantom guarded case edges");
        Equal(lateGraph.Instructions.Any(i =>
            i.Address == new FlowAddress(1, 0x80)), false,
            "late invalidation must also retract speculative case decoding");
        Equal(lateTrace.Snapshot().Any(e =>
            e.Outcome == AnalysisDecisionOutcome.PointerTableCandidate), false,
            "final decision trace must not expose superseded provisional tables");
    }

    // A backedge entering CMP AL itself is different: CMP resets
    // flags on every path before JA is executed, so the guard remains
    // valid. Rejecting *all* loops or late edges would be imprecise.
    var reenterCompare = (byte[])byteSwitchBytes.Clone();
    reenterCompare[0x200 + 0x11] = 0x90;
    reenterCompare[0x200 + 0x12] = 0xEB;
    reenterCompare[0x200 + 0x13] = 0xEE; // 0x14-0x12 -> CMP at 0x02
    var compareLoop = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(reenterCompare)));
    Equal(compareLoop.JumpTables.Count, 1,
        "return to CMP preserves the independent unsigned guard");
    Equal(compareLoop.JumpTables[0].CandidateTargets.Count, 10,
        "loop back to CMP must not arbitrarily reduce finite table entries");

    // Resource exhaustion AFTER provisional table discovery must
    // withdraw any bounded evidence, not return partial pointer
    // targets whose alternate CFG paths were not all examined.
    var budgetLimited = ControlFlowAnalysis.Analyze(
        byteSwitchImage, maxInstructions: 6);
    Equal(budgetLimited.JumpTables.Count, 0,
        "partial worklist cannot publish a guarded literal switch table");
    Equal(budgetLimited.Edges.Any(e =>
        e.Kind == "candidate-indirect-jump"), false,
        "limited worklist replay must not leave provisional pointer edges");

    // The indexed JMP at object 1:0009 encodes its DWORD displacement
    // at 1:000D. A competing relocation starting in the immediate,
    // or as many as three bytes before its start, invalidates
    // interpretation of the unrelocated literal table base. This
    // negative test must not be confused with fixups *on* the table.
    for (int delta = -3; delta <= 3; delta++)
    {
        if (delta == 0) continue;
        var displacementOverlap = (byte[])byteSwitchBytes.Clone();
        Dword(displacementOverlap, 0x80 + 0x68, 0xE0);
        Dword(displacementOverlap, 0x80 + 0x6C, 0xE8);
        Dword(displacementOverlap, 0x80 + 0xE4, 7);
        new byte[] {
            0x07, 0x00, (byte)(0x0D + delta), 0x00,
            0x01, 0x40, 0x00
        }.CopyTo(displacementOverlap, 0x80 + 0xE8);
        var overlapCodeGraph = ControlFlowAnalysis.Analyze(
            LeImage.Load(BinaryArtifact.FromBytes(displacementOverlap)));
        Equal(overlapCodeGraph.JumpTables.Count, 0,
            "overlapping JMP displacement fixup delta=" + delta +
            " cannot reuse original unrelocated CS displacement");
        Equal(overlapCodeGraph.Edges.Any(e =>
            e.Kind == "unresolved-indirect-jump"), true,
            "ambiguous relocated JMP displacement retains runtime unknown");
    }

    var exactDisplacementFixup = (byte[])byteSwitchBytes.Clone();
    Dword(exactDisplacementFixup, 0x80 + 0x68, 0xE0);
    Dword(exactDisplacementFixup, 0x80 + 0x6C, 0xE8);
    Dword(exactDisplacementFixup, 0x80 + 0xE4, 7);
    new byte[] { 0x07, 0, 0x0D, 0, 1, 0x40, 0 }
        .CopyTo(exactDisplacementFixup, 0x80 + 0xE8);
    var exactDisplacementGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(exactDisplacementFixup)));
    Equal(exactDisplacementGraph.JumpTables.Count, 1,
        "an unambiguous exact operand relocation preserves CS indexed table");
    Equal(exactDisplacementGraph.JumpTables[0].CandidateTargets.Count, 10,
        "exact relocated CS base still requires every ten source-backed slots");

    var relativeDisplacementFixup = (byte[])exactDisplacementFixup.Clone();
    relativeDisplacementFixup[0x80 + 0xE8] = 0x08;
    var relativeDisplacementGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(relativeDisplacementFixup)));
    Equal(relativeDisplacementGraph.JumpTables.Count, 0,
        "exact LE relative fixup cannot be treated as an absolute CS table base");
    Equal(relativeDisplacementGraph.Edges.Any(e =>
        e.Kind == "unresolved-indirect-jump"), true,
        "relative effective address remains runtime-unproven");

    // A loader relocation rewriting any byte of CMP/JA/MOVZX or
    // the jump opcode invalidates a source-byte guard, even when
    // original-file decoding still shows the expected instruction.
    // The exact terminal disp32 relocation is the one allowed
    // exception and has an independent positive control above.
    foreach (uint sourceOffset in new uint[] {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
    })
    {
        var mutatedInstructionFixup = (byte[])byteSwitchBytes.Clone();
        Dword(mutatedInstructionFixup, 0x80 + 0x68, 0xE0);
        Dword(mutatedInstructionFixup, 0x80 + 0x6C, 0xE8);
        Dword(mutatedInstructionFixup, 0x80 + 0xE4, 7);
        new byte[] {
            0x07, 0, (byte)sourceOffset, 0, 1, 0x40, 0
        }.CopyTo(mutatedInstructionFixup, 0x80 + 0xE8);
        var relocatedInstructionGraph = ControlFlowAnalysis.Analyze(
            LeImage.Load(BinaryArtifact.FromBytes(mutatedInstructionFixup)));
        Equal(relocatedInstructionGraph.JumpTables.Count, 0,
            "LE fixup rewriting guarded instruction source at 0x" +
            sourceOffset.ToString("X2") + " cannot prove unsigned index");
        Equal(relocatedInstructionGraph.Edges.Any(e =>
            e.Kind == "unresolved-indirect-jump"), true,
            "rewritten LE compare/branch/index retains runtime unknown");
        Equal(relocatedInstructionGraph.Edges.Any(e =>
            e.Kind == "candidate-indirect-jump"), false,
            "relocation-mutated guard cannot retain speculative literal pointers");
    }
    var relocatedExactCodeGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(exactDisplacementFixup)),
        relocated: true);
    Equal(relocatedExactCodeGraph.JumpTables.Count, 1,
        "isolated terminal disp32 fixup works when decoding relocated view");
    Equal(relocatedExactCodeGraph.JumpTables[0].CandidateTargets.Count, 10,
        "relocated decoder retains ten independently source-backed cases");

    // A four-byte fixup starting exactly AFTER the complete indexed
    // jump does not alter any bytes used by the compare/guard/MOVZX or
    // displacement proof. Do not over-reject unrelated loader writes.
    var afterInstructionFixup = (byte[])byteSwitchBytes.Clone();
    Dword(afterInstructionFixup, 0x80 + 0x68, 0xE0);
    Dword(afterInstructionFixup, 0x80 + 0x6C, 0xE8);
    Dword(afterInstructionFixup, 0x80 + 0xE4, 7);
    new byte[] { 0x07, 0, 0x11, 0, 1, 0x40, 0 }
        .CopyTo(afterInstructionFixup, 0x80 + 0xE8);
    var afterInstructionGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(afterInstructionFixup)));
    Equal(afterInstructionGraph.JumpTables.Count, 1,
        "LE fixup beginning after the JMP must not modify guard evidence");
    Equal(afterInstructionGraph.JumpTables[0].CandidateTargets.Count, 10,
        "unrelated loader writes preserve validated finite index count");

    var signedByteSwitch = (byte[])byteSwitchBytes.Clone();
    signedByteSwitch[0x200 + 4] = 0x7F; // JG, signed rather than JA
    Equal(ControlFlowAnalysis.Analyze(LeImage.Load(
        BinaryArtifact.FromBytes(signedByteSwitch))).JumpTables.Count, 0,
        "signed guard cannot prove unsigned byte switch bounds");
    var otherByteGuard = (byte[])byteSwitchBytes.Clone();
    otherByteGuard[0x200 + 4] = 0x75; // JNZ cannot prove upper bound
    Equal(ControlFlowAnalysis.Analyze(LeImage.Load(
        BinaryArtifact.FromBytes(otherByteGuard))).JumpTables.Count, 0,
        "inequality guard cannot stand in for an unsigned JA");
    var signExtendSwitch = (byte[])byteSwitchBytes.Clone();
    signExtendSwitch[0x200 + 7] = 0xBE; // MOVSX EAX,AL
    Equal(ControlFlowAnalysis.Analyze(LeImage.Load(
        BinaryArtifact.FromBytes(signExtendSwitch))).JumpTables.Count, 0,
        "MOVSX may sign-extend and does not match safe byte MOVZX proof");
    var wrongByteSource = (byte[])byteSwitchBytes.Clone();
    wrongByteSource[0x200 + 8] = 0xC3; // MOVZX EAX,BL
    Equal(ControlFlowAnalysis.Analyze(LeImage.Load(
        BinaryArtifact.FromBytes(wrongByteSource))).JumpTables.Count, 0,
        "MOVZX source register must exactly match compared byte");
    var wrongIndexTarget = (byte[])byteSwitchBytes.Clone();
    wrongIndexTarget[0x200 + 8] = 0xC8; // MOVZX ECX,AL, JMP still indexes EAX
    Equal(ControlFlowAnalysis.Analyze(LeImage.Load(
        BinaryArtifact.FromBytes(wrongIndexTarget))).JumpTables.Count, 0,
        "MOVZX destination must exactly match indexed jump register");
    var wrongSegmentSwitch = (byte[])byteSwitchBytes.Clone();
    wrongSegmentSwitch[0x200 + 9] = 0x3E; // DS instead of CS override
    Equal(ControlFlowAnalysis.Analyze(LeImage.Load(
        BinaryArtifact.FromBytes(wrongSegmentSwitch))).JumpTables.Count, 0,
        "CS object-relative table proof is not borrowed by DS");
    Console.WriteLine("PASS: guarded LE byte-width SUB/CMP/JA/MOVZX switch and negative controls");

    // Part 17: relocation-backed, indexed dword jump tables are CANDIDATES,
    // not proof of all runtime index values. The synthetic fixture uses an
    // explicit absolute LE fixup at every table element.
    var tableBytes = MakeFixture(false);
    Dword(tableBytes, 0x80 + 0xc4, 96);
    Dword(tableBytes, 0x80 + 0x2c, 96);
    new byte[] { 0xFF, 0x24, 0x85, 0x40, 0x00, 0x01, 0x00 }
        .CopyTo(tableBytes, 0x200); // jmp dword ptr [eax*4+0x10040]
    tableBytes[0x200 + 0x20] = 0xC3;
    tableBytes[0x200 + 0x28] = 0xC3;
    tableBytes[0x200 + 0x30] = 0xC3;
    Dword(tableBytes, 0x80 + 0x68, 0xe0);
    Dword(tableBytes, 0x80 + 0x6c, 0xe8);
    Dword(tableBytes, 0x80 + 0xe4, 21);
    for (int i = 0; i < 3; i++)
    {
        int at = 0x80 + 0xe8 + i * 7;
        tableBytes[at] = 0x07; // internal 32-bit absolute pointer
        tableBytes[at + 1] = 0;
        BinaryPrimitives.WriteUInt16LittleEndian(tableBytes.AsSpan(at + 2, 2), (ushort)(0x40 + i * 4));
        tableBytes[at + 4] = 1; // target object
        BinaryPrimitives.WriteUInt16LittleEndian(tableBytes.AsSpan(at + 5, 2), (ushort)(0x20 + i * 8));
    }
    var tableImage = LeImage.Load(BinaryArtifact.FromBytes(tableBytes));
    var tableTrace = new AnalysisDecisionTrace(capacity: 8);
    var tableGraph = ControlFlowAnalysis.Analyze(tableImage, decisionTrace: tableTrace);
    Equal(tableTrace.Snapshot().Count, 8, "bounded event capacity");
    Equal(tableTrace.DroppedEvents > 0, true, "older event eviction disclosed");
    Equal(tableTrace.Snapshot().Any(x => x.Outcome == AnalysisDecisionOutcome.IndirectTargetQueued &&
        x.IsSpeculative && x.Destination is not null), true,
        "relocation-backed target remains speculative");
    // Sweep all competing four-byte fixup starts overlapping the
    // first relocated table slot. Exact-start duplicates are covered
    // separately; the six distinct overlapping origins all fail.
    for (int delta = -3; delta <= 3; delta++)
    {
        if (delta == 0) continue;
        var overlappingRelocatedFixup = (byte[])tableBytes.Clone();
        Dword(overlappingRelocatedFixup, 0x80 + 0xE4, 28);
        new byte[] {
            0x07, 0x00, (byte)(0x40 + delta), 0x00,
            0x01, 0x20, 0x00
        }.CopyTo(overlappingRelocatedFixup, 0x80 + 0xE8 + 21);
        var overlapRelocatedGraph = ControlFlowAnalysis.Analyze(
            LeImage.Load(BinaryArtifact.FromBytes(overlappingRelocatedFixup)));
        Equal(overlapRelocatedGraph.JumpTables.Count, 0,
            "overlapping relocated DWORD fixup delta=" + delta);
        Equal(overlapRelocatedGraph.Edges.Any(e =>
            e.Kind == "unresolved-indirect-jump"), true,
            "overlapping relocation retains unresolved control flow");
    }

    Equal(tableGraph.JumpTables.Count, 1, "LE relocation-backed table recognized");
    Equal(tableGraph.JumpTables[0].Table, new FlowAddress(1, 0x40), "jump table address");
    Equal(tableGraph.JumpTables[0].CandidateTargets.Count, 3, "three jump table candidates");
    Equal(tableGraph.JumpTables[0].DispatchKind, "indirect-jump-table",
        "relocation-backed jump retains dispatch type");
    // Explicit CS table addressing is within the current LE object, not
    // an arbitrary linear memory address. Other segments remain unresolved.
    var csBytes = (byte[])tableBytes.Clone();
    new byte[] { 0x2E, 0xFF, 0x24, 0x85, 0x40, 0x00, 0x00, 0x00 }
        .CopyTo(csBytes, 0x200);
    var csGraph = ControlFlowAnalysis.Analyze(LeImage.Load(BinaryArtifact.FromBytes(csBytes)));
    Equal(csGraph.JumpTables.Count, 1, "CS indexed relocation table is discoverable");
    Equal(csGraph.JumpTables[0].CandidateTargets.Count, 3,
        "CS table retains all relocation-backed candidate destinations");
    Equal(csGraph.JumpTables[0].Evidence.Contains("cs-object-relative", StringComparison.Ordinal),
        true, "CS-base assumption must appear in evidence");
    // The same independently source-proven unsigned byte guard must
    // also constrain a table whose stored entries are LE relocation
    // records, not merely the unrelocated-literal fallback.
    var relocatedByteSwitch = (byte[])tableBytes.Clone();
    new byte[] {
        0x2C, 0x02,       // SUB AL,2 (post-subtraction byte input)
        0x3C, 0x02,       // CMP AL,2
        0x77, 0x0B,       // JA default RET at 0x11
        0x0F, 0xB6, 0xC0, // MOVZX EAX,AL
        0x2E, 0xFF, 0x24, 0x85, 0x40, 0, 0, 0,
        0xC3
    }.CopyTo(relocatedByteSwitch, 0x200);
    var relocatedByteGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(relocatedByteSwitch)));
    Equal(relocatedByteGraph.JumpTables.Count, 1,
        "relocation-backed byte guard still recognizes its table");
    Equal(relocatedByteGraph.JumpTables[0].CandidateTargets.Count, 3,
        "CMP AL,2 keeps exactly three source-backed relocated slots");
    Equal(relocatedByteGraph.JumpTables[0].Evidence.Contains(
        "index-bounded-by-source-backed-unsigned-ja"), true,
        "relocation-backed table no longer mislabels proven index bounds");
    Equal(relocatedByteGraph.JumpTables[0].Evidence.Contains(
        "byte-register-MOVZX-zeroextend"), true,
        "relocation-backed byte index retains its proof pattern");
    Equal(relocatedByteGraph.Edges.Any(e =>
        e.Kind == "unresolved-indirect-jump"), true,
        "bounded relocated table remains runtime-unverified");

    // Relocation-backed table pointers remain independently inspectable
    // when the JA does not skip the dispatch. Their original fixups
    // must not be misrepresented as proof of an indexed upper bound.
    foreach (byte displacement in new byte[] { 0, 3, 10 })
    {
        var bypassRelocated = (byte[])relocatedByteSwitch.Clone();
        bypassRelocated[0x200 + 5] = displacement;
        var report = ControlFlowAnalysis.Analyze(
            LeImage.Load(BinaryArtifact.FromBytes(bypassRelocated)));
        Equal(report.JumpTables.Any(table =>
            table.Evidence.Contains(
                "index-bounded-by-source-backed-unsigned-ja")), false,
            "relocation-backed candidate cannot inherit JA dispatch bypass +" +
            displacement);
    }

    var shorterGuard = (byte[])relocatedByteSwitch.Clone();
    shorterGuard[0x200 + 3] = 0x01; // CMP AL,1: only slots 0 and 1
    var shortBoundGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(shorterGuard)));
    Equal(shortBoundGraph.JumpTables.Count, 1,
        "shorter guard retains relocation-backed pointer evidence");
    Equal(shortBoundGraph.JumpTables[0].CandidateTargets.Count, 2,
        "source-backed upper bound restricts larger stored relocation run");
    Equal(shortBoundGraph.JumpTables[0].Evidence.Contains(
        "index-bounded-by-source-backed-unsigned-ja"), true,
        "shorter guarded relocation table is labeled bounded");

    var oversizedGuard = (byte[])relocatedByteSwitch.Clone();
    oversizedGuard[0x200 + 3] = 0x04; // CMP AL,4 needs five pointer slots
    var incompleteSlots = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(oversizedGuard)));
    Equal(incompleteSlots.JumpTables.Count, 1,
        "original relocation-backed candidate table remains inspectable");
    Equal(incompleteSlots.JumpTables[0].Evidence.Contains(
        "index-bound-unproven"), true,
        "guard count exceeding validated pointer slots cannot be claimed bounded");
    Equal(incompleteSlots.JumpTables[0].CandidateTargets.Count, 3,
        "an incomplete relocation run is not expanded to a speculative five slots");

    var fsBytes = (byte[])csBytes.Clone();
    fsBytes[0x200] = 0x64;
    Equal(ControlFlowAnalysis.Analyze(LeImage.Load(BinaryArtifact.FromBytes(fsBytes)))
        .JumpTables.Count, 0, "FS selector base must not be guessed");
    var csGuardedBytes = (byte[])boundedBytes.Clone();
    csGuardedBytes[0x200 + 4] = 0x08; // JA over extra CS prefix
    new byte[] { 0x2E, 0xFF, 0x24, 0x85, 0x40, 0, 0, 0, 0xC3 }
        .CopyTo(csGuardedBytes, 0x200 + 5);
    for (int i = 0; i < 7; i++)
        Dword(csGuardedBytes, 0x200 + 0x40 + i * 4, (uint)(0x20 + i * 4));
    var csBoundedGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(csGuardedBytes)));
    Equal(csBoundedGraph.JumpTables.Count, 1,
        "guarded CS-relative literal table remains a conservative candidate");
    Equal(csBoundedGraph.JumpTables[0].CandidateTargets.Count, 7,
        "guarded CS-relative table recovers all seven validated offsets");

    var indexedCallBytes = (byte[])tableBytes.Clone();
    indexedCallBytes[0x200] = 0xFF;
    indexedCallBytes[0x201] = 0x14;
    indexedCallBytes[0x202] = 0x85; // call dword [eax*4+0x10040]
    var indexedCallGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(indexedCallBytes)));
    Equal(indexedCallGraph.JumpTables.Count, 1, "relocation-backed call table recovered");
    Equal(indexedCallGraph.JumpTables[0].DispatchKind, "indirect-call-table",
        "indexed indirect CALL is not mislabeled as an indirect JMP");
    Equal(indexedCallGraph.JumpTables[0].CandidateTargets.Count, 3,
        "indexed call table preserves its candidate destinations");
    using (var indexedCallReport = JsonDocument.Parse(AnalysisReportExport.BuildJson(indexedCallGraph)))
    {
        Equal(indexedCallReport.RootElement.GetProperty("counts").GetProperty("indirectCallTables").GetInt32(),
            1, "call table count in machine-readable report");
        Equal(indexedCallReport.RootElement.GetProperty("jumpTableCandidates")[0]
            .GetProperty("DispatchKind").GetString(), "indirect-call-table",
            "legacy dispatch table collection identifies call versus jump");
    }
    Equal(tableGraph.Instructions.Count, 4, "table candidates recursively decoded");
    Equal(tableGraph.Instructions.Where(x => x.Address.Offset is 0x20 or 0x28 or 0x30)
        .All(x => x.Evidence == "candidate-relocation-backed-indirect"), true,
        "indirect-only recovered instructions disclose speculative origin");
    Equal(tableGraph.Edges.Count(e => e.Kind == "candidate-indirect-jump"), 3,
        "distinct jump candidates recorded");
    Equal(tableGraph.Edges.Any(e => e.Kind == "unresolved-indirect-jump"), true,
        "unproven index bound stays unresolved");
    Equal(tableGraph.DataReferences.Any(x => x.Kind == "indexed-pointer-table-base" &&
        x.Target.Offset == 0x40), true, "jump-table base xref recorded");
    Equal(tableGraph.Ranges.Any(r => r.Begin <= 0x40 && r.EndExclusive > 0x40 &&
        r.Classification == "unknown-file-backed"), true,
        "pointer table not silently promoted to verified data");
    Equal(Convert.ToHexString(tableImage.ReadStoredBytes(1, 0x40, 4)), "00000000",
        "original pointer remains original");
    Equal(Convert.ToHexString(tableImage.ReadRelocatedBytes(1, 0x40, 4)), "20000100",
        "relocated pointer reconstructed without mutation");

    // The on-disk disp32 may be only an object-relative offset. An LE
    // relocation attached to that disp32 resolves the actual table base.
    var unrelocatedDisplacement = (byte[])tableBytes.Clone();
    Dword(unrelocatedDisplacement, 0x200 + 3, 0x40);
    Dword(unrelocatedDisplacement, 0x80 + 0xe4, 28);
    new byte[] { 0x07, 0x00, 0x03, 0x00, 0x01, 0x40, 0x00 }
        .CopyTo(unrelocatedDisplacement, 0x80 + 0xe8 + 21);
    var adjustedGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(unrelocatedDisplacement)));
    Equal(adjustedGraph.JumpTables.Count, 1,
        "relocation resolves on-disk unrelocated indexed displacement");
    Equal(adjustedGraph.DataReferences.Any(x => x.Kind == "relocated-operand" &&
        x.Target.Offset == 0x40), true, "displacement fixup source xref identified");

    var unprovenTableBytes = (byte[])tableBytes.Clone();
    Dword(unprovenTableBytes, 0x80 + 0x68, 0);
    Dword(unprovenTableBytes, 0x80 + 0x6c, 0);
    Dword(unprovenTableBytes, 0x80 + 0xe4, 0);
    // These literals LOOK like pointers, but there are no LE fixups.
    Dword(unprovenTableBytes, 0x200 + 0x40, 0x10020);
    Dword(unprovenTableBytes, 0x200 + 0x44, 0x10028);
    Dword(unprovenTableBytes, 0x200 + 0x48, 0x10030);
    var unprovenGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(unprovenTableBytes)));
    Equal(unprovenGraph.JumpTables.Count, 0, "coincidental literal addresses not promoted");
    Equal(unprovenGraph.Edges.Count(e => e.Kind == "candidate-indirect-jump"), 0,
        "unproved table generates no flow edges");

    var badTableBytes = (byte[])tableBytes.Clone();
    // Second pointer now targets outside the object; one entry does not
    // establish a table, even though three encoded fixups remain present.
    BinaryPrimitives.WriteUInt16LittleEndian(badTableBytes.AsSpan(0x80 + 0xe8 + 7 + 5, 2), 0x7F);
    var badTableGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(badTableBytes)));
    Equal(badTableGraph.JumpTables.Count, 0, "invalid second target breaks candidate table");

    var directDataBytes = (byte[])tableBytes.Clone();
    new byte[] { 0xA1, 0x40, 0x00, 0x01, 0x00, 0xC3 }
        .CopyTo(directDataBytes, 0x200); // mov eax,[0x10040]; ret
    var directDataGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(directDataBytes)));
    Equal(directDataGraph.DataReferences.Any(r => r.Kind == "absolute-memory-operand" &&
        r.Target == new FlowAddress(1, 0x40)), true,
        "absolute memory operand xref retains object-relative location");
    Equal(directDataGraph.JumpTables.Count, 0, "ordinary direct memory read isn't a table");

    var pointerBytes = (byte[])tableBytes.Clone();
    new byte[] { 0xFF, 0x15, 0x40, 0x00, 0x01, 0x00, 0xC3 }
        .CopyTo(pointerBytes, 0x200); // call dword ptr [0x10040]; ret
    Dword(pointerBytes, 0x80 + 0xe4, 7);
    var pointerGraph = ControlFlowAnalysis.Analyze(
        LeImage.Load(BinaryArtifact.FromBytes(pointerBytes)));
    Equal(pointerGraph.Edges.Any(e => e.Kind == "candidate-indirect-call" &&
        e.Destination == new FlowAddress(1, 0x20)), true,
        "relocation-backed absolute indirect call recognized");
    Equal(pointerGraph.Edges.Any(e => e.Kind == "unresolved-indirect-call"), true,
        "indirect call still carries unresolved runtime possibility");
    Equal(pointerGraph.Diagnostics.Any(d => d.Code == "unresolved-indirect-call"),
        true, "relocation-backed indirect calls still appear in unresolved diagnostics");
    using (var pointerExport = JsonDocument.Parse(AnalysisReportExport.BuildJson(pointerGraph)))
        Equal(pointerExport.RootElement.GetProperty("counts").GetProperty("unresolvedIndirectCalls").GetInt32(),
            1, "report records unresolved call despite candidate recovery");
    Equal(pointerGraph.FunctionCandidates.Any(f => f.Entry == new FlowAddress(1, 0x20) &&
        f.Evidence == "relocation-backed-indirect-call-candidate"), true,
        "indirect call candidate distinguished from direct call");

    var protectedTableGraph = ControlFlowAnalysis.Analyze(tableImage,
        protectedData: new[] { new ProtectedDataRange(1, 0x40, 0x4C, "confirmed-pointer-table") });
    Equal(protectedTableGraph.JumpTables.Count, 1,
        "analyst-protected table data can still supply fixup evidence");
    Equal(protectedTableGraph.Ranges.Any(r => r.Begin <= 0x40 && r.EndExclusive >= 0x4C &&
        r.Classification == "protected-data"), true, "protected table classification retained");
    Console.WriteLine("PASS: opt-in bounded analyzer decision trace, privacy and evidence offsets");
    Console.WriteLine("PASS: relocation-backed indirect calls, indexed jump tables, data xrefs and conservative uncertainty");
    Console.WriteLine("PASS: recursive CFG, function candidates, shared-tail ownership, protected data, conflicts and budgets");
    Console.WriteLine("PASS: synthetic nested/flat LE, address maps, decoder, immutability and malformed fixtures");

    if (args.Length == 2)
    {
        string[] names = { "CODE.1", "CODE.2" };
        string[] expectedHashes = {
            "fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed",
            "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
        };
        long[] starts = { 0x2c724, 0x3af24 };
        uint[] sizes = { 0x11302, 0x3a852 };
        uint[] entries = { 0xcc1c, 0x353a0 };
        for (int i = 0; i < 2; i++)
        {
            var art = BinaryArtifact.Open(args[i]);
            Equal(art.Sha256, expectedHashes[i], names[i] + " original SHA-256");
            var detected = BinaryFormatDetector.Inspect(art);
            Equal(detected.FormatId, "le-signature",
                names[i] + " nested protected-mode module is recognized from generic signatures");
            Equal(detected.SignatureFileOffset, 0x27bdcL,
                names[i] + " nested LE header is at its original physical file offset");
            Equal(detected.LoaderStatus, "le-validation-required",
                names[i] + " signature detection must not claim validated LE pages");
            Fails(() => DosMzImage.Load(art),
                names[i] + " outer DOS launcher is not a standalone 16-bit MZ");
            var outerDos = DosMzImage.LoadLauncherStub(art);
            Equal(outerDos.IsLauncherStub, true,
                names[i] + " outer DOS program is isolated only by explicit launcher mode");
            Equal(outerDos.DeclaredExecutableFileLength, 62532,
                names[i] + " independently inspected outer DOS MZ declared extent");
            Equal(outerDos.LoadImageFileOffset, 512,
                names[i] + " original DOS MZ header length");
            Equal(outerDos.Relocations.Count, 78,
                names[i] + " outer DOS relocation count");
            Equal(outerDos.InitialCs, (ushort)0x1BD,
                names[i] + " outer DOS entry segment");
            Equal(outerDos.InitialIp, (ushort)0x2382,
                names[i] + " outer DOS entry offset");
            Equal(outerDos.IsLauncherStub &&
                outerDos.CreateEntryCodeRegion().MappingEvidence.Contains(
                    "protected-mode-module-excluded", StringComparison.Ordinal),
                true, names[i] + " launcher CFG remains distinct from the LE object");
            Equal(outerDos.SimulateRelocations(0x2000).Summary.RelocationWriteCount,
                78, names[i] + " outer DOS fixup count reproduced without editing original bytes");
            var image = LeImage.Load(art);
            Equal(BinaryFormatDetector.Inspect(art, image).FormatId, "le",
                names[i] + " fully validated LE supersedes signature-only evidence");
            Equal(image.ModuleOrigin, 0x25d24L, names[i] + " module");
            Equal(image.HeaderOffset, 0x27bdcL, names[i] + " LE");
            Equal(image.Objects[0].VirtualSize, sizes[i], names[i] + " object size");
            Equal(image.EntryOffset, entries[i], names[i] + " entry");
            Equal(image.ToFileOffset(1, 0), starts[i], names[i] + " object start");
        }
        var code2 = LeImage.Load(BinaryArtifact.Open(args[1]));
        Equal(code2.ToFileOffset(1, 0x2975c), 0x64680L, "CODE.2 wrapper physical offset");
        var code1 = LeImage.Load(BinaryArtifact.Open(args[0]));
        Equal(code1.Relocations.Count, 2617, "CODE.1 internal fixups");
        Equal(code2.Relocations.Count, 9813, "CODE.2 internal fixups");
        Equal(Convert.ToHexString(code1.ReadRelocatedBytes(1, 0xf1c, 4)), "58090300", "CODE.1 first fixup target");
        Equal(Convert.ToHexString(code2.ReadRelocatedBytes(1, 0xba7, 4)), "E0980600", "CODE.2 first fixup target");
        var lines = Disassembly.Decode(code2, 1, 0x2975c, 4);
        Equal(lines[0].ObjectOffset, 0x2975cU, "CODE.2 wrapper instruction start");
        Equal(lines[0].Bytes, "FF742414", "CODE.2 wrapper first instruction bytes");
        // The graph is still a candidate traversal, not a complete disassembly
        // or a replacement for the verified CODE.2 audit. Include the proven
        // shared-return RET located immediately BEFORE the selected wrapper.
        var wrapper = ControlFlowAnalysis.Analyze(code2, new[] { new FlowAddress(1, 0x2975c) },
            maxInstructions: 3000);
        Equal(wrapper.Instructions.Any(x => x.Address.Offset == 0x2975c), true,
            "CODE.2 wrapper reached");
        Equal(wrapper.Instructions.Any(x => x.Address.Offset == 0x2975a && x.Assembly == "ret"), true,
            "CODE.2 backward shared return recognized");
        Equal(wrapper.Edges.Any(x => x.Source.Offset == 0x2976e &&
            x.Destination?.Offset == 0x2975a && x.Kind == "branch-taken"), true,
            "CODE.2 backward return-edge preserved");
        Console.WriteLine("PASS: CODE.2 shared-return wrapper recursive reachability against original bytes");
        Console.WriteLine("PASS: RLOFT original CODE.1/CODE.2 hashes/geometry/addresses/decoder fixtures");
    }
    Console.WriteLine("ALL SMOKE TESTS PASSED");
    return 0;
}
catch (Exception ex)
{
    Console.Error.WriteLine($"FAIL: {ex}");
    return 1;
}
