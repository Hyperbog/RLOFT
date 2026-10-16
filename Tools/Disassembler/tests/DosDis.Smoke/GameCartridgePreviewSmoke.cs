using DosDis.Core;

internal static class GameCartridgePreviewSmoke
{
    private static void Same<T>(T actual, T expected, string reason)
    {
        if (!EqualityComparer<T>.Default.Equals(actual, expected))
            throw new Exception("Cartridge preview " + reason + ": expected " +
                expected + ", got " + actual);
    }

    public static void Run()
    {
        var nes = new byte[16 + 16384];
        nes[0] = (byte)'N'; nes[1] = (byte)'E';
        nes[2] = (byte)'S'; nes[3] = 0x1A; nes[4] = 1;
        nes[16] = 0xA9; nes[17] = 1; nes[18] = 0x60;
        nes[^4] = 0x00; nes[^3] = 0x80; // reset vector
        BinaryArtifact nesArtifact = BinaryArtifact.FromBytes(nes);
        GameCartridgePreview shown = GameCartridgePreview.TryCreate(nesArtifact)
            ?? throw new Exception("NROM must produce source-backed preview.");
        Same(shown.PlatformLabel, "NES NROM / 6502", "NROM identification");
        Same(shown.Instructions.Count, 2, "bounded NROM instruction list");
        Same(shown.Instructions[0].GuestAddress, 0x8000u, "NROM reset entry");
        Same(shown.Instructions[0].OriginalBytes, "A901", "NROM source bytes");
        Same(shown.Instructions[0].SourceFragments[0].PhysicalStart,
            16L, "correct iNES header file offset");
        Same(shown.Instructions[1].Assembly, "RTS", "source-backed return");
        Same(shown.Truncated, false, "bounded small program");

        byte[] banked = (byte[])nes.Clone();
        banked[6] = 0x20; // mapper 2
        Array.Resize(ref banked, 16 + 2 * 16384);
        Same(GameCartridgePreview.TryCreate(BinaryArtifact.FromBytes(banked)) is null,
            true, "switchable mapper cannot auto-select bank");

        var sms = new byte[16384];
        sms[0] = 0x00; sms[1] = 0xC9;
        "TMR SEGA"u8.CopyTo(sms.AsSpan(0x3FF0));
        var smsArtifact = BinaryArtifact.FromBytes(sms);
        GameCartridgePreview smsPreview = GameCartridgePreview.TryCreate(smsArtifact)
            ?? throw new Exception("Single-bank SMS must produce source-backed preview.");
        Same(smsPreview.Instructions.Count, 2, "Z80 entry preview");
        Same(smsPreview.Instructions[0].OriginalBytes, "00", "SMS NOP source");
        Same(smsPreview.Instructions[1].Assembly, "RET", "SMS RET");
        Same(smsPreview.Instructions[0].SourceFragments[0].PhysicalStart,
            0L, "SMS physical first byte");

        var larger = new byte[32768];
        "TMR SEGA"u8.CopyTo(larger.AsSpan(0x7FF0));
        Same(GameCartridgePreview.TryCreate(BinaryArtifact.FromBytes(larger)) is null,
            true, "bank-switched SMS view cannot be auto-assumed");
        Same(GameCartridgePreview.TryCreate(BinaryArtifact.FromBytes(
            new byte[] { 0x11, 0x22, 0x33 })) is null,
            true, "arbitrary raw data is never auto-disassembled");
        Console.WriteLine("PASS: conservative source-backed NES NROM and single-bank SMS desktop previews, " +
            "physical spans, mapper abstention and raw-data rejection");
    }
}
