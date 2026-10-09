using DosDis.Core;

internal static class Dos16RegisterTransferSmoke
{
    private static void Check(bool condition, string name)
    {
        if (!condition) throw new Exception("x86-16 transfer: " + name);
    }

    private static void BadSource(Action action, string name)
    {
        bool rejected = false;
        try { action(); }
        catch (InvalidDataException) { rejected = true; }
        Check(rejected, name);
    }

    internal static void Run()
    {
        // Synthetic 16-bit instruction stream; no original game bytes.
        byte[] source = {
            0xB8, 0x34, 0x12,       // MOV AX,1234h
            0xBB, 0x02, 0x00,       // MOV BX,2
            0x89, 0xC3,             // MOV BX,AX
            0x83, 0xC3, 0xFF,       // ADD BX,-1
            0x81, 0xEB, 0x02, 0x00, // SUB BX,2
            0x3D, 0x34, 0x12,       // CMP AX,1234h (flags not modeled)
            0xB0, 0x7F,             // MOV AL,7Fh (partial alias => kill)
            0x90,                   // NOP
            0xB8, 0xFE, 0xFF,       // MOV AX,FFFEh
            0x05, 0x01, 0x00,       // ADD AX,1
            0x2D, 0x03, 0x00,       // SUB AX,3
            0x8B, 0xD8,             // MOV BX,AX
            0x83, 0xEB, 0xFF,       // SUB BX,-1
            0x83, 0xFB, 0xFF,       // CMP BX,-1
            0xEB, 0x00,             // JMP short +0 (unmodeled transfer)
            0xC3                    // RET (unmodeled transfer)
        };
        var artifact = BinaryArtifact.FromBytes(source, "synthetic.bin");
        var region = DosComImage.LoadExplicit(artifact).CreateCodeRegion();
        var graph = Dos16ControlFlowAnalysis.Analyze(region, 0x100);
        Check(!graph.Truncated && graph.Instructions.Count == 16,
            "exact synthetic linear/branch graph available");
        var current = Dos16RegisterSnapshot.UnknownFor(region);
        var initial = current;

        Dos16TransferResult Step(int fileOffset)
        {
            var instruction = graph.Instructions.Single(x =>
                x.Address.Offset == 0x100 + fileOffset);
            var result = Dos16RegisterTransfer.Apply(region, instruction, current);
            current = result.After;
            return result;
        }
        Dos16AbstractValue Value(Dos16WordRegister register) => current.Get(register);
        bool Is(Dos16WordRegister register, ushort expected) =>
            Value(register).Kind == Dos16ValueKind.Constant &&
            Value(register).Minimum == expected;

        var first = Step(0);
        Check(Is(Dos16WordRegister.AX, 0x1234) &&
            Value(Dos16WordRegister.BX).Kind == Dos16ValueKind.Unknown,
            "MOV AX immediate changes only AX");
        Check(first.SourceOrigin is not null &&
            first.SourceOrigin.SourceSha256 == region.SourceSha256 &&
            first.SourceOrigin.SourceFileOffset == 0 && first.FlagsPreserved,
            "machine-byte backed origin and flag retention");

        Step(3);
        Check(Is(Dos16WordRegister.BX, 2), "MOV BX immediate");
        Step(6);
        Check(Is(Dos16WordRegister.BX, 0x1234), "89 register copy direction");
        var add = Step(8);
        Check(Is(Dos16WordRegister.BX, 0x1233) &&
            !add.FlagsPreserved, "83 sign-extended addition kills flag certainty");
        Step(11);
        Check(Is(Dos16WordRegister.BX, 0x1231), "81 subtract word immediate");

        var cmp = Step(15);
        Check(Is(Dos16WordRegister.AX, 0x1234) &&
            Is(Dos16WordRegister.BX, 0x1231) &&
            !cmp.FlagsPreserved && cmp.SourceOrigin is null,
            "CMP preserves registers but does not invent flags or value fact");
        var partial = Step(18);
        Check(partial.RegistersInvalidated &&
            Value(Dos16WordRegister.AX).Kind == Dos16ValueKind.Unknown &&
            Value(Dos16WordRegister.BX).Kind == Dos16ValueKind.Unknown,
            "unmodeled AL alias clears all 16-bit register guesses");
        var nop = Step(20);
        Check(ReferenceEquals(partial.After, nop.After) && nop.FlagsPreserved,
            "byte-exact NOP preserves state");
        Step(21);
        Check(Is(Dos16WordRegister.AX, 0xFFFE), "recover precise value after invalidation");
        Step(24);
        Check(Is(Dos16WordRegister.AX, 0xFFFF), "accumulator ADD immediate");
        Step(27);
        Check(Is(Dos16WordRegister.AX, 0xFFFC), "accumulator SUB immediate");
        Step(30);
        Check(Is(Dos16WordRegister.BX, 0xFFFC), "8B register copy direction");
        Step(32);
        Check(Is(Dos16WordRegister.BX, 0xFFFD), "SUB of signed imm8 -1 is addition");
        Step(35);
        Check(Is(Dos16WordRegister.BX, 0xFFFD), "83 /7 compare leaves BX intact");
        var jump = Step(38);
        Check(!jump.RegistersInvalidated && jump.FlagsPreserved &&
            Is(Dos16WordRegister.BX, 0xFFFD),
            "byte-verified direct JMP preserves register state");
        Check(Step(40).RegistersInvalidated,
            "RET cannot imply a preserved call context");

        var valid = graph.Instructions.Single(x => x.Address.Offset == 0x100);
        BadSource(() => Dos16RegisterTransfer.Apply(region,
            valid with { SourceFileOffset = 1 }, initial),
            "wrong physical source offset rejected");
        BadSource(() => Dos16RegisterTransfer.Apply(region,
            valid with { OriginalBytes = "B8FFFF" }, initial),
            "forged instruction bytes rejected");
        BadSource(() => Dos16RegisterTransfer.Apply(region,
            valid with { FlowControl = "Return" }, initial),
            "forged control flow label rejected");
        var alternateRegion = DosComImage.LoadExplicit(artifact, 0x200).CreateCodeRegion();
        BadSource(() => Dos16RegisterTransfer.Apply(region, valid,
            Dos16RegisterSnapshot.UnknownFor(alternateRegion)),
            "same file in different load window cannot borrow state");
        var otherRegion = DosComImage.LoadExplicit(BinaryArtifact.FromBytes(
            new byte[] { 0x90, 0xC3 }), 0x100).CreateCodeRegion();
        BadSource(() => Dos16RegisterTransfer.Apply(region, valid,
            Dos16RegisterSnapshot.UnknownFor(otherRegion)),
            "unrelated original source cannot borrow state");

        Check(Value(Dos16WordRegister.AX).Kind == Dos16ValueKind.Unknown,
            "unknown transfer never leaves stale AX interpretation");
        Console.WriteLine("PASS: exact x86-16 register transfers, alias invalidation and source identity");
    }
}
