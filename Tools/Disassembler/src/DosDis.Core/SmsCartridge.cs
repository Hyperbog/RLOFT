namespace DosDis.Core;

/// <summary>
/// Raw SMS/Game Gear ROM geometry with explicitly requested paging profiles.
/// A TMR SEGA signature is evidence, not proof of which hardware mapper a game uses.
/// </summary>
public sealed class SmsCartridge
{
    private readonly BinaryArtifact _source;

    private SmsCartridge(BinaryArtifact source, int headerOffset)
    {
        _source = source;
        HeaderOffset = headerOffset;
    }

    public string SourceSha256 => _source.Sha256;
    public int HeaderOffset { get; }
    public int Banks => _source.Length / 16384;

    public static SmsCartridge Load(BinaryArtifact artifact)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        ReadOnlySpan<byte> bytes = artifact.Data;
        // No implicit 512-byte copier-header stripping: it changes every
        // original physical address and requires a separate validated adapter.
        if (bytes.Length < 16384 || bytes.Length % 16384 != 0)
            throw new InvalidDataException("SMS cartridge must be an aligned, complete 16 KiB-bank source.");
        int header = -1;
        foreach (int at in new[] { 0x1FF0, 0x3FF0, 0x7FF0 })
        {
            if (at + 16 <= bytes.Length && bytes.Slice(at, 8).SequenceEqual("TMR SEGA"u8))
                header = at;
        }
        if (header < 0)
            throw new NotSupportedException(
                "No documented TMR SEGA signature; headerless and alternate cartridge formats need explicit identification.");
        return new SmsCartridge(artifact, header);
    }

    /// <summary>
    /// The Sega mapper keeps CPU 0000-03FF in bank 0, while the other
    /// 15 KiB of slot 0 and both 16 KiB slots are paged separately.
    /// Caller must specify slot state (never infer BIOS/reset behavior).
    /// </summary>
    public (GuestSourceMemory Memory, string ViewId) CreateSegaCpuView(
        int slot0, int slot1, int slot2)
    {
        CheckBank(slot0);
        CheckBank(slot1);
        CheckBank(slot2);
        string view = $"sega-{slot0}-{slot1}-{slot2}";
        var windows = new[]
        {
            new GuestSourceWindow("sms.cpu", view, 0x0000, 0x0400, 0),
            new GuestSourceWindow("sms.cpu", view, 0x0400, 0x3C00,
                (long)slot0 * 16384 + 0x400),
            new GuestSourceWindow("sms.cpu", view, 0x4000, 0x4000,
                (long)slot1 * 16384),
            new GuestSourceWindow("sms.cpu", view, 0x8000, 0x4000,
                (long)slot2 * 16384)
        };
        return (new GuestSourceMemory(_source, windows), view);
    }

    /// <summary>
    /// Codemasters page mapping has no fixed first KiB. This static ROM view
    /// does not model optional SRAM overlays or mapper register writes.
    /// </summary>
    public (GuestSourceMemory Memory, string ViewId) CreateCodemastersCpuView(
        int slot0, int slot1, int slot2)
    {
        CheckBank(slot0);
        CheckBank(slot1);
        CheckBank(slot2);
        string view = $"codemasters-{slot0}-{slot1}-{slot2}";
        var windows = new[]
        {
            new GuestSourceWindow("sms.cpu", view, 0x0000, 0x4000, (long)slot0 * 16384),
            new GuestSourceWindow("sms.cpu", view, 0x4000, 0x4000, (long)slot1 * 16384),
            new GuestSourceWindow("sms.cpu", view, 0x8000, 0x4000, (long)slot2 * 16384)
        };
        return (new GuestSourceMemory(_source, windows), view);
    }

    private void CheckBank(int bank)
    {
        if (bank < 0 || bank >= Banks)
            throw new ArgumentOutOfRangeException(nameof(bank));
    }
}
