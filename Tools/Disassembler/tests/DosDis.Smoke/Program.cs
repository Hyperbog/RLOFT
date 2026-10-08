using System.Buffers.Binary;
using DosDis.Core;

static void Equal<T>(T actual, T expected, string label)
{
    if (!EqualityComparer<T>.Default.Equals(actual, expected))
        throw new Exception($"{label}: expected {expected}, received {actual}");
}
static void Fails(Action action, string label)
{
    try { action(); }
    catch (InvalidDataException) { return; }
    catch (NotSupportedException) { return; }
    throw new Exception($"{label}: malformed input was accepted");
}
static void Dword(byte[] b, int at, uint v) => BinaryPrimitives.WriteUInt32LittleEndian(b.AsSpan(at, 4), v);
static byte[] MakeFixture(bool nested)
{
    var b = new byte[0x600];
    int m = nested ? 0x180 : 0, h = m + 0x80;
    b[0] = (byte)'M'; b[1] = (byte)'Z';
    b[m] = (byte)'M'; b[m + 1] = (byte)'Z';
    Dword(b, m + 0x3c, 0x80);
    b[h] = (byte)'L'; b[h + 1] = (byte)'E';
    Dword(b, h + 0x14, 1);   // one stored LE page
    Dword(b, h + 0x18, 1);   // entry object
    Dword(b, h + 0x1c, 0);   // entry offset
    Dword(b, h + 0x28, 4096);
    Dword(b, h + 0x2c, 4);   // only four bytes stored in final page
    Dword(b, h + 0x40, 0xc4); // object table
    Dword(b, h + 0x44, 1);
    Dword(b, h + 0x48, 0xdc); // page map immediately after object
    Dword(b, h + 0x80, 0x200); // nested origin, NOT outer 0
    Dword(b, h + 0xc4, 4); // object virtual size
    Dword(b, h + 0xc8, 0x10000);
    Dword(b, h + 0xcc, 0x2005);
    Dword(b, h + 0xd0, 1);
    Dword(b, h + 0xd4, 1);
    b[h + 0xdc + 2] = 1; // first page ordinal, big-endian 3-byte LE page map index
    b[m + 0x200] = 0x90; b[m + 0x201] = 0xC3; // nop / ret
    b[m + 0x202] = 0x90; b[m + 0x203] = 0xC3;
    return b;
}

try
{
    foreach (bool nested in new[] { false, true })
    {
        byte[] raw = MakeFixture(nested);
        var art = BinaryArtifact.FromBytes(raw);
        var image = LeImage.Load(art);
        Equal(image.ModuleOrigin, nested ? 0x180L : 0L, "nested module origin");
        Equal(image.Objects.Count, 1, "object count");
        Equal(image.ToFileOffset(1, 0), nested ? 0x380L : 0x200L, "file mapping");
        Equal(image.ToFileOffset(1, 4), (long?)null, "object out-of-range");
        Equal(Disassembly.Decode(image, 1, 0, 2)[1].Assembly, "ret", "decode RET");
        Equal(Disassembly.Decode(image, 1, 0, 2)[0].Bytes, "90", "encoded byte");
        raw[0] = 0; // original snapshot must not change
        Equal(art.Data[0], (byte)'M', "immutability");
    }
    var invalidMap = MakeFixture(false);
    invalidMap[0x80 + 0xdc + 3] = 5;
    Fails(() => LeImage.Load(BinaryArtifact.FromBytes(invalidMap)), "unsupported page kind");
    var invalidHeader = MakeFixture(false);
    Dword(invalidHeader, 0x80 + 0x40, uint.MaxValue);
    Fails(() => LeImage.Load(BinaryArtifact.FromBytes(invalidHeader)), "invalid object offset");
    var tooShort = new byte[] { (byte)'M', (byte)'Z' };
    Fails(() => LeImage.Load(BinaryArtifact.FromBytes(tooShort)), "truncated MZ");
    Console.WriteLine("PASS: synthetic nested/flat LE, address maps, decoder, immutability and malformed fixtures");

    if (args.Length == 2)
    {
        string[] names = { "CODE.1", "CODE.2" };
        long[] starts = { 0x2c724, 0x3af24 };
        uint[] sizes = { 0x11302, 0x3a852 };
        uint[] entries = { 0xcc1c, 0x353a0 };
        for (int i = 0; i < 2; i++)
        {
            var art = BinaryArtifact.Open(args[i]);
            var image = LeImage.Load(art);
            Equal(image.ModuleOrigin, 0x25d24L, names[i] + " module");
            Equal(image.HeaderOffset, 0x27bdcL, names[i] + " LE");
            Equal(image.Objects[0].VirtualSize, sizes[i], names[i] + " object size");
            Equal(image.EntryOffset, entries[i], names[i] + " entry");
            Equal(image.ToFileOffset(1, 0), starts[i], names[i] + " object start");
        }
        var code2 = LeImage.Load(BinaryArtifact.Open(args[1]));
        Equal(code2.ToFileOffset(1, 0x2975c), 0x64680L, "CODE.2 wrapper physical offset");
        var lines = Disassembly.Decode(code2, 1, 0x2975c, 4);
        Equal(lines[0].ObjectOffset, 0x2975cU, "CODE.2 wrapper instruction start");
        Equal(lines[0].Bytes, "FF742414", "CODE.2 wrapper first instruction bytes");
        Console.WriteLine("PASS: RLOFT original CODE.1/CODE.2 hashes/geometry/addresses/decoder fixtures");
    }
    Console.WriteLine("ALL SMOKE TESTS PASSED");
    return 0;
}
catch (Exception ex)
{
    Console.Error.WriteLine($"FAIL: {ex}");
    return 1;
}
