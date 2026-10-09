using System.Buffers.Binary;
using Iced.Intel;

namespace DosDis.Core;

public sealed record DosMzRelocation(ushort Segment, ushort Offset, long SourceFileOffset);
public sealed record DosMzDecodedInstruction(
    ushort Segment, uint Offset, long SourceFileOffset, string OriginalBytes,
    string Assembly, string FlowControl, bool Valid);

/// <summary>
/// Validates the on-disk MZ load module and reports original-byte addresses.
/// No PSP address, runtime segment base, BSS, or relocation values are guessed.
/// </summary>
public sealed class DosMzImage
{
    private readonly BinaryArtifact _original;
    private DosMzImage(BinaryArtifact artifact, int imageStart, int imageLength,
        int declaredFileBytes, ushort cs, ushort ip, ushort ss, ushort sp,
        IReadOnlyList<DosMzRelocation> relocations)
    {
        _original = artifact;
        LoadImageFileOffset = imageStart;
        FileBackedImageLength = imageLength;
        DeclaredExecutableFileLength = declaredFileBytes;
        InitialCs = cs; InitialIp = ip; InitialSs = ss; InitialSp = sp;
        Relocations = relocations;
    }

    public string SourceSha256 => _original.Sha256;
    public int LoadImageFileOffset { get; }
    public int FileBackedImageLength { get; }
    public int DeclaredExecutableFileLength { get; }
    public ushort InitialCs { get; }
    public ushort InitialIp { get; }
    public ushort InitialSs { get; }
    public ushort InitialSp { get; }
    public IReadOnlyList<DosMzRelocation> Relocations { get; }
    public bool HasTrailingOverlayData => DeclaredExecutableFileLength < _original.Length;

    public static DosMzImage Load(BinaryArtifact artifact)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        var bytes = artifact.Data;
        // A bound LE/LX/NE/PE module is not to be relabeled as a DOS
        // real-mode program merely because its wrapper begins with MZ.
        string type = BinaryFormatDetector.Inspect(artifact).FormatId;
        if (type != "mz")
            throw new NotSupportedException("A standalone MZ loader requires a DOS MZ header without an identified extended module.");
        if (bytes.Length < 0x1C)
            throw new InvalidDataException("Truncated MZ header.");
        ushort Read(int offset) => BinaryPrimitives.ReadUInt16LittleEndian(artifact.Data.Slice(offset, 2));
        ushort lastPageBytes = Read(2), pages = Read(4), relocationCount = Read(6);
        ushort headerParagraphs = Read(8), ss = Read(0x0E), sp = Read(0x10);
        ushort ip = Read(0x14), cs = Read(0x16), relocTable = Read(0x18);
        if (pages == 0 || lastPageBytes > 511 || headerParagraphs < 2)
            throw new InvalidDataException("Invalid MZ page or header geometry.");
        int declared = checked((pages - 1) * 512 + (lastPageBytes == 0 ? 512 : lastPageBytes));
        int headerBytes = checked(headerParagraphs * 16);
        if (headerBytes > declared || declared > bytes.Length)
            throw new InvalidDataException("MZ header, declared length, or original file is inconsistent.");
        if (relocationCount != 0 &&
            ((long)relocTable < 0x1C || (long)relocTable + relocationCount * 4 > headerBytes))
            throw new InvalidDataException("MZ relocation table exceeds the declared executable header.");
        int loadLength = declared - headerBytes;
        var fixups = new List<DosMzRelocation>(relocationCount);
        for (int i = 0; i < relocationCount; i++)
        {
            int position = relocTable + i * 4;
            ushort offset = Read(position), segment = Read(position + 2);
            long address = (long)segment * 16 + offset;
            if (address + 2 > loadLength)
                throw new InvalidDataException("MZ relocation points outside the file-backed load module.");
            fixups.Add(new DosMzRelocation(segment, offset, headerBytes + address));
        }
        long entry = (long)cs * 16 + ip;
        if (entry >= loadLength)
            throw new InvalidDataException("MZ initial CS:IP has no file-backed instruction byte; manual loader context required.");
        return new DosMzImage(artifact, headerBytes, loadLength, declared, cs, ip, ss, sp, fixups);
    }

    /// <summary>
    /// Returns null for unbacked or non-image offsets. The value is the
    /// original file offset; no inferred process segment base is included.
    /// </summary>
    public long? ToFileOffset(ushort segment, ushort offset)
    {
        long linear = (long)segment * 16 + offset;
        return linear < FileBackedImageLength ? LoadImageFileOffset + linear : null;
    }

    /// <summary>
    /// Analyst-requested 16-bit linear decode. Not proof of a function,
    /// reachable entry or DOS runtime relocation state.
    /// </summary>
    public IReadOnlyList<DosMzDecodedInstruction> Decode(
        ushort segment, ushort offset, int instructionCount = 32)
    {
        if (instructionCount is < 1 or > 1024)
            throw new ArgumentOutOfRangeException(nameof(instructionCount));
        long start = (long)segment * 16 + offset;
        if (start >= FileBackedImageLength)
            throw new ArgumentOutOfRangeException(nameof(offset), "No file-backed bytes at the requested 16-bit segmented address.");
        int available = checked((int)Math.Min(
            (long)FileBackedImageLength - start,
            Math.Min((long)instructionCount * 15, 0x10000L - offset)));
        byte[] data = _original.Data.Slice(checked((int)(LoadImageFileOffset + start)), available).ToArray();
        var reader = new ByteArrayCodeReader(data);
        var decoder = Decoder.Create(16, reader);
        decoder.IP = offset;
        var formatter = new NasmFormatter();
        formatter.Options.SpaceAfterOperandSeparator = true;
        var result = new List<DosMzDecodedInstruction>();
        var output = new StringOutput();
        int used = 0;
        while (result.Count < instructionCount && reader.CanReadByte)
        {
            decoder.Decode(out var instruction);
            if (instruction.Length <= 0 || used + instruction.Length > data.Length)
                break;
            formatter.Format(instruction, output);
            result.Add(new DosMzDecodedInstruction(segment, checked((uint)(offset + used)),
                LoadImageFileOffset + start + used,
                Convert.ToHexString(data.AsSpan(used, instruction.Length)),
                output.ToStringAndReset(), instruction.FlowControl.ToString(),
                instruction.Code != Code.INVALID));
            used += instruction.Length;
        }
        return result;
    }
}
