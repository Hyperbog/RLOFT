namespace DosDis.Core;

/// <summary>
/// Signature-first classification. Recognition never grants permission to
/// decode an unvalidated container body as executable instructions.
/// </summary>
public sealed record BinaryFormatDetection(
    string FormatId, string SignatureEvidence, long SignatureFileOffset,
    string DecoderMode, string LoaderStatus, string Qualification);

public static class BinaryFormatDetector
{
    public static BinaryFormatDetection Inspect(BinaryArtifact artifact, LeImage? validatedLe = null)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        var bytes = artifact.Data;
        if (validatedLe is not null)
        {
            if (validatedLe.ArtifactSha256 != artifact.Sha256)
                throw new InvalidDataException("Validated LE module belongs to another original source.");
            return new("le", "validated MZ-bound LE object/page mapping",
                validatedLe.HeaderOffset, "x86-32", "supported-le",
                "Object-relative candidate analysis; runtime targets remain unverified.");
        }

        // Container signatures take priority over misleading extensions.
        if (bytes.StartsWith("FBOV"u8))
            return Known("fbov", "FBOV at offset zero", "overlay-parser-required",
                "The overlay directory, segment loading and code offsets are not yet mapped.");
        if (bytes.StartsWith("TPOV"u8))
            return Known("tpov", "TPOV at offset zero", "overlay-parser-required",
                "The overlay directory, segment loading and code offsets are not yet mapped.");
        if (bytes.StartsWith("AESOP/16"u8))
            return Known("aesop-resource", "AESOP/16 at offset zero", "vm-parser-required",
                "AESOP instructions are not x86 instructions.");
        if (bytes.StartsWith("GFFI"u8))
            return Known("gff-container", "GFFI at offset zero", "resource-parser-required",
                "GFF resource records require their own validated container mapping.");

        if (bytes.Length >= 2 && bytes[0] == (byte)'M' && bytes[1] == (byte)'Z')
        {
            // All supported loaders share the same source-backed MZ/extension
            // geometry. Multiple plausible modules remain ambiguous.
            var modules = MzExtendedModuleLocator.Scan(artifact);
            if (modules.Count > 1)
                return new("ambiguous-mz-extended", "multiple MZ-bound extended headers",
                    0, "not-assumed", "manual-module-selection-required",
                    "Multiple embedded module signatures; do not guess which program is executed.");
            if (modules.Count == 1)
            {
                var module = modules[0];
                string status = module.FormatId switch
                {
                    "le-signature" => "le-validation-required",
                    "lx" => "lx-parser-required",
                    "ne" => "ne-parser-required",
                    _ => "pe-parser-required"
                };
                string decoder = module.FormatId switch
                {
                    "le-signature" => "x86-32-candidate",
                    "ne" => "x86-16-candidate",
                    _ => "unknown-until-loader-mapped"
                };
                return new(module.FormatId,
                    $"MZ at 0x{module.ModuleOrigin:X} links to embedded extended header",
                    module.HeaderOffset, decoder, status,
                    "Signature-only identification; object/segment maps need independent validation. " +
                    "The outer DOS launcher is not classified as a standalone executable.");
            }
            return new("mz", "MZ at offset zero; no recognized bounded extended module",
                0, "x86-16-candidate", "mz-header-validation-required",
                "Standalone DOS MZ header/relocations still require validation; appended data may exist.");
        }

        return Known("unknown-headerless", "no supported executable signature at offset zero",
            "manual-format-selection-required",
            "COM, device drivers, wrapped modules and arbitrary data cannot be inferred from extensions.");
    }

    private static BinaryFormatDetection Known(string format, string evidence, string status, string qualification) =>
        new(format, evidence, 0, "not-assumed", status, qualification);
}
