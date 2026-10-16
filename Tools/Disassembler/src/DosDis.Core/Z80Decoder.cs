namespace DosDis.Core;

/// <summary>
/// Source-backed Z80 instruction in a selected SMS cartridge mapping.
/// Only verified core, CB and documented ED forms are emitted. DD/FD indexed
/// forms and undocumented ED/CB opcodes explicitly abstain until supported.
/// </summary>
public sealed record Z80Instruction(
    ushort Address, string Assembly, int Length, string OriginalBytes,
    string FlowKind, ushort? DirectTarget,
    IReadOnlyList<GuestSourceFragment> SourceFragments);

public static class Z80Decoder
{
    private static readonly string[] Reg8 = { "B", "C", "D", "E", "H", "L", "(HL)", "A" };
    private static readonly string[] Reg16 = { "BC", "DE", "HL", "SP" };
    private static readonly string[] Reg16Stack = { "BC", "DE", "HL", "AF" };
    private static readonly string[] Conditions = { "NZ", "Z", "NC", "C", "PO", "PE", "P", "M" };
    private static readonly string[] Alu = { "ADD A,", "ADC A,", "SUB ", "SBC A,",
        "AND ", "XOR ", "OR ", "CP " };
    private static readonly string[] Rotations = { "RLC", "RRC", "RL", "RR", "SLA", "SRA", "SLL", "SRL" };
    private static readonly string[] Accumulator = { "RLCA", "RRCA", "RLA", "RRA",
        "DAA", "CPL", "SCF", "CCF" };

    public static Z80Instruction? TryDecode(GuestSourceMemory memory,
        string view, ushort address)
    {
        ArgumentNullException.ThrowIfNull(memory);
        int used = 0;
        var data = new List<byte>(4);
        byte Read()
        {
            if (used >= 8)
                throw new InvalidDataException("Z80 instruction exceeds bounded decoding size.");
            ushort guest = (ushort)(address + used);
            GuestSourceRead? read = memory.TryReadExact("sms.cpu", view, guest, 1);
            if (read is null)
                throw new EndOfStreamException("Z80 instruction references unmapped source bytes.");
            used++;
            data.Add(read.Bytes[0]);
            return read.Bytes[0];
        }
        byte first;
        try { first = Read(); }
        catch (EndOfStreamException) { return null; }
        // Indexed prefixes change both instruction lengths and semantics.
        // Never misinterpret their following operand bytes as a base opcode.
        if (first is 0xDD or 0xFD)
            return null;
        string asm = "";
        string flow = "fallthrough";
        ushort? target = null;
        string Imm8() => "$" + Read().ToString("X2");
        ushort Word() { byte low = Read(); return (ushort)(low | Read() << 8); }
        string Imm16() => "$" + Word().ToString("X4");
        ushort Relative()
        {
            sbyte delta = (sbyte)Read();
            return (ushort)(address + used + delta);
        }
        try
        {
            if (first == 0xCB)
            {
                byte op = Read();
                int x = op >> 6, y = (op >> 3) & 7, z = op & 7;
                // SLL is not a documented Zilog instruction. Avoid a false
                // positive until unofficial semantics are independently tested.
                if (x == 0 && y == 6) return null;
                asm = x switch
                {
                    0 => Rotations[y] + " " + Reg8[z],
                    1 => "BIT " + y + "," + Reg8[z],
                    2 => "RES " + y + "," + Reg8[z],
                    _ => "SET " + y + "," + Reg8[z]
                };
            }
            else if (first == 0xED)
            {
                byte op = Read();
                int x = op >> 6, y = (op >> 3) & 7, z = op & 7;
                int p = y >> 1, q = y & 1;
                if (x == 1)
                {
                    switch (z)
                    {
                        case 0:
                            if (y == 6) return null; // undocumented IN (C)
                            asm = "IN " + Reg8[y] + ",(C)";
                            break;
                        case 1:
                            if (y == 6) return null; // undocumented OUT (C),0
                            asm = "OUT (C)," + Reg8[y];
                            break;
                        case 2:
                            asm = (q == 0 ? "SBC HL," : "ADC HL,") + Reg16[p];
                            break;
                        case 3:
                            string addr = Imm16();
                            asm = q == 0 ? "LD (" + addr + ")," + Reg16[p]
                                : "LD " + Reg16[p] + ",(" + addr + ")";
                            break;
                        case 4:
                            if (y != 0) return null;
                            asm = "NEG";
                            break;
                        case 5:
                            if (y > 1) return null;
                            asm = y == 1 ? "RETI" : "RETN";
                            flow = "return";
                            break;
                        case 6:
                            if (y is not (0 or 2 or 3)) return null;
                            asm = "IM " + (y == 0 ? 0 : y - 1);
                            break;
                        case 7:
                            asm = y switch
                            {
                                0 => "LD I,A", 1 => "LD R,A", 2 => "LD A,I",
                                3 => "LD A,R", 4 => "RRD", 5 => "RLD", _ => ""
                            };
                            if (asm.Length == 0) return null;
                            break;
                    }
                }
                else if (x == 2 && y >= 4 && z <= 3)
                {
                    // Repeating ED block ops reexecute at the same PC until
                    // their terminating condition is satisfied.
                    if (y >= 6) flow = "repeat";
                    string[] prefix = { "LD", "CP", "IN", "OT" };
                    string[] suffix = { "I", "D", "IR", "DR" };
                    if (z == 2) // INI IND INIR INDR
                        asm = "IN" + suffix[y - 4];
                    else if (z == 3) // OUTI OUTD OTIR OTDR
                        asm = y < 6 ? "OUT" + suffix[y - 4] : "OT" + suffix[y - 4];
                    else
                        asm = prefix[z] + suffix[y - 4];
                }
                else return null;
            }
            else
            {
                int x = first >> 6, y = (first >> 3) & 7, z = first & 7;
                int p = y >> 1, q = y & 1;
                switch (x)
                {
                    case 0:
                        switch (z)
                        {
                            case 0:
                                if (y == 0) asm = "NOP";
                                else if (y == 1) asm = "EX AF,AF'";
                                else if (y == 2)
                                {
                                    target = Relative();
                                    asm = "DJNZ $" + target.Value.ToString("X4");
                                    flow = "conditional";
                                }
                                else if (y == 3)
                                {
                                    target = Relative();
                                    asm = "JR $" + target.Value.ToString("X4");
                                    flow = "jump";
                                }
                                else
                                {
                                    target = Relative();
                                    asm = "JR " + Conditions[y - 4] + ",$" + target.Value.ToString("X4");
                                    flow = "conditional";
                                }
                                break;
                            case 1:
                                asm = q == 0 ? "LD " + Reg16[p] + "," + Imm16()
                                    : "ADD HL," + Reg16[p];
                                break;
                            case 2:
                                if (p == 0) asm = q == 0 ? "LD (BC),A" : "LD A,(BC)";
                                else if (p == 1) asm = q == 0 ? "LD (DE),A" : "LD A,(DE)";
                                else
                                {
                                    string addr = Imm16();
                                    string reg = p == 2 ? "HL" : "A";
                                    asm = q == 0 ? "LD (" + addr + ")," + reg
                                        : "LD " + reg + ",(" + addr + ")";
                                }
                                break;
                            case 3:
                                asm = (q == 0 ? "INC " : "DEC ") + Reg16[p];
                                break;
                            case 4: asm = "INC " + Reg8[y]; break;
                            case 5: asm = "DEC " + Reg8[y]; break;
                            case 6: asm = "LD " + Reg8[y] + "," + Imm8(); break;
                            default: asm = Accumulator[y]; break;
                        }
                        break;
                    case 1:
                        if (first == 0x76)
                        {
                            asm = "HALT"; flow = "halt";
                        }
                        else asm = "LD " + Reg8[y] + "," + Reg8[z];
                        break;
                    case 2:
                        asm = Alu[y] + Reg8[z];
                        break;
                    default:
                        switch (z)
                        {
                            case 0:
                                asm = "RET " + Conditions[y];
                                flow = "conditional-return";
                                break;
                            case 1:
                                if (q == 0) asm = "POP " + Reg16Stack[p];
                                else
                                    switch (p)
                                    {
                                        case 0: asm = "RET"; flow = "return"; break;
                                        case 1: asm = "EXX"; break;
                                        case 2: asm = "JP (HL)"; flow = "indirect-jump"; break;
                                        default: asm = "LD SP,HL"; break;
                                    }
                                break;
                            case 2:
                                target = Word();
                                asm = "JP " + Conditions[y] + ",$" + target.Value.ToString("X4");
                                flow = "conditional";
                                break;
                            case 3:
                                switch (y)
                                {
                                    case 0:
                                        target = Word();
                                        asm = "JP $" + target.Value.ToString("X4");
                                        flow = "jump";
                                        break;
                                    case 1: return null; // CB already decoded above
                                    case 2: asm = "OUT (" + Imm8() + "),A"; break;
                                    case 3: asm = "IN A,(" + Imm8() + ")"; break;
                                    case 4: asm = "EX (SP),HL"; break;
                                    case 5: asm = "EX DE,HL"; break;
                                    case 6: asm = "DI"; break;
                                    default: asm = "EI"; break;
                                }
                                break;
                            case 4:
                                target = Word();
                                asm = "CALL " + Conditions[y] + ",$" + target.Value.ToString("X4");
                                flow = "conditional-call";
                                break;
                            case 5:
                                if (q == 0) asm = "PUSH " + Reg16Stack[p];
                                else if (p == 0)
                                {
                                    target = Word();
                                    asm = "CALL $" + target.Value.ToString("X4");
                                    flow = "call";
                                }
                                else return null; // DD, ED, FD prefixes
                                break;
                            case 6: asm = Alu[y] + Imm8(); break;
                            case 7:
                                target = (ushort)(y * 8);
                                asm = "RST $" + target.Value.ToString("X2");
                                flow = "call";
                                break;
                        }
                        break;
                }
            }

            GuestSourceRead? source = memory.TryReadExact("sms.cpu", view, address, used);
            // CPU PC wraps at 0xFFFF; the current source map must not
            // invent a contiguous physical interval across this wrap.
            if (source is null) return null;
            return new Z80Instruction(address, asm, used,
                Convert.ToHexString(data.ToArray()), flow, target, source.Fragments);
        }
        catch (EndOfStreamException)
        {
            return null;
        }
    }
}

public sealed record Z80Edge(ushort From, ushort To, string Kind);
public sealed record Z80Unknown(ushort Address, string Reason);
public sealed record Z80Graph(string SourceSha256, string ViewId,
    IReadOnlyList<Z80Instruction> Instructions,
    IReadOnlyList<Z80Edge> Edges, IReadOnlyList<Z80Unknown> Unknowns, bool Truncated);

public static class Z80FlowAnalysis
{
    public static Z80Graph Analyze(GuestSourceMemory memory, string view,
        IEnumerable<ushort> seeds, int maxInstructions = 50_000)
    {
        ArgumentNullException.ThrowIfNull(memory);
        ArgumentNullException.ThrowIfNull(seeds);
        if (maxInstructions is < 1 or > 100_000)
            throw new ArgumentOutOfRangeException(nameof(maxInstructions));

        var queue = new Queue<ushort>(seeds.Distinct());
        var seen = new HashSet<ushort>();
        var code = new List<Z80Instruction>();
        var edges = new List<Z80Edge>();
        var unknowns = new List<Z80Unknown>();
        bool capped = false;
        var occupied = new Dictionary<ushort, ushort>();
        while (queue.Count != 0)
        {
            if (code.Count >= maxInstructions) { capped = true; break; }
            ushort address = queue.Dequeue();
            if (!seen.Add(address)) continue;
            Z80Instruction? instruction = Z80Decoder.TryDecode(memory, view, address);
            if (instruction is null)
            {
                unknowns.Add(new Z80Unknown(address,
                    memory.TryReadExact("sms.cpu", view, address, 1) is null
                        ? "unmapped-source" : "unsupported-or-truncated-opcode"));
                continue;
            }
            bool overlap = false;
            for (int i = 0; i < instruction.Length; i++)
                if (occupied.ContainsKey((ushort)(instruction.Address + i)))
                    overlap = true;
            if (overlap)
            {
                unknowns.Add(new Z80Unknown(address, "overlapping-instruction-candidates"));
                continue;
            }
            for (int i = 0; i < instruction.Length; i++)
                occupied[(ushort)(instruction.Address + i)] = address;
            code.Add(instruction);
            ushort next = (ushort)(address + instruction.Length);
            void Follow(ushort target, string type)
            {
                edges.Add(new Z80Edge(address, target, type));
                if (!seen.Contains(target)) queue.Enqueue(target);
            }
            switch (instruction.FlowKind)
            {
                case "conditional":
                case "conditional-call":
                    Follow(instruction.DirectTarget!.Value,
                        instruction.FlowKind == "conditional" ? "branch-taken" : "call-taken");
                    Follow(next, "not-taken-or-return-candidate");
                    break;
                case "conditional-return":
                    unknowns.Add(new Z80Unknown(address, "conditional-return-stack-target"));
                    Follow(next, "return-not-taken");
                    break;
                case "jump":
                case "call":
                    Follow(instruction.DirectTarget!.Value, instruction.FlowKind);
                    if (instruction.FlowKind == "call")
                        Follow(next, "return-continuation-candidate");
                    break;
                case "indirect-jump":
                    unknowns.Add(new Z80Unknown(address, "indirect-jump-register-target"));
                    break;
                case "repeat":
                    Follow(address, "block-repeat");
                    Follow(next, "block-exit");
                    break;
                case "return":
                case "halt":
                    break;
                default:
                    Follow(next, "fallthrough");
                    break;
            }
        }
        return new Z80Graph(memory.SourceSha256, view,
            code.OrderBy(i => i.Address).ToArray(), edges.ToArray(), unknowns.ToArray(), capped);
    }
}
