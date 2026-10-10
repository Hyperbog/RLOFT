using System.Buffers.Binary;

namespace DosDis.Core;

/// <summary>
/// Signature-level locations only. Real object/page/segment layouts need
/// a separate validated loader; arbitrary embedded signatures are not
/// proof of runtime reachability.
/// </summary>
public sealed record MzExtendedModuleLocation(
    long ModuleOrigin, long HeaderOffset, string FormatId);

public static class MzExtendedModuleLocator
{
    public static IReadOnlyList<MzExtendedModuleLocation> Scan(BinaryArtifact artifact)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        ReadOnlySpan<byte> bytes = artifact.Data;
        var found = new List<MzExtendedModuleLocation>();
        if (bytes.Length < 0x40) return found;
        for (int origin = 0; origin <= bytes.Length - 0x40; origin++)
        {
            if (bytes[origin] != (byte)'M' || bytes[origin + 1] != (byte)'Z')
                continue;
            uint relative = BinaryPrimitives.ReadUInt32LittleEndian(
                bytes.Slice(origin + 0x3C, 4));
            if (relative < 0x40 || (long)relative + 4 > bytes.Length - (long)origin)
                continue;
            int position = checked(origin + (int)relative);
            var header = bytes.Slice(position, 4);
            string? format = header.SequenceEqual("LE\0\0"u8) ? "le-signature" :
                header.SequenceEqual("LX\0\0"u8) ? "lx" :
                header[0] == (byte)'N' && header[1] == (byte)'E' ? "ne" :
                header.SequenceEqual("PE\0\0"u8) ? "pe" : null;
            if (format is null) continue;
            found.Add(new MzExtendedModuleLocation(origin, position, format));
        }
        return found;
    }
}
