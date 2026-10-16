using DosDis.Core;

internal static class Z80Smoke
{
    private static void Same<T>(T actual, T expected, string label)
    {
        if (!EqualityComparer<T>.Default.Equals(actual, expected))
            throw new Exception("Z80 " + label + ": expected " + expected + ", got " + actual);
    }

    public static void Run()
    {
        byte[] bytes = new byte[0x200];
        void Put(int guest, params byte[] opcode) =>
            opcode.CopyTo(bytes, guest - 0x4000);
        Put(0x4000, 0x31, 0x00, 0xD0); // LD SP,D000
        Put(0x4003, 0xCD, 0x10, 0x40); // CALL 4010
        Put(0x4006, 0x20, 0x03);       // JR NZ,400B
        Put(0x4008, 0x00);             // NOP
        Put(0x4009, 0xC9);             // RET
        Put(0x400B, 0xC3, 0x20, 0x40); // JP 4020
        Put(0x4010, 0xCB, 0x7E);       // BIT 7,(HL)
        Put(0x4012, 0xC9);             // RET
        Put(0x4020, 0xED, 0xB0);       // LDIR
        Put(0x4022, 0xE9);             // JP (HL), unresolved
        Put(0x4030, 0xDD, 0x21, 0x00, 0x80); // unsupported indexed instruction
        Put(0x4040, 0xCB, 0x36);       // undocumented SLL (HL)
        Put(0x4050, 0xED, 0x00);       // undocumented ED opcode
        Put(0x4060, 0x18, 0xFC);       // JR -4 (to 405E)
        Put(0x4070, 0xC0);             // RET NZ
        Put(0x4080, 0xCF);             // RST 08
        Put(0x4090, 0xED, 0x4B, 0x34, 0x12); // LD BC,(1234)
        Put(0x40A0, 0xED, 0x56);       // IM 1
        Put(0x40A2, 0xED, 0x5E);       // IM 2
        var source = BinaryArtifact.FromBytes(bytes);
        var memory = new GuestSourceMemory(source, new[]
        {
            new GuestSourceWindow("sms.cpu", "page", 0x4000, (uint)bytes.Length, 0)
        });
        Z80Instruction At(int a) => Z80Decoder.TryDecode(memory, "page", (ushort)a)
            ?? throw new Exception("Missing Z80 instruction at " + a.ToString("X4"));
        Same(At(0x4000).Assembly, "LD SP,$D000", "16-bit little endian");
        Same(At(0x4000).Length, 3, "LD SP byte length");
        Same(At(0x4000).SourceFragments[0].PhysicalStart, 0L, "source provenance");
        Same(At(0x4003).DirectTarget, (ushort)0x4010, "CALL target");
        Same(At(0x4006).Assembly, "JR NZ,$400B", "JR relative target");
        Same(At(0x4010).Assembly, "BIT 7,(HL)", "CB bit opcode");
        Same(At(0x4020).Assembly, "LDIR", "ED repeating operation");
        Same(At(0x4022).FlowKind, "indirect-jump", "indirect register jump");
        Same(Z80Decoder.TryDecode(memory, "page", 0x4030) is null,
            true, "indexed prefix never decoded as base instruction");
        Same(Z80Decoder.TryDecode(memory, "page", 0x4040) is null,
            true, "undocumented SLL abstention");
        Same(Z80Decoder.TryDecode(memory, "page", 0x4050) is null,
            true, "unknown ED abstention");
        Same(At(0x4060).DirectTarget, (ushort)0x405E, "signed JR negative displacement");
        Same(At(0x4070).FlowKind, "conditional-return", "RET cc not forced fallthrough");
        Same(At(0x4080).DirectTarget, (ushort)8, "RST vector");
        Same(At(0x4090).Assembly, "LD BC,($1234)", "ED indirect 16-bit load");
        Same(At(0x40A0).Assembly, "IM 1", "documented IM1");
        Same(At(0x40A2).Assembly, "IM 2", "documented IM2");
        Z80Graph g = Z80FlowAnalysis.Analyze(memory, "page", new ushort[] { 0x4000 });
        Same(g.Instructions.Count, 10, "reachable base/CB/ED instruction count");
        Same(g.Edges.Any(x => x.From == 0x4006 &&
            x.To == 0x400B && x.Kind == "branch-taken"), true, "conditional branch");
        Same(g.Edges.Any(x => x.From == 0x4003 &&
            x.To == 0x4010 && x.Kind == "call"), true, "call edge");
        Same(g.Unknowns.Any(x => x.Address == 0x4022 &&
            x.Reason == "indirect-jump-register-target"), true, "indirect branch unknown");
        Same(Z80FlowAnalysis.Analyze(memory, "page", new ushort[] { 0x4030 }).Instructions.Count,
            0, "no false function from unsupported IX");
        Same(Z80FlowAnalysis.Analyze(memory, "page", new ushort[] { 0x4000 }, 1).Truncated,
            true, "bounded graph recovery");
        Console.WriteLine("PASS: Z80 core, CB and supported ED instructions, byte lengths, " +
            "signed edges, calls, bounded graph and explicit indexed/undocumented abstentions");
    }
}
