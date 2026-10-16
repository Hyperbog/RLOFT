namespace DosDis.Core;

/// <summary>
/// Conservative iNES 1.0 loader. This is an original-byte layout reader,
/// not a cycle-accurate mapper or console emulator.
/// </summary>
public sealed class NesCartridge
{
    private readonly BinaryArtifact _source;
    private readonly int _prgOffset;

    private NesCartridge(BinaryArtifact source, int prgOffset,
        int prgBanks, int chrBanks, int mapper, bool trainer, bool battery, bool fourScreen)
    {
        _source = source;
        _prgOffset = prgOffset;
        PrgBanks = prgBanks;
        ChrBanks = chrBanks;
        Mapper = mapper;
        HasTrainer = trainer;
        HasBattery = battery;
        FourScreen = fourScreen;
    }

    public string SourceSha256 => _source.Sha256;
    public int Mapper { get; }
    public int PrgBanks { get; }
    public int ChrBanks { get; }
    public bool HasTrainer { get; }
    public bool HasBattery { get; }
    public bool FourScreen { get; }
    public int PrgPhysicalStart => _prgOffset;
    public int ChrPhysicalStart => _prgOffset + PrgBanks * 16384;

    public static NesCartridge Load(BinaryArtifact artifact)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        ReadOnlySpan<byte> bytes = artifact.Data;
        if (bytes.Length < 16 || !bytes[..4].SequenceEqual(new byte[] { 0x4E, 0x45, 0x53, 0x1A }))
            throw new InvalidDataException("Not a complete iNES ROM header.");
        byte flags6 = bytes[6], flags7 = bytes[7];
        // NES 2.0 uses a different size/mapper grammar. Do not reinterpret
        // it as iNES 1.0 or derive false cartridge addresses.
        if ((flags7 & 0x0C) != 0 || (flags7 & 0x03) != 0 ||
            bytes.Slice(12, 4).IndexOfAnyExcept((byte)0) >= 0)
            throw new NotSupportedException(
                "NES 2.0, archaic/ambiguous, VS, and PlayChoice headers require a separate validated loader.");
        int mapper = (flags6 >> 4) | (flags7 & 0xF0);
        int prg = bytes[4], chr = bytes[5];
        bool trainer = (flags6 & 4) != 0;
        int offset = 16 + (trainer ? 512 : 0);
        long expected = (long)offset + (long)prg * 16384 + (long)chr * 8192;
        if (prg == 0 || expected != artifact.Length)
            throw new InvalidDataException("iNES PRG/CHR/trainer size does not match the exact original file.");
        if (mapper == 0 && prg is not (1 or 2))
            throw new NotSupportedException("NROM requires exactly one or two 16 KiB PRG banks.");
        if (mapper == 2 && prg < 2)
            throw new NotSupportedException("UxROM requires at least two 16 KiB PRG banks.");
        if (mapper is not (0 or 2))
            throw new NotSupportedException("NES mapper is recognized but no source-backed CPU view is implemented.");
        return new NesCartridge(artifact, offset, prg, chr, mapper,
            trainer, (flags6 & 2) != 0, (flags6 & 8) != 0);
    }

    /// <summary>
    /// Return a static ROM-only CPU view. Mapper 2's switchable bank must be
    /// supplied explicitly; no claim is made about bank writes or reset state.
    /// RAM, PPU, I/O and trainer-initialized RAM are intentionally unmapped.
    /// </summary>
    public (GuestSourceMemory Memory, string ViewId) CreateCpuView(int selectedBank = 0)
    {
        if (Mapper == 0 && selectedBank != 0)
            throw new ArgumentOutOfRangeException(nameof(selectedBank));
        if (selectedBank < 0 || selectedBank >= PrgBanks)
            throw new ArgumentOutOfRangeException(nameof(selectedBank));
        string view = Mapper == 0 ? "nrom" : "uxrom-bank-" + selectedBank;
        int upper = Mapper == 0 && PrgBanks == 1 ? 0 : PrgBanks - 1;
        if (Mapper == 0 && PrgBanks == 2) upper = 1;
        var windows = new[]
        {
            new GuestSourceWindow("nes.cpu", view, 0x8000, 0x4000,
                _prgOffset + (long)selectedBank * 16384),
            new GuestSourceWindow("nes.cpu", view, 0xC000, 0x4000,
                _prgOffset + (long)upper * 16384)
        };
        return (new GuestSourceMemory(_source, windows), view);
    }

    public (ushort Nmi, ushort Reset, ushort Irq)? ReadVectors(int selectedBank = 0)
    {
        (GuestSourceMemory memory, string view) = CreateCpuView(selectedBank);
        GuestSourceRead? bytes = memory.TryReadExact("nes.cpu", view, 0xFFFA, 6);
        if (bytes is null) return null;
        static ushort Word(byte[] b, int at) => (ushort)(b[at] | (b[at + 1] << 8));
        return (Word(bytes.Bytes, 0), Word(bytes.Bytes, 2), Word(bytes.Bytes, 4));
    }
}
