using System.Buffers.Binary;
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
    var graph = ControlFlowAnalysis.Analyze(graphImage, protectedData: new[] {
        new ProtectedDataRange(1, 6, 10, "synthetic-known-data") });
    Equal(graph.ArtifactSha256, BinaryArtifact.FromBytes(graphBytes).Sha256, "graph source identity");
    Equal(graph.Instructions.Count, 4, "reachable-only decoded starts");
    Equal(graph.Blocks.Count, 4, "basic blocks and branch leaders");
    Equal(graph.FunctionCandidates.Count, 2, "call target discovers candidate callee");
    Equal(graph.Edges.Any(e => e.Kind == "call" && e.Destination?.Offset == 0x10), true, "direct call reference");
    Equal(graph.Edges.Any(e => e.Kind == "branch-taken" && e.Destination?.Offset == 5), true, "backward branch into shared RET");
    Equal(graph.Edges.Any(e => e.Kind == "unresolved-indirect-jump" && e.Destination is null), true, "indirect branch stays unresolved");
    Equal(graph.FunctionCandidates.All(f => f.Blocks.Contains(new FlowAddress(1, 5))), true,
        "shared return block may belong to multiple candidates");
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
    var overlapping = ControlFlowAnalysis.Analyze(graphImage, new[] {
        new FlowAddress(1, 0), new FlowAddress(1, 2) });
    Equal(overlapping.Diagnostics.Any(d => d.Code == "mid-instruction-target"), true,
        "mid-instruction overlapping seed reported");
    Equal(overlapping.Instructions.Any(x => x.Address.Offset == 2), false,
        "mid-instruction seed not emitted as verified instruction");
    var blocked = ControlFlowAnalysis.Analyze(graphImage, new[] { new FlowAddress(1, 0x10) },
        new[] { new ProtectedDataRange(1, 5, 6) });
    Equal(blocked.Diagnostics.Any(d => d.Code == "protected-data-target"), true,
        "branch targeting protected data is halted");
    Equal(blocked.Instructions.Any(x => x.Address.Offset == 5), false,
        "protected byte not decoded");
    var limited = ControlFlowAnalysis.Analyze(graphImage, maxInstructions: 1);
    Equal(limited.Truncated, true, "analysis budget truncation is disclosed");
    Equal(limited.Instructions.Count, 1, "analysis respects budget");
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
    var tableGraph = ControlFlowAnalysis.Analyze(tableImage);
    Equal(tableGraph.JumpTables.Count, 1, "LE relocation-backed table recognized");
    Equal(tableGraph.JumpTables[0].Table, new FlowAddress(1, 0x40), "jump table address");
    Equal(tableGraph.JumpTables[0].CandidateTargets.Count, 3, "three jump table candidates");
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
    Equal(pointerGraph.FunctionCandidates.Any(f => f.Entry == new FlowAddress(1, 0x20) &&
        f.Evidence == "relocation-backed-indirect-call-candidate"), true,
        "indirect call candidate distinguished from direct call");

    var protectedTableGraph = ControlFlowAnalysis.Analyze(tableImage,
        protectedData: new[] { new ProtectedDataRange(1, 0x40, 0x4C, "confirmed-pointer-table") });
    Equal(protectedTableGraph.JumpTables.Count, 1,
        "analyst-protected table data can still supply fixup evidence");
    Equal(protectedTableGraph.Ranges.Any(r => r.Begin <= 0x40 && r.EndExclusive >= 0x4C &&
        r.Classification == "protected-data"), true, "protected table classification retained");
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
            var image = LeImage.Load(art);
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
