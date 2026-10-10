using System.Text;

namespace DosDis.Core;

public sealed record IntelligenceField(string Value, string Status, string Evidence);

public static class FileIntelligence
{
    /// <summary>
    /// Only an opt-in catalog supplies game metadata. Format signatures
    /// provide separate evidence, never executable behavior assertions.
    /// </summary>
    public static IReadOnlyDictionary<string, IntelligenceField> Inspect(
        BinaryArtifact artifact, LeImage? le = null, SourceIdentityRegistry? registry = null)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        var fields = new SortedDictionary<string, IntelligenceField>();
        fields["sha256"] = new(artifact.Sha256, "direct", "SHA-256 over immutable input bytes");
        fields["size_bytes"] = new(artifact.Length.ToString(), "direct", "Input byte length");

        var detected = BinaryFormatDetector.Inspect(artifact, le);
        fields["file_format"] = new(detected.FormatId, "signature-derived", detected.SignatureEvidence);
        fields["format_loader_status"] = new(detected.LoaderStatus,
            "tool-capability", detected.Qualification);
        fields["analysis_support"] = le is null
            ? new("Read-only bytes and format detection; no validated LE candidate analysis",
                "tool-capability",
                "Non-LE instruction decoding requires a separate validated loader and explicit address mapping.")
            : new("32-bit LE object-mapped candidate analysis", "tool-capability",
                "Only bounded MZ/LE executable mapping is currently supported for recursive disassembly.");

        SourceIdentityEntry? identity = registry?.Find(artifact);
        if (identity is not null)
        {
            const string status = "external-catalog-exact-hash";
            string evidence = identity.SourceEvidence;
            fields["game_id"] = new(identity.GameId, status, evidence);
            fields["module"] = new(identity.Module, status, evidence);
            void Add(string key, string? value)
            {
                if (value is not null)
                    fields[key] = new IntelligenceField(value, status, evidence);
            }
            Add("software", identity.Software);
            Add("game_version", identity.Version);
            Add("developer", identity.Developer);
            Add("publisher", identity.Publisher);
            Add("original_year", identity.OriginalYear);
            Add("engine_family", identity.EngineFamily);
        }

        // Embedded strings are hints; they do not establish interpreter,
        // runtime version, compiler identity or a unique source edition.
        string binaryText = Encoding.Latin1.GetString(artifact.Data);
        if (binaryText.Contains("DOS/4GW Professional Protected Mode Run-time", StringComparison.Ordinal))
            fields["dos_extender"] = new("DOS/4GW Professional", "embedded-string",
                "Literal extender banner appears in the original file");
        if (fields.ContainsKey("dos_extender") &&
            binaryText.Contains("Feb 17 1994 11:54:25", StringComparison.Ordinal))
            fields["extender_build_stamp"] = new("Feb 17 1994 11:54:25", "embedded-string",
                "Literal timestamp appears alongside an extender banner");
        if (le is not null)
        {
            fields["executable_format"] = new("MZ-bound LE", "validated-loader",
                $"Nested MZ at 0x{le.ModuleOrigin:X}, LE header at 0x{le.HeaderOffset:X}");
            fields["architecture"] = new("x86 32-bit protected-mode application", "header-derived",
                "LE object geometry; runtime context still requires validation");
            fields["entry_point"] = new($"object {le.EntryObject} + 0x{le.EntryOffset:X}",
                "header-derived", "LE entry object/offset");
            fields["minimum_dos_version"] = new("Unknown", "unknown",
                "Not inferred from DOS API usage or extender banner");
            fields["compiler_exact_version"] = new("Unknown", "unknown",
                "Version-specific compiler signatures are not proven");
        }
        return fields;
    }
}
