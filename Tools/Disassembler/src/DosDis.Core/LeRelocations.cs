using System.Buffers.Binary;

namespace DosDis.Core;

/// <summary>
/// A fully addressed internal 32-bit LE fixup. Source offsets are signed in the
/// original page record, and may point back into the preceding logical page.
/// Object offsets are independent from physical file offsets.
/// </summary>
public sealed record LeRelocation(
    int SourcePage, int SourceObject, long SourceOffset,
    byte SourceType, byte TargetFlags, int TargetObject, uint TargetOffset,
    uint Additive, long RecordFileOffset);

internal static class LeFixupReader
{
    private static ushort U16(ReadOnlySpan<byte> b, long p)
    {
        Guard(b, p, 2);
        return BinaryPrimitives.ReadUInt16LittleEndian(b.Slice(checked((int)p), 2));
    }
    private static uint U32(ReadOnlySpan<byte> b, long p)
    {
        Guard(b, p, 4);
        return BinaryPrimitives.ReadUInt32LittleEndian(b.Slice(checked((int)p), 4));
    }
    private static void Guard(ReadOnlySpan<byte> b, long p, long length)
    {
        if (p < 0 || length < 0 || p > b.Length || length > b.Length - p)
            throw new InvalidDataException($"Invalid LE fixup boundary at 0x{p:X} (+{length}).");
    }

    public static IReadOnlyList<LeRelocation> Parse(
        byte[] b, long header, uint pageCount,
        IReadOnlyList<LeObject> objects)
    {
        uint pageTableRelative = U32(b, header + 0x68);
        uint recordTableRelative = U32(b, header + 0x6c);
        if (pageTableRelative == 0 && recordTableRelative == 0)
            return Array.Empty<LeRelocation>();
        if (pageTableRelative == 0 || recordTableRelative == 0)
            throw new InvalidDataException("Incomplete LE fixup table pointers.");
        long pageTable = checked(header + pageTableRelative);
        long recordTable = checked(header + recordTableRelative);
        Guard(b, pageTable, checked(((long)pageCount + 1) * 4));
        var boundaries = new uint[checked((int)pageCount + 1)];
        for (int i = 0; i < boundaries.Length; i++)
        {
            boundaries[i] = U32(b, pageTable + i * 4L);
            if (i > 0 && boundaries[i] < boundaries[i - 1])
                throw new InvalidDataException($"LE fixup page offsets decrease at page {i}.");
        }
        Guard(b, recordTable, boundaries[^1]);
        if (boundaries[^1] > 32 * 1024 * 1024)
            throw new InvalidDataException("LE fixup records exceed prototype size limit.");

        // The fixup-page table uses logical page indices, not physical page ordinals.
        var ownership = new Dictionary<int, (int Object, long Base, uint Size)>();
        foreach (var obj in objects)
        {
            for (uint p = 0; p < obj.PageCount; p++)
            {
                int pageIndex = checked((int)(obj.FirstPage + p));
                if (!ownership.TryAdd(pageIndex, (obj.Number, checked((long)p * U32(b, header + 0x28)), obj.VirtualSize)))
                    throw new InvalidDataException($"LE logical page {pageIndex} assigned to multiple objects.");
            }
        }
        var results = new List<LeRelocation>();
        for (int i = 0; i < (int)pageCount; i++)
        {
            long cursor = checked(recordTable + boundaries[i]);
            long end = checked(recordTable + boundaries[i + 1]);
            if (end == cursor) continue;
            if (!ownership.TryGetValue(i + 1, out var owner))
                throw new InvalidDataException($"Fixups reference unowned LE logical page {i + 1}.");
            while (cursor < end)
            {
                long recordAt = cursor;
                byte src = ReadByte();
                byte flags = ReadByte();
                byte srcType = (byte)(src & 0x0f);
                if ((src & ~0x2f) != 0 || (src & 0x10) != 0 ||
                    srcType is not (0x07 or 0x08) || (flags & 0x0b) != 0)
                    throw new NotSupportedException($"Unsupported LE fixup source/target at 0x{recordAt:X}: {src:X2}/{flags:X2}.");
                // P15 supports internal 32-bit absolute or self-relative offsets.
                // Selector, chained and import fixups must never be guessed.
                int count = (src & 0x20) != 0 ? ReadByte() : 1;
                short source = count == 1 && (src & 0x20) == 0 ? unchecked((short)ReadWord()) : (short)0;
                int targetObject = (flags & 0x40) != 0 ? ReadWord() : ReadByte();
                if (targetObject < 1 || targetObject > objects.Count)
                    throw new InvalidDataException($"LE fixup at 0x{recordAt:X} targets invalid object {targetObject}.");
                uint targetOffset = (flags & 0x10) != 0 ? ReadDword() : ReadWord();
                uint additive = (flags & 0x04) == 0 ? 0 : (flags & 0x20) != 0 ? ReadDword() : ReadWord();
                void Append(short pageOffset)
                {
                    long objectOffset = owner.Base + pageOffset;
                    if (objectOffset < 0 || objectOffset + 4 > owner.Size)
                        throw new InvalidDataException($"LE fixup at 0x{recordAt:X} points outside object {owner.Object}.");
                    results.Add(new LeRelocation(i + 1, owner.Object, objectOffset,
                        srcType, flags, targetObject, targetOffset, additive, recordAt));
                }
                if ((src & 0x20) != 0)
                    for (int k = 0; k < count; k++) Append(unchecked((short)ReadWord()));
                else Append(source);

                byte ReadByte()
                {
                    if (cursor >= end)
                        throw new InvalidDataException($"Truncated LE fixup record at 0x{recordAt:X}.");
                    return b[checked((int)cursor++)];
                }
                ushort ReadWord()
                {
                    if (end - cursor < 2)
                        throw new InvalidDataException($"Truncated LE fixup word at 0x{recordAt:X}.");
                    ushort value = U16(b, cursor); cursor += 2; return value;
                }
                uint ReadDword()
                {
                    if (end - cursor < 4)
                        throw new InvalidDataException($"Truncated LE fixup dword at 0x{recordAt:X}.");
                    uint value = U32(b, cursor); cursor += 4; return value;
                }
            }
            if (cursor != end)
                throw new InvalidDataException($"LE page {i + 1} fixup record list does not close.");
        }
        return results;
    }
}
