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
    if (args.Length < 2 || args[0] is not ("inspect" or "disasm"))
    {
        Console.Error.WriteLine("Usage: dosdis inspect <file> | dosdis disasm <file> --object 1 --offset 0x2975C --count 16");
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
                image.EntryObject, image.EntryOffset, image.Objects, Pages = image.Pages
            },
            Intelligence = FileIntelligence.Inspect(artifact, image)
        });
    }
    else
    {
        image ??= LeImage.Load(artifact);
        int objectNumber = int.Parse(Option(args, "--object", "1"), CultureInfo.InvariantCulture);
        uint offset = Hex(Option(args, "--offset", "0"));
        int count = int.Parse(Option(args, "--count", "16"), CultureInfo.InvariantCulture);
        Print(new { artifact.Sha256, Object = objectNumber, Offset = $"0x{offset:X}",
            Lines = Disassembly.Decode(image, objectNumber, offset, count) });
    }
    return 0;
}
catch (Exception ex) when (ex is IOException or ArgumentException or OverflowException or NotSupportedException)
{
    Console.Error.WriteLine($"Analysis failed: {ex.Message}");
    return 1;
}
