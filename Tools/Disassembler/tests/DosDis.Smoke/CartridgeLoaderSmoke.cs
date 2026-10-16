using DosDis.Core;

internal static class CartridgeLoaderSmoke
{
    private static void Same<T>(T actual, T expected, string label)
    {
        if (!EqualityComparer<T>.Default.Equals(actual, expected))
            throw new Exception("Cartridge " + label + ": unexpected value.");
    }

    private static void Reject(Action action, string label)
    {
        try { action(); }
        catch (InvalidDataException) { return; }
        catch (NotSupportedException) { return; }
        throw new Exception("Cartridge loader accepted " + label);
    }

    private static BinaryArtifact MakeNes(int mapper, int prgBanks,
        int chrBanks = 0, bool trainer = false)
    {
        int start = 16 + (trainer ? 512 : 0);
        var b = new byte[start + prgBanks * 16384 + chrBanks * 8192];
        b[0] = 0x4E; b[1] = 0x45; b[2] = 0x53; b[3] = 0x1A;
        b[4] = (byte)prgBanks;
        b[5] = (byte)chrBanks;
        b[6] = (byte)(((mapper & 15) << 4) | (trainer ? 4 : 0));
        b[7] = (byte)(mapper & 0xF0);
        for (int i = 0; i < prgBanks; i++)
            b.AsSpan(start + i * 16384, 16384).Fill((byte)(0x20 + i));
        if (chrBanks > 0) b.AsSpan(start + prgBanks * 16384).Fill(0xAA);
        return BinaryArtifact.FromBytes(b);
    }

    private static BinaryArtifact MakeSms(int banks)
    {
        var b = new byte[banks * 16384];
        for (int i = 0; i < banks; i++) b.AsSpan(i * 16384, 16384).Fill((byte)(0x40 + i));
        "TMR SEGA"u8.CopyTo(b.AsSpan(0x3FF0));
        return BinaryArtifact.FromBytes(b);
    }

    public static void Run()
    {
        var nrom = NesCartridge.Load(MakeNes(0, 1, 1, true));
        Same(nrom.PrgPhysicalStart, 528, "trainer shifts PRG start");
        Same(nrom.ChrPhysicalStart, 528 + 16384, "CHR source offset");
        Same(nrom.HasTrainer, true, "trainer flag");
        var (nromMemory, nromView) = nrom.CreateCpuView();
        Same(nromMemory.TryReadExact("nes.cpu", nromView, 0x8000, 1)!.Bytes[0],
            (byte)0x20, "NROM 8000 lower");
        Same(nromMemory.TryReadExact("nes.cpu", nromView, 0xC000, 1)!.Bytes[0],
            (byte)0x20, "NROM 16KiB mirror");
        Same(nromMemory.TryReadExact("nes.cpu", nromView, 0x7000, 1) is null,
            true, "trainer RAM not reclassified as ROM");
        var nrom32 = NesCartridge.Load(MakeNes(0, 2));
        Same(nrom32.CreateCpuView().Memory.TryReadExact("nes.cpu", "nrom", 0xC000, 1)!.Bytes[0],
            (byte)0x21, "NROM 32KiB upper");
        var uxrom = NesCartridge.Load(MakeNes(2, 4));
        Same(uxrom.CreateCpuView(2).Memory.TryReadExact("nes.cpu", "uxrom-bank-2", 0x8000, 1)!.Bytes[0],
            (byte)0x22, "UxROM selected lower bank");
        Same(uxrom.CreateCpuView(2).Memory.TryReadExact("nes.cpu", "uxrom-bank-2", 0xC000, 1)!.Bytes[0],
            (byte)0x23, "UxROM fixed last bank");
        Same(uxrom.CreateCpuView(0).Memory.TryReadExact("nes.cpu", "uxrom-bank-0", 0x8000, 1)!.Bytes[0],
            (byte)0x20, "UxROM other selected bank");

        byte[] vectorRom = MakeNes(0, 1).Data.ToArray();
        vectorRom[^6] = 0x34; vectorRom[^5] = 0x12;
        vectorRom[^4] = 0x78; vectorRom[^3] = 0x56;
        vectorRom[^2] = 0xBC; vectorRom[^1] = 0x9A;
        var vectors = NesCartridge.Load(BinaryArtifact.FromBytes(vectorRom)).ReadVectors();
        Same(vectors?.Nmi, (ushort)0x1234, "NMI little endian");
        Same(vectors?.Reset, (ushort)0x5678, "reset little endian");
        Same(vectors?.Irq, (ushort)0x9ABC, "IRQ little endian");
        Reject(() => NesCartridge.Load(MakeNes(4, 4)), "unsupported MMC3 mapping");
        Reject(() => NesCartridge.Load(MakeNes(0, 3)), "invalid NROM PRG geometry");
        byte[] nes2 = MakeNes(0, 1).Data.ToArray(); nes2[7] = 8;
        Reject(() => NesCartridge.Load(BinaryArtifact.FromBytes(nes2)), "NES 2.0 as iNES 1.0");
        byte[] corrupt = MakeNes(0, 1).Data.ToArray(); Array.Resize(ref corrupt, corrupt.Length - 1);
        Reject(() => NesCartridge.Load(BinaryArtifact.FromBytes(corrupt)), "truncated PRG");
        byte[] extra = MakeNes(0, 1).Data.ToArray(); Array.Resize(ref extra, extra.Length + 1);
        Reject(() => NesCartridge.Load(BinaryArtifact.FromBytes(extra)), "unmodeled appended payload");

        SmsCartridge sms = SmsCartridge.Load(MakeSms(4));
        Same(sms.HeaderOffset, 0x3FF0, "SMS header location");
        var (sega, segaView) = sms.CreateSegaCpuView(2, 1, 3);
        Same(sega.TryReadExact("sms.cpu", segaView, 0x0000, 1)!.Bytes[0],
            (byte)0x40, "Sega mapper first KiB fixed");
        Same(sega.TryReadExact("sms.cpu", segaView, 0x0400, 1)!.Bytes[0],
            (byte)0x42, "Sega mapper slot 0 remainder");
        Same(sega.TryReadExact("sms.cpu", segaView, 0x4000, 1)!.Bytes[0],
            (byte)0x41, "Sega mapper slot 1");
        Same(sega.TryReadExact("sms.cpu", segaView, 0x8000, 1)!.Bytes[0],
            (byte)0x43, "Sega mapper slot 2");
        var (codemasters, cmView) = sms.CreateCodemastersCpuView(2, 1, 0);
        Same(codemasters.TryReadExact("sms.cpu", cmView, 0, 1)!.Bytes[0],
            (byte)0x42, "Codemasters does not preserve first KiB");
        Same(sega.TryReadExact("sms.cpu", segaView, 0xC000, 1) is null,
            true, "SMS CPU RAM never fabricated from ROM");
        bool invalidSlot = false;
        try { sms.CreateSegaCpuView(0, 1, 4); }
        catch (ArgumentOutOfRangeException) { invalidSlot = true; }
        Same(invalidSlot, true, "reject invalid SMS page selection");
        byte[] headerless = MakeSms(4).Data.ToArray();
        headerless.AsSpan(0x3FF0, 8).Clear();
        Reject(() => SmsCartridge.Load(BinaryArtifact.FromBytes(headerless)),
            "unverified SMS headerless ROM");
        Reject(() => SmsCartridge.Load(BinaryArtifact.FromBytes(new byte[0x4001])),
            "misaligned ROM / unknown copier header");
        Console.WriteLine("PASS: verified iNES NROM/UxROM mapping, trainer/CHR/vector offsets, " +
            "explicit SMS Sega/Codemasters paging and malformed/unsupported rejection");
    }
}
