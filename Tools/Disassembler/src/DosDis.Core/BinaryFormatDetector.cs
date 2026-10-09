using System.Buffers.Binary;

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
            // An outer DOS launcher may append an independent MZ-bound
            // LE/LX/NE/PE module. Never declare the outer header a standalone
            // 16-bit program merely because its e_lfanew is not usable.
            // Every candidate must have its OWN MZ at the module origin and
            // its OWN bounded e_lfanew at module+0x3c.
            // Recognition is still weaker than a validated format loader.
            string? detected = null;
            long moduleStart = -1, extensionAt = -1;
            int candidates = 0;
            for (int origin = 0; origin <= bytes.Length - 0x40; origin++)
            {
                if (bytes[origin] != (byte)'M' || bytes[origin + 1] != (byte)'Z')
                    continue;
                uint relative = BinaryPrimitives.ReadUInt32LittleEndian(
                    bytes.Slice(origin + 0x3c, 4));
                if (relative < 0x40 || (ulong)relative + 4 > (ulong)bytes.Length - (uint)origin)
                    continue;
                int at = checked(origin + (int)relative);
                var header = bytes.Slice(at, 4);
                string? family = header.SequenceEqual("LE\0\0"u8) ? "le-signature" :
                    header.SequenceEqual("LX\0\0"u8) ? "lx" :
                    header[0] == (byte)'N' && header[1] == (byte)'E' ? "ne" :
                    header.SequenceEqual("PE\0\0"u8) ? "pe" : null;
                if (family is null) continue;
                candidates++;
                detected = family;
                moduleStart = origin;
                extensionAt = at;
                if (candidates > 1) break;
            }
            if (candidates > 1)
                return new("ambiguous-mz-extended", "multiple MZ-bound extended headers",
                    0, "not-assumed", "manual-module-selection-required",
                    "Multiple embedded module signatures; do not guess which program is executed.");
            if (candidates == 1)
            {
                string status = detected switch
                {
                    "le-signature" => "le-validation-required",
                    "lx" => "lx-parser-required",
                    "ne" => "ne-parser-required",
                    _ => "pe-parser-required"
                };
                string decoder = detected switch
                {
                    "le-signature" => "x86-32-candidate",
                    "ne" => "x86-16-candidate",
                    _ => "unknown-until-loader-mapped"
                };
                return new(detected!, $"MZ at 0x{moduleStart:X} links to embedded extended header",
                    extensionAt, decoder, status,
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
