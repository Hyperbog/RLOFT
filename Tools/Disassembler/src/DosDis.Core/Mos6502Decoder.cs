namespace DosDis.Core;

/// <summary>
/// One decoded, documented NMOS 6502 instruction in a selected immutable CPU
/// ROM view. Unofficial opcodes are intentionally unknown, not treated as NOP.
/// </summary>
public sealed record Mos6502Instruction(
    ushort Address, byte Opcode, string Mnemonic, string Mode, string Operand,
    string BytesHex, int Length, string FlowKind, ushort? DirectTarget,
    IReadOnlyList<GuestSourceFragment> SourceFragments)
{
    public string Assembly => Operand.Length == 0 ? Mnemonic : Mnemonic + " " + Operand;
}

public static class Mos6502Decoder
{
    private sealed record Entry(string Mnemonic, string Mode);

    // The original 151 documented NMOS 6502 opcodes. Undocumented 2A03
    // opcodes need a separate semantics/length-verification gate.
    private const string OfficialTable = """
00:BRK:imp 01:ORA:izx 05:ORA:zp 06:ASL:zp 08:PHP:imp 09:ORA:imm 0A:ASL:acc 0D:ORA:abs 0E:ASL:abs
10:BPL:rel 11:ORA:izy 15:ORA:zpx 16:ASL:zpx 18:CLC:imp 19:ORA:aby 1D:ORA:abx 1E:ASL:abx
20:JSR:abs 21:AND:izx 24:BIT:zp 25:AND:zp 26:ROL:zp 28:PLP:imp 29:AND:imm 2A:ROL:acc 2C:BIT:abs 2D:AND:abs 2E:ROL:abs
30:BMI:rel 31:AND:izy 35:AND:zpx 36:ROL:zpx 38:SEC:imp 39:AND:aby 3D:AND:abx 3E:ROL:abx
40:RTI:imp 41:EOR:izx 45:EOR:zp 46:LSR:zp 48:PHA:imp 49:EOR:imm 4A:LSR:acc 4C:JMP:abs 4D:EOR:abs 4E:LSR:abs
50:BVC:rel 51:EOR:izy 55:EOR:zpx 56:LSR:zpx 58:CLI:imp 59:EOR:aby 5D:EOR:abx 5E:LSR:abx
60:RTS:imp 61:ADC:izx 65:ADC:zp 66:ROR:zp 68:PLA:imp 69:ADC:imm 6A:ROR:acc 6C:JMP:ind 6D:ADC:abs 6E:ROR:abs
70:BVS:rel 71:ADC:izy 75:ADC:zpx 76:ROR:zpx 78:SEI:imp 79:ADC:aby 7D:ADC:abx 7E:ROR:abx
81:STA:izx 84:STY:zp 85:STA:zp 86:STX:zp 88:DEY:imp 8A:TXA:imp 8C:STY:abs 8D:STA:abs 8E:STX:abs
90:BCC:rel 91:STA:izy 94:STY:zpx 95:STA:zpx 96:STX:zpy 98:TYA:imp 99:STA:aby 9A:TXS:imp 9D:STA:abx
A0:LDY:imm A1:LDA:izx A2:LDX:imm A4:LDY:zp A5:LDA:zp A6:LDX:zp A8:TAY:imp A9:LDA:imm AA:TAX:imp AC:LDY:abs AD:LDA:abs AE:LDX:abs
B0:BCS:rel B1:LDA:izy B4:LDY:zpx B5:LDA:zpx B6:LDX:zpy B8:CLV:imp B9:LDA:aby BA:TSX:imp BC:LDY:abx BD:LDA:abx BE:LDX:aby
C0:CPY:imm C1:CMP:izx C4:CPY:zp C5:CMP:zp C6:DEC:zp C8:INY:imp C9:CMP:imm CA:DEX:imp CC:CPY:abs CD:CMP:abs CE:DEC:abs
D0:BNE:rel D1:CMP:izy D5:CMP:zpx D6:DEC:zpx D8:CLD:imp D9:CMP:aby DD:CMP:abx DE:DEC:abx
E0:CPX:imm E1:SBC:izx E4:CPX:zp E5:SBC:zp E6:INC:zp E8:INX:imp E9:SBC:imm EA:NOP:imp EC:CPX:abs ED:SBC:abs EE:INC:abs
F0:BEQ:rel F1:SBC:izy F5:SBC:zpx F6:INC:zpx F8:SED:imp F9:SBC:aby FD:SBC:abx FE:INC:abx
""";

    private static readonly IReadOnlyDictionary<byte, Entry> Official = BuildTable();

    public static int DocumentedOpcodeCount => Official.Count;

    private static IReadOnlyDictionary<byte, Entry> BuildTable()
    {
        var table = new Dictionary<byte, Entry>();
        foreach (string token in OfficialTable.Split(
            new[] { ' ', '\r', '\n', '\t' }, StringSplitOptions.RemoveEmptyEntries))
        {
            string[] parts = token.Split(':');
            if (parts.Length != 3 ||
                !table.TryAdd(Convert.ToByte(parts[0], 16), new Entry(parts[1], parts[2])))
                throw new InvalidDataException("Invalid 6502 documented opcode table.");
        }
        if (table.Count != 151)
            throw new InvalidDataException("Incomplete documented 6502 opcode table.");
        return table;
    }

    public static Mos6502Instruction? TryDecode(
        GuestSourceMemory memory, string viewId, ushort address)
    {
        ArgumentNullException.ThrowIfNull(memory);
        GuestSourceRead? first = memory.TryReadExact("nes.cpu", viewId, address, 1);
        if (first is null || !Official.TryGetValue(first.Bytes[0], out Entry? entry))
            return null;

        int length = entry.Mode switch
        {
            "imp" or "acc" => 1,
            "imm" or "zp" or "zpx" or "zpy" or "izx" or "izy" or "rel" => 2,
            "abs" or "abx" or "aby" or "ind" => 3,
            _ => throw new InvalidDataException("Invalid 6502 addressing mode.")
        };
        GuestSourceRead? read = memory.TryReadExact("nes.cpu", viewId, address, length);
        if (read is null) return null;
        byte[] bytes = read.Bytes;
        ushort word = length == 3 ? (ushort)(bytes[1] | bytes[2] << 8) : (ushort)0;
        ushort? target = entry.Mode == "rel"
            ? (ushort)((address + length + (sbyte)bytes[1]) & 0xFFFF)
            : entry.Mode == "abs" && entry.Mnemonic is "JSR" or "JMP"
                ? word : null;
        string operand = entry.Mode switch
        {
            "imp" => "",
            "acc" => "A",
            "imm" => "#$" + bytes[1].ToString("X2"),
            "zp" => "$" + bytes[1].ToString("X2"),
            "zpx" => "$" + bytes[1].ToString("X2") + ",X",
            "zpy" => "$" + bytes[1].ToString("X2") + ",Y",
            "izx" => "($" + bytes[1].ToString("X2") + ",X)",
            "izy" => "($" + bytes[1].ToString("X2") + "),Y",
            "rel" => "$" + target!.Value.ToString("X4"),
            "abs" => "$" + word.ToString("X4"),
            "abx" => "$" + word.ToString("X4") + ",X",
            "aby" => "$" + word.ToString("X4") + ",Y",
            "ind" => "($" + word.ToString("X4") + ")",
            _ => throw new InvalidDataException("Invalid 6502 addressing mode.")
        };
        string flow = entry.Mnemonic switch
        {
            "JMP" when entry.Mode == "ind" => "indirect-jump",
            "JMP" => "jump",
            "JSR" => "call",
            "RTS" or "RTI" => "return",
            "BRK" => "interrupt",
            "BPL" or "BMI" or "BVC" or "BVS" or "BCC" or "BCS" or "BNE" or "BEQ"
                => "conditional",
            _ => "fallthrough"
        };
        return new Mos6502Instruction(address, bytes[0], entry.Mnemonic, entry.Mode,
            operand, Convert.ToHexString(bytes), length, flow, target, read.Fragments);
    }
}

/// <summary>A bank-sensitive control-flow edge in one explicitly chosen view.</summary>
public sealed record Mos6502Edge(ushort From, ushort To, string Kind);

/// <summary>Unknown target/bytes are preserved as unknown, not fabricated code.</summary>
public sealed record Mos6502Unresolved(ushort Address, string Reason);

public sealed record Mos6502Graph(
    string SourceSha256, string ViewId,
    IReadOnlyList<Mos6502Instruction> Instructions,
    IReadOnlyList<Mos6502Edge> Edges,
    IReadOnlyList<Mos6502Unresolved> Unresolved,
    bool Truncated);

public static class Mos6502FlowAnalysis
{
    /// <summary>
    /// Evidence-bounded static discovery: branching follows only direct ROM
    /// addresses; indirect jumps and interrupts need independent runtime proof.
    /// The caller selects a bank view and entry points, not the analyzer.
    /// </summary>
    public static Mos6502Graph Analyze(GuestSourceMemory memory, string viewId,
        IEnumerable<ushort> seeds, int maxInstructions = 50_000)
    {
        ArgumentNullException.ThrowIfNull(memory);
        ArgumentNullException.ThrowIfNull(seeds);
        if (maxInstructions is < 1 or > 100_000)
            throw new ArgumentOutOfRangeException(nameof(maxInstructions));

        var todo = new Queue<ushort>(seeds.Distinct());
        var seen = new HashSet<ushort>();
        var decoded = new List<Mos6502Instruction>();
        var edges = new List<Mos6502Edge>();
        var unresolved = new List<Mos6502Unresolved>();
        bool truncated = false;
        // An operand byte may not silently become another instruction
        // merely because a second static entry hypothesis points into it.
        var occupied = new Dictionary<ushort, ushort>();
        while (todo.Count != 0)
        {
            if (decoded.Count >= maxInstructions)
            {
                truncated = true;
                break;
            }
            ushort address = todo.Dequeue();
            if (!seen.Add(address)) continue;

            Mos6502Instruction? inst = Mos6502Decoder.TryDecode(memory, viewId, address);
            if (inst is null)
            {
                string reason = memory.TryReadExact("nes.cpu", viewId, address, 1) is null
                    ? "source-unmapped" : "undocumented-opcode-or-truncated-instruction";
                unresolved.Add(new Mos6502Unresolved(address, reason));
                continue;
            }
            bool overlap = false;
            for (int i = 0; i < inst.Length; i++)
                if (occupied.ContainsKey((ushort)(inst.Address + i)))
                    overlap = true;
            if (overlap)
            {
                unresolved.Add(new Mos6502Unresolved(inst.Address,
                    "overlapping-instruction-candidates"));
                continue;
            }
            for (int i = 0; i < inst.Length; i++)
                occupied[(ushort)(inst.Address + i)] = inst.Address;
            decoded.Add(inst);
            ushort next = (ushort)(inst.Address + inst.Length);
            void Follow(ushort target, string kind)
            {
                edges.Add(new Mos6502Edge(inst.Address, target, kind));
                if (!seen.Contains(target)) todo.Enqueue(target);
            }
            switch (inst.FlowKind)
            {
                case "conditional":
                    Follow(inst.DirectTarget!.Value, "branch-taken");
                    Follow(next, "branch-not-taken");
                    break;
                case "call":
                    Follow(inst.DirectTarget!.Value, "call");
                    Follow(next, "return-continuation-candidate");
                    break;
                case "jump":
                    Follow(inst.DirectTarget!.Value, "jump");
                    break;
                case "indirect-jump":
                    unresolved.Add(new Mos6502Unresolved(inst.Address,
                        "indirect-jump-needs-runtime-pointer"));
                    break;
                case "return":
                case "interrupt":
                    break;
                default:
                    Follow(next, "fallthrough");
                    break;
            }
        }
        return new Mos6502Graph(memory.SourceSha256, viewId,
            decoded.OrderBy(x => x.Address).ToArray(), edges.ToArray(),
            unresolved.ToArray(), truncated);
    }
}
