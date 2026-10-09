using Iced.Intel;

namespace DosDis.Core;

/// <summary>
/// One 16-bit x86 instruction decoded on request from original file-backed
/// bytes; segment values are loader assumptions, never claimed runtime facts.
/// </summary>
public sealed record Dos16Instruction(
    ushort Segment, uint Offset, long SourceFileOffset, string OriginalBytes,
    string Assembly, string FlowControl, bool Valid);

public static class X86RealModeDecoder
{
    public static IReadOnlyList<Dos16Instruction> Decode(
        BinaryArtifact artifact, int physicalFileOffset, int backedLength,
        ushort segment, ushort instructionPointer, int count)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        if (count is < 1 or > 1024)
            throw new ArgumentOutOfRangeException(nameof(count));
        if (physicalFileOffset < 0 || backedLength < 1 ||
            (long)physicalFileOffset + backedLength > artifact.Length)
            throw new InvalidDataException("16-bit decode would read outside original backed bytes.");
        // Do not silently wrap an IP across the segment boundary or invent
        // memory beyond either the loader's validated backing or EOF.
        int available = checked((int)Math.Min(
            Math.Min((long)count * 15, backedLength),
            0x10000L - instructionPointer));
        byte[] source = artifact.Data.Slice(physicalFileOffset, available).ToArray();
        var reader = new ByteArrayCodeReader(source);
        var decoder = Decoder.Create(16, reader);
        decoder.IP = instructionPointer;
        var formatter = new NasmFormatter();
        formatter.Options.SpaceAfterOperandSeparator = true;
        var output = new StringOutput();
        var result = new List<Dos16Instruction>();
        int cursor = 0;
        while (result.Count < count && reader.CanReadByte)
        {
            decoder.Decode(out var instruction);
            if (instruction.Length <= 0 || cursor + instruction.Length > source.Length)
                break;
            formatter.Format(instruction, output);
            result.Add(new Dos16Instruction(segment,
                checked((uint)instructionPointer + (uint)cursor),
                physicalFileOffset + cursor,
                Convert.ToHexString(source.AsSpan(cursor, instruction.Length)),
                output.ToStringAndReset(), instruction.FlowControl.ToString(),
                instruction.Code != Code.INVALID));
            cursor += instruction.Length;
        }
        return result;
    }
}
