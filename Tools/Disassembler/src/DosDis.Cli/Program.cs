using System.Globalization;
using System.Text.Json;
using DosDis.Core;

static string Option(string[] args, string name, string fallback)
{
    int i = Array.IndexOf(args, name);
    return i < 0 ? fallback : i + 1 < args.Length ? args[i + 1]
        : throw new ArgumentException($"Missing value for {name}.");
}
static uint Hex(string s) => uint.Parse(s.StartsWith("0x", StringComparison.OrdinalIgnoreCase) ? s[2..] : s,
    NumberStyles.AllowHexSpecifier, CultureInfo.InvariantCulture);
static void Print(object payload) => Console.WriteLine(JsonSerializer.Serialize(payload,
    new JsonSerializerOptions { WriteIndented = true }));

try
{
    if (args.Length < 2 || args[0] is not ("inspect" or "disasm" or "fixups" or "memory"))
    {
        Console.Error.WriteLine("Usage: dosdis inspect <file> | dosdis fixups <file> [--max 40] | dosdis memory <file> --object 1 --offset 0 --count 16 | dosdis disasm <file> --object 1 --offset 0x2975C --count 16 [--relocated]");
        return 2;
    }
    var artifact = BinaryArtifact.Open(args[1]);
    LeImage? image = null;
    try { image = LeImage.Load(artifact); }
    catch (InvalidDataException) when (args[0] == "inspect") { /* Unsupported format: report only confirmed fields. */ }
    catch (NotSupportedException) when (args[0] == "inspect") { /* Unsupported LE page variant. */ }
    if (args[0] == "inspect")
    {
        Print(new {
            artifact.Name,
            artifact.Length,
            artifact.Sha256,
            Format = image == null ? "unrecognized-or-not-yet-supported" : "DOS LE",
            Module = image == null ? null : new {
                image.ModuleOrigin, image.HeaderOffset, image.PageSize,
                image.EntryObject, image.EntryOffset, image.Objects, Pages = image.Pages,
                FixupCount = image.Relocations.Count
            },
            Intelligence = FileIntelligence.Inspect(artifact, image)
        });
    }
    else if (args[0] == "fixups")
    {
        image ??= LeImage.Load(artifact);
        int max = int.Parse(Option(args, "--max", "40"), CultureInfo.InvariantCulture);
        if (max is < 0 or > 10000) throw new ArgumentOutOfRangeException(nameof(max));
        Print(new { artifact.Sha256, Total = image.Relocations.Count,
            Fixups = image.Relocations.Take(max) });
    }
    else if (args[0] == "memory")
    {
        image ??= LeImage.Load(artifact);
        int number = int.Parse(Option(args, "--object", "1"), CultureInfo.InvariantCulture);
        uint at = Hex(Option(args, "--offset", "0"));
        int length = int.Parse(Option(args, "--count", "16"), CultureInfo.InvariantCulture);
        if (length is < 1 or > 4096) throw new ArgumentOutOfRangeException(nameof(length));
        Print(new { artifact.Sha256, Object = number, Offset = $"0x{at:X}",
            MemoryView = "relocated-virtual-memory", Bytes = Convert.ToHexString(image.ReadRelocatedBytes(number, at, length)),
            FirstSourceFileOffset = image.ToFileOffset(number, at) });
    }
    else
    {
        image ??= LeImage.Load(artifact);
        int objectNumber = int.Parse(Option(args, "--object", "1"), CultureInfo.InvariantCulture);
        uint offset = Hex(Option(args, "--offset", "0"));
        int count = int.Parse(Option(args, "--count", "16"), CultureInfo.InvariantCulture);
        bool relocated = args.Contains("--relocated", StringComparer.Ordinal);
        Print(new { artifact.Sha256, Object = objectNumber, Offset = $"0x{offset:X}",
            MemoryView = relocated ? "relocated-virtual-memory" : "original-file-bytes",
            Lines = Disassembly.Decode(image, objectNumber, offset, count, relocated) });
    }
    return 0;
}
catch (Exception ex) when (ex is IOException or ArgumentException or OverflowException or NotSupportedException)
{
    Console.Error.WriteLine($"Analysis failed: {ex.Message}");
    return 1;
}
