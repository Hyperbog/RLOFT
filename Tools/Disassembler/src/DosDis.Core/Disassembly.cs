using Iced.Intel;

namespace DosDis.Core;

public sealed record DecodedLine(uint ObjectOffset, string Bytes, string Assembly,
    string FlowControl, bool Valid);

public static class Disassembly
{
    public static IReadOnlyList<DecodedLine> Decode(LeImage image, int objectNumber,
        uint offset, int instructionCount)
    {
        if (instructionCount is < 1 or > 1024)
            throw new ArgumentOutOfRangeException(nameof(instructionCount));
        if (objectNumber < 1 || objectNumber > image.Objects.Count)
            throw new ArgumentOutOfRangeException(nameof(objectNumber));
        var obj = image.Objects[objectNumber - 1];
        if (offset >= obj.VirtualSize)
            throw new ArgumentOutOfRangeException(nameof(offset));
        // Prototype: 32-bit LE code objects only. Real-mode and 16-bit selectors
        // require an independently verified execution context and separate loader.
        if ((obj.Flags & 0x4) == 0)
            throw new NotSupportedException("Selected object is not marked executable.");
        int available = checked((int)Math.Min((ulong)instructionCount * 15,
            (ulong)obj.VirtualSize - offset));
        int backed = 0;
        while (backed < available && image.ToFileOffset(objectNumber, offset + (uint)backed) != null)
            backed++;
        if (backed == 0)
            throw new InvalidDataException("No file-backed bytes at requested address.");
        byte[] data = image.ReadStoredBytes(objectNumber, offset, backed);
        var reader = new ByteArrayCodeReader(data);
        var decoder = Decoder.Create(32, reader);
        decoder.IP = offset;
        var formatter = new NasmFormatter();
        formatter.Options.SpaceAfterOperandSeparator = true;
        var output = new StringOutput();
        var result = new List<DecodedLine>();
        int position = 0;
        while (result.Count < instructionCount && reader.CanReadByte)
        {
            decoder.Decode(out var instruction);
            if (instruction.Length <= 0 || position + instruction.Length > data.Length)
                break;
            formatter.Format(instruction, output);
            string asm = output.ToStringAndReset();
            result.Add(new DecodedLine(checked(offset + (uint)position),
                Convert.ToHexString(data.AsSpan(position, instruction.Length)),
                asm, instruction.FlowControl.ToString(), instruction.Code != Code.INVALID));
            position += instruction.Length;
        }
        return result;
    }
}
