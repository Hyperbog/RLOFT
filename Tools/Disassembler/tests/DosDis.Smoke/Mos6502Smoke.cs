using DosDis.Core;

internal static class Mos6502Smoke
{
    private static void Same<T>(T actual, T expected, string label)
    {
        if (!EqualityComparer<T>.Default.Equals(actual, expected))
            throw new Exception("6502 " + label + ": expected " + expected + ", got " + actual);
    }

    private static (GuestSourceMemory Memory, string ViewId) Fixture()
    {
        byte[] b = new byte[0x4010];
        // Original iNES file-backed PRG starts at physical offset 16.
        void Put(int address, params byte[] values) =>
            values.AsSpan().CopyTo(b.AsSpan(16 + address - 0x8000));
        Put(0x8000, 0xA9, 0x01);       // LDA #$01
        Put(0x8002, 0x20, 0x10, 0x80); // JSR $8010
        Put(0x8005, 0xD0, 0x02);       // BNE $8009
        Put(0x8007, 0xEA);             // NOP
        Put(0x8008, 0x60);             // RTS
        Put(0x8009, 0x4C, 0x14, 0x80); // JMP $8014
        Put(0x8010, 0xE8, 0x60);       // INX, RTS
        Put(0x8014, 0x6C, 0x00, 0x90); // JMP ($9000), runtime data unknown
        // Unofficial opcode must not be declared regular 6502 code.
        Put(0x8020, 0x02);
        var artifact = BinaryArtifact.FromBytes(b);
        var map = new GuestSourceMemory(artifact, new[]
        {
            new GuestSourceWindow("nes.cpu", "bank-0", 0x8000, 0x4000, 16)
        });
        return (map, "bank-0");
    }

    public static void Run()
    {
        Same(Mos6502Decoder.DocumentedOpcodeCount, 151, "complete documented opcode count");
        var (memory, view) = Fixture();
        Mos6502Instruction lda = Mos6502Decoder.TryDecode(memory, view, 0x8000)!;
        Same(lda.Mnemonic, "LDA", "opcode A9");
        Same(lda.Assembly, "LDA #$01", "immediate operand");
        Same(lda.BytesHex, "A901", "exact instruction bytes");
        Same(lda.SourceFragments[0].PhysicalStart, 16L, "original file provenance");
        Same(Mos6502Decoder.TryDecode(memory, view, 0x8002)!.DirectTarget,
            (ushort)0x8010, "JSR absolute target");
        Same(Mos6502Decoder.TryDecode(memory, view, 0x8005)!.DirectTarget,
            (ushort)0x8009, "BNE signed positive target");
        Same(Mos6502Decoder.TryDecode(memory, view, 0x8014)!.FlowKind,
            "indirect-jump", "JMP indirect unresolved flow");
        Same(Mos6502Decoder.TryDecode(memory, view, 0x8020) is null,
            true, "no invention for unofficial opcode 02");
        Same(Mos6502Decoder.TryDecode(memory, view, 0x7FFF) is null,
            true, "no reads from unmapped RAM");
        Mos6502Graph graph = Mos6502FlowAnalysis.Analyze(memory, view, new ushort[] { 0x8000 });
        Same(graph.Instructions.Count, 9, "nine reachable original-backed instructions");
        Same(graph.Edges.Any(x => x.From == 0x8005 &&
            x.To == 0x8009 && x.Kind == "branch-taken"), true, "taken edge");
        Same(graph.Edges.Any(x => x.From == 0x8005 &&
            x.To == 0x8007 && x.Kind == "branch-not-taken"), true, "untaken edge");
        Same(graph.Edges.Any(x => x.From == 0x8002 &&
            x.To == 0x8010 && x.Kind == "call"), true, "subroutine edge");
        Same(graph.Unresolved.Any(x => x.Address == 0x8014 &&
            x.Reason == "indirect-jump-needs-runtime-pointer"), true, "indirect target recorded");
        Same(graph.Truncated, false, "complete small fixture");
        var capped = Mos6502FlowAnalysis.Analyze(memory, view, new ushort[] { 0x8000 }, 2);
        Same(capped.Truncated, true, "analysis cap explicit");

        var bad = Mos6502FlowAnalysis.Analyze(memory, view, new ushort[] { 0x8020 });
        Same(bad.Instructions.Count, 0, "unsupported opcode blocks false code discovery");
        Same(bad.Unresolved[0].Reason, "undocumented-opcode-or-truncated-instruction",
            "unofficial opcode classification");
        byte[] negative = { 0xD0, 0xFC, 0x00 };
        var back = new GuestSourceMemory(BinaryArtifact.FromBytes(negative),
            new[] { new GuestSourceWindow("nes.cpu", "relative", 0x8000, 3, 0) });
        Same(Mos6502Decoder.TryDecode(back, "relative", 0x8000)!.DirectTarget,
            (ushort)0x7FFE, "signed backwards relative target");
        Same(Mos6502Decoder.TryDecode(back, "relative", 0x8002)!.FlowKind,
            "interrupt", "BRK does not invent an immediate successor");
        var wraps = new GuestSourceMemory(BinaryArtifact.FromBytes(new byte[] { 0xD0, 0x02 }),
            new[] { new GuestSourceWindow("nes.cpu", "wrap", 0xFFFE, 2, 0) });
        Same(Mos6502Decoder.TryDecode(wraps, "wrap", 0xFFFE)!.DirectTarget,
            (ushort)0x0002, "PC branch wraps 16-bit CPU address");
        Console.WriteLine("PASS: 151 documented NES 6502 opcodes, exact source-backed decode, " +
            "branches, subroutine control flow, unknown opcode and bounded discovery");
    }
}
