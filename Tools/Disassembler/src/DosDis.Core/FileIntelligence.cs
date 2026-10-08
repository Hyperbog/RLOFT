using System.Text;

namespace DosDis.Core;

public sealed record IntelligenceField(string Value, string Status, string Evidence);

public static class FileIntelligence
{
    private static readonly Dictionary<string, (string Title, string Version)> Known = new()
    {
        ["fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed"] =
            ("Ravenloft: Strahd's Possession / CODE.1", "1.2a executable set"),
        ["853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"] =
            ("Ravenloft: Strahd's Possession / CODE.2", "1.2a")
    };

    public static IReadOnlyDictionary<string, IntelligenceField> Inspect(BinaryArtifact artifact,
        LeImage? le = null)
    {
        var fields = new SortedDictionary<string, IntelligenceField>();
        fields["sha256"] = new(artifact.Sha256, "direct", "SHA-256 over input bytes");
        fields["size_bytes"] = new(artifact.Length.ToString(), "direct", "Input byte length");
        if (Known.TryGetValue(artifact.Sha256, out var identity))
        {
            fields["software"] = new(identity.Title, "verified-fingerprint", "RLOFT preserved 1.2a SHA-256 corpus");
            fields["game_version"] = new(identity.Version, "verified-fingerprint", "RLOFT source identity manifest");
            fields["developer"] = new("DreamForge Intertainment", "documented", "RLOFT preservation archive, chapter 19");
            fields["publisher"] = new("Strategic Simulations, Inc. (SSI)", "documented", "RLOFT preservation archive, chapter 19");
            fields["original_year"] = new("1994", "documented", "RLOFT preservation archive, chapter 19");
            fields["engine_family"] = new("DreamForge RPG lineage", "documented", "RLOFT preservation archive, chapter 19");
        }
        if (le != null)
        {
            fields["executable_format"] = new("MZ-bound LE", "direct", $"Nested MZ at 0x{le.ModuleOrigin:X}, LE header at 0x{le.HeaderOffset:X}");
            fields["architecture"] = new("x86 32-bit protected-mode application", "header-derived", "LE application object geometry; runtime context requires validation");
            fields["entry_point"] = new($"object {le.EntryObject} + 0x{le.EntryOffset:X}", "direct", "LE entry object/offset fields");
        }
        // A string signature establishes its presence in the file, not the
        // minimum DOS version, original compiler version, or runtime parity.
        string content = Encoding.Latin1.GetString(artifact.Data);
        if (content.Contains("DOS/4GW Professional Protected Mode Run-time", StringComparison.Ordinal))
            fields["dos_extender"] = new("DOS/4GW Professional", "embedded-string", "Exact original executable banner");
        if (fields.ContainsKey("dos_extender") &&
            content.Contains("Feb 17 1994 11:54:25", StringComparison.Ordinal))
            fields["extender_build_stamp"] = new("Feb 17 1994 11:54:25", "embedded-string", "DOS/4GW bound-runtime identification");
        fields["minimum_dos_version"] = new("Unknown", "unknown", "Not inferred from DOS API usage or extender version");
        fields["compiler_exact_version"] = new("Unknown", "unknown", "Version-specific compiler signature verification pending");
        return fields;
    }
}
