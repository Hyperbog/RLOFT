using System.Buffers.Binary;

namespace DosDis.Core;

/// <summary>Exact 16-bit general-purpose register names in x86 opcode order.</summary>
public enum Dos16WordRegister { AX, CX, DX, BX, SP, BP, SI, DI }

/// <summary>
/// Immutable per-region register hypotheses. A register is Unknown until a
/// locally validated transfer establishes a narrower value. No CPU flags,
/// memory, segment selectors or call conventions are inferred.
/// </summary>
public sealed class Dos16RegisterSnapshot
{
    private readonly Dos16AbstractValue[] _values;
    public string SourceSha256 { get; }
    public ushort Segment { get; }
    public ushort Origin { get; }
    public int FileOffset { get; }
    public int RegionByteLength { get; }
    public string MappingEvidence { get; }

    private Dos16RegisterSnapshot(Dos16CodeRegion region, Dos16AbstractValue[] values)
    {
        SourceSha256 = region.SourceSha256;
        Segment = region.Segment;
        Origin = region.Origin;
        FileOffset = region.FileOffset;
        RegionByteLength = region.ByteLength;
        MappingEvidence = region.MappingEvidence;
        _values = (Dos16AbstractValue[])values.Clone();
    }

    public static Dos16RegisterSnapshot UnknownFor(Dos16CodeRegion region)
    {
        ArgumentNullException.ThrowIfNull(region);
        return new Dos16RegisterSnapshot(region,
            Enumerable.Repeat(Dos16AbstractValue.Unknown, 8).ToArray());
    }

    internal bool Matches(Dos16CodeRegion region) =>
        SourceSha256.Equals(region.SourceSha256, StringComparison.OrdinalIgnoreCase) &&
        Segment == region.Segment && Origin == region.Origin &&
        FileOffset == region.FileOffset && RegionByteLength == region.ByteLength &&
        MappingEvidence == region.MappingEvidence;

    public Dos16AbstractValue Get(Dos16WordRegister register)
    {
        if (!Enum.IsDefined(register))
            throw new ArgumentOutOfRangeException(nameof(register));
        return _values[(int)register];
    }

    internal Dos16RegisterSnapshot Set(Dos16CodeRegion region, int register,
        Dos16AbstractValue value)
    {
        if (!Matches(region)) throw new InvalidDataException("Register state is from another code region.");
        if (register is < 0 or >= 8) throw new ArgumentOutOfRangeException(nameof(register));
        ArgumentNullException.ThrowIfNull(value);
        var copy = (Dos16AbstractValue[])_values.Clone();
        copy[register] = value;
        return new Dos16RegisterSnapshot(region, copy);
    }

    internal Dos16RegisterSnapshot KillAll(Dos16CodeRegion region) =>
        UnknownFor(region);
}

/// <summary>
/// Exactly one source-backed instruction transfer. Output is candidate
/// evidence only: no reachability, register state merge or runtime execution.
/// </summary>
public sealed record Dos16TransferResult(
    Dos16RegisterSnapshot After, string Status, bool FlagsPreserved,
    Dos16ValueOrigin? SourceOrigin, bool RegistersInvalidated);

public static class Dos16RegisterTransfer
{
    /// <summary>
    /// Decode and check original physical bytes before interpreting a small
    /// prefix-free subset. Unrecognized instructions invalidate all tracked
    /// registers and flag knowledge; they are not treated as harmless NOPs.
    /// </summary>
    public static Dos16TransferResult Apply(Dos16CodeRegion region,
        Dos16FlowInstruction instruction, Dos16RegisterSnapshot before)
    {
        ArgumentNullException.ThrowIfNull(region);
        ArgumentNullException.ThrowIfNull(instruction);
        ArgumentNullException.ThrowIfNull(before);
        if (!before.Matches(region))
            throw new InvalidDataException("Cannot transfer an unrelated source or load window.");
        if (!Dos16UniqueTransparentPath.IsOriginalDecoded(region, instruction))
            throw new InvalidDataException("Candidate instruction lacks exact original-byte/decoder proof.");

        var bytes = region.Source.Data.Slice(
            checked((int)instruction.SourceFileOffset), instruction.Length);
        byte opcode = bytes[0];

        Dos16TransferResult Result(Dos16RegisterSnapshot next, string status,
            bool flagsPreserved, Dos16ValueOriginKind? origin = null,
            bool invalidated = false) =>
            new(next, status, flagsPreserved, origin is null ? null :
                Dos16ValueOrigin.Create(region.SourceSha256,
                    instruction.SourceFileOffset, instruction.Address,
                    origin.Value), invalidated);

        Dos16TransferResult Set(int register, Dos16AbstractValue value,
            string status, bool flagsPreserved, Dos16ValueOriginKind origin) =>
            Result(before.Set(region, register, value), status, flagsPreserved, origin);

        Dos16TransferResult Unsupported() =>
            Result(before.KillAll(region), "unsupported-kill-all-registers-and-flags",
                flagsPreserved: false, invalidated: true);

        // For now, only the one-byte NOP is proven register/flag-neutral.
        if (opcode == 0x90 && bytes.Length == 1)
            return Result(before, "exact-nop-preserve", flagsPreserved: true);

        // MOV r16,imm16; no operand/address-size override or segment prefix.
        if (opcode is >= 0xB8 and <= 0xBF && bytes.Length == 3)
            return Set(opcode - 0xB8,
                Dos16AbstractValue.Constant(BinaryPrimitives.ReadUInt16LittleEndian(bytes.Slice(1, 2))),
                "exact-mov-word-immediate", true, Dos16ValueOriginKind.Immediate);

        // MOV r/m16,r16 or r16,r/m16 only when ModRM specifies registers.
        if (opcode is 0x89 or 0x8B && bytes.Length == 2 &&
            (bytes[1] & 0xC0) == 0xC0)
        {
            int encodedRm = bytes[1] & 7;
            int encodedReg = (bytes[1] >> 3) & 7;
            int dest = opcode == 0x89 ? encodedRm : encodedReg;
            int source = opcode == 0x89 ? encodedReg : encodedRm;
            return Set(dest, before.Get((Dos16WordRegister)source),
                "exact-mov-register-word", true, Dos16ValueOriginKind.RegisterCopy);
        }

        // Accumulator immediates, with 16-bit modular addition/subtraction.
        if (opcode is 0x05 or 0x2D && bytes.Length == 3)
        {
            var right = Dos16AbstractValue.Constant(
                BinaryPrimitives.ReadUInt16LittleEndian(bytes.Slice(1, 2)));
            var left = before.Get(Dos16WordRegister.AX);
            return Set(0, opcode == 0x05
                    ? Dos16AbstractValue.Add(left, right)
                    : Dos16AbstractValue.Subtract(left, right),
                "exact-accumulator-word-arithmetic", false,
                Dos16ValueOriginKind.Arithmetic);
        }

        // Only ModRM register operands. 83 sign-extends imm8 to a word.
        // 81 uses the full imm16. /0 ADD, /5 SUB, /7 CMP.
        if (opcode is 0x81 or 0x83 &&
            bytes.Length == (opcode == 0x81 ? 4 : 3) &&
            (bytes[1] & 0xC0) == 0xC0)
        {
            int operation = (bytes[1] >> 3) & 7;
            int dest = bytes[1] & 7;
            ushort immediate = opcode == 0x81
                ? BinaryPrimitives.ReadUInt16LittleEndian(bytes.Slice(2, 2))
                : unchecked((ushort)(short)(sbyte)bytes[2]);
            if (operation == 7)
                return Result(before, "exact-compare-flags-unknown", false);
            if (operation is 0 or 5)
            {
                var left = before.Get((Dos16WordRegister)dest);
                var right = Dos16AbstractValue.Constant(immediate);
                return Set(dest, operation == 0
                        ? Dos16AbstractValue.Add(left, right)
                        : Dos16AbstractValue.Subtract(left, right),
                    "exact-register-word-arithmetic", false,
                    Dos16ValueOriginKind.Arithmetic);
            }
        }

        if (opcode == 0x3D && bytes.Length == 3)
            return Result(before, "exact-compare-flags-unknown", false);

        // Direct, prefix-free relative control transfers do not modify
        // 16-bit general registers or flags. The CFG must separately prove
        // exact destinations and both sides of conditional branches.
        if ((bytes.Length == 2 &&
                (opcode is >= 0x70 and <= 0x7F || opcode == 0xEB)) ||
            (bytes.Length == 4 && opcode == 0x0F &&
                bytes[1] is >= 0x80 and <= 0x8F) ||
            (bytes.Length == 3 && opcode == 0xE9))
            return Result(before, "exact-direct-branch-preserve",
                flagsPreserved: true);

        // Includes byte-register aliases (AH/AL...), memory writes, strings,
        // CALL/RET, interrupts, prefixed opcodes, unknown flag consumers and
        // unmodeled operand widths. Conservative even when needlessly broad.
        return Unsupported();
    }
}
