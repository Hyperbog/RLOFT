using System.Buffers.Binary;

namespace DosDis.Core;

public sealed record LeObject(int Number, uint VirtualSize, uint PreferredBase, uint Flags,
    uint FirstPage, uint PageCount);
public sealed record LePage(int ObjectNumber, uint ObjectPage, uint PageOrdinal, byte Flags,
    long? FileOffset, int StoredLength);

/// <summary>Strict read-only loader for the simple LE page variants in the initial corpus.</summary>
public sealed class LeImage
{
    private readonly BinaryArtifact _artifact;
    private readonly IReadOnlyDictionary<int, List<LePage>> _pages;
    private readonly Lazy<IReadOnlyDictionary<int, byte[]>> _relocatedMemory;

    private LeImage(BinaryArtifact artifact, long moduleOrigin, long header,
        uint pageSize, uint lastPageSize, uint entryObject, uint entryOffset,
        IReadOnlyList<LeObject> objects, IReadOnlyDictionary<int, List<LePage>> pages,
        IReadOnlyList<LeRelocation> relocations)
    {
        _artifact = artifact;
        ModuleOrigin = moduleOrigin;
        HeaderOffset = header;
        PageSize = pageSize;
        LastPageSize = lastPageSize;
        EntryObject = entryObject;
        EntryOffset = entryOffset;
        Objects = objects;
        _pages = pages;
        Relocations = relocations;
        _relocatedMemory = new Lazy<IReadOnlyDictionary<int, byte[]>>(BuildRelocatedMemory);
    }

    public string ArtifactSha256 => _artifact.Sha256;
    public long ModuleOrigin { get; }
    public long HeaderOffset { get; }
    public uint PageSize { get; }
    public uint LastPageSize { get; }
    public uint EntryObject { get; }
    public uint EntryOffset { get; }
    public IReadOnlyList<LeObject> Objects { get; }
    public IReadOnlyList<LeRelocation> Relocations { get; }
    public IReadOnlyList<LePage> Pages => _pages.Values.SelectMany(x => x).ToArray();

    private static ushort U16(ReadOnlySpan<byte> data, long offset)
    {
        Require(data, offset, 2);
        return BinaryPrimitives.ReadUInt16LittleEndian(data.Slice(checked((int)offset), 2));
    }
    private static uint U32(ReadOnlySpan<byte> data, long offset)
    {
        Require(data, offset, 4);
        return BinaryPrimitives.ReadUInt32LittleEndian(data.Slice(checked((int)offset), 4));
    }
    private static void Require(ReadOnlySpan<byte> data, long offset, long length)
    {
        if (offset < 0 || length < 0 || offset > data.Length || length > data.Length - offset)
            throw new InvalidDataException($"Truncated/out-of-range executable structure at 0x{offset:X} (size {length}).");
    }

    public static LeImage Load(BinaryArtifact artifact)
    {
        var b = artifact.Data;
        if (b.Length < 0x40 || U16(b, 0) != 0x5A4D)
            throw new InvalidDataException("Not an MZ executable.");
        // DOS/4GW-bound executables can embed a second MZ and a nested LE.
        // Probe candidate MZ headers; require a bounded e_lfanew and LE\0\0 signature.
        var candidates = new List<(long Module, long Header)>();
        for (long m = 0; m <= b.Length - 0x40; ++m)
        {
            if (b[(int)m] != (byte)'M' || b[(int)m + 1] != (byte)'Z') continue;
            uint lfanew = U32(b, m + 0x3c);
            if (lfanew < 0x40 || lfanew > b.Length - m - 4) continue;
            long h = m + lfanew;
            if (b[(int)h] == (byte)'L' && b[(int)h + 1] == (byte)'E' &&
                b[(int)h + 2] == 0 && b[(int)h + 3] == 0)
                candidates.Add((m, h));
        }
        if (candidates.Count != 1)
            throw new InvalidDataException($"Expected one valid MZ/LE module, found {candidates.Count}; ambiguous and unsupported layouts are not guessed.");
        var (module, hdr) = candidates[0];
        uint pageCount = U32(b, hdr + 0x14);
        uint entryObject = U32(b, hdr + 0x18);
        uint entryOffset = U32(b, hdr + 0x1c);
        uint pageSize = U32(b, hdr + 0x28);
        uint lastPageSize = U32(b, hdr + 0x2c);
        uint objectTable = U32(b, hdr + 0x40);
        uint objectCount = U32(b, hdr + 0x44);
        uint pageTable = U32(b, hdr + 0x48);
        uint dataPages = U32(b, hdr + 0x80);
        if (pageSize is < 16 or > 65536 || (pageSize & (pageSize - 1)) != 0 ||
            pageCount is 0 or > 65536 || objectCount is 0 or > 4096 ||
            lastPageSize > pageSize || entryObject == 0 || entryObject > objectCount)
            throw new InvalidDataException("Unsupported LE header dimensions.");
        Require(b, hdr + objectTable, (long)objectCount * 24);
        Require(b, hdr + pageTable, (long)pageCount * 4);
        var objects = new List<LeObject>();
        var pages = new Dictionary<int, List<LePage>>();
        for (int i = 0; i < objectCount; i++)
        {
            long op = hdr + objectTable + i * 24L;
            var obj = new LeObject(i + 1, U32(b, op), U32(b, op + 4),
                U32(b, op + 8), U32(b, op + 12), U32(b, op + 16));
            if (obj.PageCount > 0 && (obj.FirstPage == 0 ||
                (ulong)obj.FirstPage + obj.PageCount - 1 > pageCount))
                throw new InvalidDataException($"Object {obj.Number} references invalid pages.");
            if ((ulong)obj.PageCount * pageSize > (ulong)obj.VirtualSize + pageSize - 1)
                throw new InvalidDataException($"Object {obj.Number} page geometry is inconsistent.");
            objects.Add(obj);
            var entries = new List<LePage>();
            for (uint p = 0; p < obj.PageCount; p++)
            {
                long pp = hdr + pageTable + (obj.FirstPage + p - 1L) * 4L;
                uint ordinal = (uint)(b[(int)pp] << 16 | b[(int)pp + 1] << 8 | b[(int)pp + 2]);
                byte flags = b[(int)pp + 3];
                // Other LE page encodings (iterated, invalid, compressed, etc.) are deferred.
                if (flags != 0 || ordinal == 0 || ordinal > pageCount)
                    throw new NotSupportedException($"LE page {obj.FirstPage + p} has unsupported map kind {flags} or ordinal {ordinal}.");
                int stored = checked((int)(ordinal == pageCount && lastPageSize != 0 ? lastPageSize : pageSize));
                long physical = checked(module + dataPages + (long)(ordinal - 1) * pageSize);
                Require(b, physical, stored);
                entries.Add(new LePage(obj.Number, p, ordinal, flags, physical, stored));
            }
            pages[obj.Number] = entries;
        }
        if (entryOffset >= objects[(int)entryObject - 1].VirtualSize)
            throw new InvalidDataException("Entry point falls outside its LE object.");
        var relocations = LeFixupReader.Parse(b.ToArray(), hdr, pageCount, objects);
        return new LeImage(artifact, module, hdr, pageSize, lastPageSize,
            entryObject, entryOffset, objects, pages, relocations);
    }

    public long? ToFileOffset(int objectNumber, uint offset)
    {
        if (objectNumber < 1 || objectNumber > Objects.Count)
            throw new ArgumentOutOfRangeException(nameof(objectNumber));
        var obj = Objects[objectNumber - 1];
        if (offset >= obj.VirtualSize) return null;
        int page = checked((int)(offset / PageSize));
        int inPage = checked((int)(offset % PageSize));
        var maps = _pages[objectNumber];
        if (page >= maps.Count || inPage >= maps[page].StoredLength) return null;
        return maps[page].FileOffset + inPage;
    }

    /// <summary>Read only file-backed bytes. Never invent code from virtual zero-fill.</summary>
    public byte[] ReadStoredBytes(int objectNumber, uint offset, int count)
    {
        if (count < 0 || count > 1024 * 1024)
            throw new ArgumentOutOfRangeException(nameof(count));
        var result = new byte[count];
        for (int i = 0; i < count; i++)
        {
            long? physical = ToFileOffset(objectNumber, checked(offset + (uint)i));
            if (physical is null)
                throw new InvalidDataException($"Object {objectNumber} offset 0x{offset + (uint)i:X} has no stored-file byte.");
            result[i] = _artifact.Data[(int)physical.Value];
        }
        return result;
    }
    /// <summary>Relocated memory view with zero-initialized virtual tails.
    /// The source file and ReadStoredBytes() always retain original bytes.</summary>
    public byte[] ReadRelocatedBytes(int objectNumber, uint offset, int count)
    {
        if (count < 0 || count > 1024 * 1024)
            throw new ArgumentOutOfRangeException(nameof(count));
        if (objectNumber < 1 || objectNumber > Objects.Count)
            throw new ArgumentOutOfRangeException(nameof(objectNumber));
        byte[] memory = _relocatedMemory.Value[objectNumber];
        if ((ulong)offset + (uint)count > (ulong)memory.Length)
            throw new InvalidDataException("Relocated memory request crosses object bounds.");
        return memory.AsSpan(checked((int)offset), count).ToArray();
    }

    private IReadOnlyDictionary<int, byte[]> BuildRelocatedMemory()
    {
        var memory = new Dictionary<int, byte[]>();
        foreach (var obj in Objects)
        {
            if (obj.VirtualSize > 32 * 1024 * 1024)
                throw new NotSupportedException("LE object exceeds prototype mapped-memory limit.");
            var mapped = new byte[checked((int)obj.VirtualSize)];
            foreach (var page in _pages[obj.Number])
            {
                if (page.FileOffset is null) continue;
                long start = checked((long)page.ObjectPage * PageSize);
                int size = checked((int)Math.Min((long)page.StoredLength, (long)obj.VirtualSize - start));
                if (size < 0)
                    throw new InvalidDataException("LE stored page starts beyond its object.");
                _artifact.Data.Slice(checked((int)page.FileOffset.Value), size)
                    .CopyTo(mapped.AsSpan(checked((int)start), size));
            }
            memory.Add(obj.Number, mapped);
        }
        // Duplicate cross-page records are valid if and only if they write
        // identical bytes. Conflicting relocation evidence is a hard failure.
        var patchedBytes = new Dictionary<(int Object, long Offset), byte>();
        foreach (var fixup in Relocations)
        {
            var destination = Objects[fixup.TargetObject - 1];
            uint target = unchecked(destination.PreferredBase + fixup.TargetOffset + fixup.Additive);
            if (fixup.SourceType == 0x08)
            {
                uint sourceLinear = unchecked(Objects[fixup.SourceObject - 1].PreferredBase + (uint)fixup.SourceOffset);
                target = unchecked(target - sourceLinear - 4);
            }
            for (int j = 0; j < 4; j++)
            {
                byte value = (byte)(target >> (j * 8));
                var key = (fixup.SourceObject, fixup.SourceOffset + j);
                if (patchedBytes.TryGetValue(key, out byte prior) && prior != value)
                    throw new InvalidDataException($"Conflicting LE fixups at object {key.Item1} + 0x{key.Item2:X}.");
                patchedBytes[key] = value;
                memory[fixup.SourceObject][checked((int)(fixup.SourceOffset + j))] = value;
            }
        }
        return memory;
    }
}
