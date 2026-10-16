using DosDis.Core;

internal static class GuestSourceMemorySmoke
{
    private static void Same<T>(T actual, T expected, string label)
    {
        if (!EqualityComparer<T>.Default.Equals(actual, expected))
            throw new Exception("Guest mapping " + label + ": unexpected result.");
    }

    private static void Reject(Action action, string label)
    {
        try { action(); }
        catch (InvalidDataException) { return; }
        throw new Exception("Guest mapping accepted " + label);
    }

    public static void Run()
    {
        byte[] original = Enumerable.Range(0, 40).Select(n => (byte)n).ToArray();
        BinaryArtifact artifact = BinaryArtifact.FromBytes(original);
        var memory = new GuestSourceMemory(artifact, new[]
        {
            new GuestSourceWindow("nes.cpu", "bank-0", 0x8000, 2, 4),
            new GuestSourceWindow("nes.cpu", "bank-0", 0x8002, 2, 16),
            new GuestSourceWindow("nes.cpu", "bank-1", 0x8000, 4, 20),
            new GuestSourceWindow("sms.cpu", "page-1", 0x4000, 4, 8),
            new GuestSourceWindow("dos.real", "cs:1234", 0, 4, 0)
        });
        Same(memory.SourceSha256, artifact.Sha256, "original SHA-256");
        GuestSourceRead bank0 = memory.TryReadExact("nes.cpu", "bank-0", 0x8000, 4)
            ?? throw new Exception("Expected mapped NES bank.");
        Same(Convert.ToHexString(bank0.Bytes), "04051011", "fragmented ROM mapping");
        Same(bank0.Fragments.Count, 2, "two physical provenance intervals");
        Same(bank0.Fragments[0].PhysicalStart, 4L, "first physical source");
        Same(bank0.Fragments[1].PhysicalStart, 16L, "second physical source");
        Same(Convert.ToHexString(memory.TryReadExact("nes.cpu", "bank-1", 0x8000, 4)!.Bytes),
            "14151617", "same CPU address, different ROM bank");
        Same(Convert.ToHexString(memory.TryReadExact("sms.cpu", "page-1", 0x4000, 4)!.Bytes),
            "08090A0B", "SMS source view");
        Same(Convert.ToHexString(memory.TryReadExact("dos.real", "cs:1234", 0, 4)!.Bytes),
            "00010203", "DOS segmented source view");

        original[4] = 255;
        bank0.Bytes[0] = 255;
        Same(memory.TryReadExact("nes.cpu", "bank-0", 0x8000, 1)!.Bytes[0],
            (byte)4, "immutable artifact and independent reads");
        Same(memory.TryReadExact("nes.cpu", "bank-0", 0x8003, 2) is null,
            true, "unmapped guest gap");
        Same(memory.TryReadExact("nes.cpu", "missing", 0x8000, 1) is null,
            true, "missing mapper bank");
        Same(memory.TryReadExact("sms.cpu", "page-1", 0x8000, 1) is null,
            true, "cross-platform isolation");
        Same(memory.TryReadExact("nes.cpu", "bank-0", uint.MaxValue, 2) is null,
            true, "guest address overflow");

        Reject(() => new GuestSourceMemory(artifact, new[]
        {
            new GuestSourceWindow("nes.cpu", "bank-0", 0x8000, 4, 0),
            new GuestSourceWindow("nes.cpu", "bank-0", 0x8002, 4, 12)
        }), "overlap in selected memory view");
        Reject(() => new GuestSourceMemory(artifact, new[]
        {
            new GuestSourceWindow("nes.cpu", "bank-0", 0x8000, 2, 39)
        }), "out-of-file source bytes");
        Reject(() => new GuestSourceMemory(artifact, new[]
        {
            new GuestSourceWindow("nes.cpu", "bank-0", uint.MaxValue, 2, 0)
        }), "guest arithmetic overflow");
        Reject(() => new GuestSourceMemory(artifact, new[]
        {
            new GuestSourceWindow("nes.cpu", "../escape", 0x8000, 1, 0)
        }), "invalid mapping view identifier");
        Reject(() => new GuestSourceMemory(artifact, new[]
        {
            new GuestSourceWindow("nes.cpu", "bank-0", 0x8000, 0, 0)
        }), "empty memory interval");
        Console.WriteLine("PASS: platform-neutral DOS/NES/SMS mapping, banks, " +
            "original bytes, provenance, unmapped gaps and adversarial bounds");
    }
}
