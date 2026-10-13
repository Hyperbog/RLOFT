using System.Buffers.Binary;
using DosDis.Core;

/// <summary>
/// Structurally representative, fully synthetic MZ-bound LE loader stress
/// test. No copyrighted bytes or original fixup lists are embedded. Geometry
/// and record counts mirror public metadata for both supplied source sizes.
/// </summary>
internal static class LargeLeOpenSmoke
{
    private static void Check(bool condition, string message)
    {
        if (!condition) throw new Exception("Large LE open: " + message);
    }

    private static void Dword(byte[] data, int offset, uint value) =>
        BinaryPrimitives.WriteUInt32LittleEndian(data.AsSpan(offset, 4), value);

    private static void Word(byte[] data, int offset, ushort value) =>
        BinaryPrimitives.WriteUInt16LittleEndian(data.AsSpan(offset, 2), value);

    private static BinaryArtifact MakeFixture(bool second)
    {
        int length = second ? 558689 : 277787;
        uint pages = second ? 78u : 24u;
        uint firstObjectPages = second ? 59u : 18u;
        uint secondObjectPages = second ? 19u : 6u;
        uint objectCodeBytes = second ? 0x3A852u : 0x11302u;
        uint objectDataBytes = second ? 0x2DD10u : 0x9E90u;
        uint entry = second ? 0x353A0u : 0xCC1Cu;
        uint lastPage = second ? 1853u : 1527u;
        uint dataPagesRelative = second ? 86528u : 27136u;
        uint fixPageRelative = second ? 565u : 349u;
        uint fixRecordRelative = second ? 881u : 449u;
        int fixups = second ? 9813 : 2617;
        int nested = 0x25D24, header = 0x27BDC;
        var data = new byte[length];
        data[0] = (byte)'M';
        data[1] = (byte)'Z'; // Outer MZ is not independently decoded.
        data[nested] = (byte)'M';
        data[nested + 1] = (byte)'Z';
        Dword(data, nested + 0x3C, (uint)(header - nested));
        data[header] = (byte)'L';
        data[header + 1] = (byte)'E';
        Dword(data, header + 0x14, pages);
        Dword(data, header + 0x18, 1);
        Dword(data, header + 0x1C, entry);
        Dword(data, header + 0x28, 4096);
        Dword(data, header + 0x2C, lastPage);
        Dword(data, header + 0x40, 0xC4);
        Dword(data, header + 0x44, 2);
        Dword(data, header + 0x48, 0xF4);
        Dword(data, header + 0x68, fixPageRelative);
        Dword(data, header + 0x6C, fixRecordRelative);
        Dword(data, header + 0x80, dataPagesRelative);

        static void Object(byte[] data, int at, uint size, uint baseAddress,
            uint flags, uint firstPage, uint count)
        {
            Dword(data, at, size);
            Dword(data, at + 4, baseAddress);
            Dword(data, at + 8, flags);
            Dword(data, at + 12, firstPage);
            Dword(data, at + 16, count);
        }
        Object(data, header + 0xC4, objectCodeBytes, 0x10000, 0x2005,
            1, firstObjectPages);
        Object(data, header + 0xDC, objectDataBytes,
            second ? 0x50000u : 0x30000u, 0x2003,
            firstObjectPages + 1, secondObjectPages);

        int dataStart = checked(nested + (int)dataPagesRelative);
        for (int page = 0; page < pages; page++)
        {
            int at = header + 0xF4 + page * 4;
            uint ordinal = (uint)(page + 1);
            data[at] = (byte)(ordinal >> 16);
            data[at + 1] = (byte)(ordinal >> 8);
            data[at + 2] = (byte)ordinal;
            data[at + 3] = 0;
            int stored = page == pages - 1 ? (int)lastPage : 4096;
            int physical = checked(dataStart + page * 4096);
            Check(physical + stored <= data.Length, "synthetic page outside source");
            // The synthetic object is NOP/RET and zero data; NO original
            // machine code, table entries or source excerpts are present.
            if (page < firstObjectPages)
                data.AsSpan(physical, stored).Fill(0x90);
        }
        int endOfCode = checked(dataStart + (int)entry);
        data[endOfCode] = 0x90;
        data[endOfCode + 1] = 0xC3;

        int fixPage = checked(header + (int)fixPageRelative);
        int fixRecord = checked(header + (int)fixRecordRelative);
        int cursor = 0;
        for (int page = 0; page < pages; page++)
        {
            Dword(data, fixPage + page * 4, (uint)cursor);
            int amount = fixups / (int)pages +
                (page < fixups % pages ? 1 : 0);
            for (int i = 0; i < amount; i++)
            {
                int at = fixRecord + cursor;
                // Explicit unique unchained 32-bit internal reference at
                // this synthetic page's local byte offset.
                data[at] = 0x07;
                data[at + 1] = 0x00;
                Word(data, at + 2, checked((ushort)(i * 4)));
                data[at + 4] = 0x01;
                Word(data, at + 5, 0);
                cursor += 7;
            }
        }
        Dword(data, fixPage + (int)pages * 4, (uint)cursor);
        Check(fixRecord + cursor <= dataStart,
            "synthetic fixup region overlaps stored original-free pages");
        return BinaryArtifact.FromBytes(data, second ? "synthetic-code2-shape" : "synthetic-code1-shape");
    }

    public static void Run()
    {
        foreach (bool second in new[] { false, true })
        {
            BinaryArtifact artifact = MakeFixture(second);
            LeImage le = LeImage.Load(artifact);
            var detected = BinaryFormatDetector.Inspect(artifact, le);
            var intel = FileIntelligence.Inspect(artifact, le);
            var preview = Disassembly.Decode(le, (int)le.EntryObject,
                le.EntryOffset, 128);
            Check(detected.FormatId == "le", "nested LE was not identified");
            Check(le.Objects.Count == 2, "two separately bounded LE objects expected");
            Check(le.Relocations.Count == (second ? 9813 : 2617),
                "large original-free fixup table count changed");
            Check(le.Objects[0].VirtualSize == (second ? 0x3A852u : 0x11302u),
                "code object geometry changed");
            Check(le.EntryOffset == (second ? 0x353A0u : 0xCC1Cu),
                "entry offset changed");
            Check(preview.Count == 128, "bounded x86-32 preview missing");
            Check(preview.All(x => x.Valid), "original-free NOP/RET preview rejected");
            Check(intel.ContainsKey("entry_point") && intel.ContainsKey("architecture"),
                "large source file-intelligence missing");
            int entryPhysical = checked((int)(le.ToFileOffset(1, le.EntryOffset)
                ?? throw new Exception("no synthetic entry mapping")));
            var hex = RawHexPreview.Render(artifact, entryPhysical);
            var tokens = HexByteNavigation.MapRenderedBytes(hex);
            Check(HexByteNavigation.FindPhysical(tokens, entryPhysical) is not null,
                "large file hex cursor cannot resolve exact physical byte");
        }
        Console.WriteLine("PASS: original-free two-object large LE open, 2617/9813 fixups, " +
            "bounded preview/intelligence and source-backed hex navigation");
    }
}
