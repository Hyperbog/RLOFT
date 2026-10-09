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
            // e_lfanew is advisory until the extended header is validated
            // by its own dedicated loader.
            if (bytes.Length >= 0x40)
            {
                uint offset = BinaryPrimitives.ReadUInt32LittleEndian(bytes.Slice(0x3C, 4));
                if ((ulong)offset + 2 <= (ulong)bytes.Length && offset >= 0x40)
                {
                    var signature = bytes.Slice((int)offset);
                    if (signature.StartsWith("LE"u8))
                        return new("le-signature", "MZ/e_lfanew points to LE signature",
                            offset, "x86-32-candidate", "le-validation-required",
                            "Signature recognition is not a verified LE object/page map.");
                    if (signature.StartsWith("LX"u8))
                        return new("lx", "MZ/e_lfanew points to LX signature",
                            offset, "unknown-until-lx-mapped", "lx-parser-required",
                            "Do not treat an LX module as Windows PE or raw code.");
                    if (signature.StartsWith("NE"u8))
                        return new("ne", "MZ/e_lfanew points to NE signature",
                            offset, "x86-16-candidate", "ne-parser-required",
                            "NE segments and relocations require a dedicated loader.");
                    if (signature.Length >= 4 && signature.StartsWith("PE\0\0"u8))
                        return new("pe", "MZ/e_lfanew points to PE signature",
                            offset, "unknown-until-pe-mapped", "pe-parser-required",
                            "Separate supporting software from original DOS executable targets.");
                }
            }
            return new("mz", "MZ at offset zero", 0, "x86-16-candidate",
                "mz-header-validation-required",
                "DOS MZ metadata and relocation validation required before image-relative decoding.");
        }

        return Known("unknown-headerless", "no supported executable signature at offset zero",
            "manual-format-selection-required",
            "COM, device drivers, wrapped modules and arbitrary data cannot be inferred from extensions.");
    }

    private static BinaryFormatDetection Known(string format, string evidence, string status, string qualification) =>
        new(format, evidence, 0, "not-assumed", status, qualification);
}
