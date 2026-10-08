using System.Text;

namespace DosDis.Core;

/// <summary>
/// Bounded, original-file-offset hexadecimal inspection for ANY input artifact.
/// It never attempts code decoding and cannot mutate the source snapshot.
/// </summary>
public static class RawHexPreview
{
    public static string Render(BinaryArtifact artifact, int startOffset = 0, int maxLines = 48)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        if (maxLines is < 1 or > 256)
            throw new ArgumentOutOfRangeException(nameof(maxLines), "Preview requires 1–256 lines.");
        if (startOffset < 0 || (artifact.Length == 0 ? startOffset != 0 : startOffset >= artifact.Length))
            throw new ArgumentOutOfRangeException(nameof(startOffset), "File offset is outside the original bytes.");

        ReadOnlySpan<byte> bytes = artifact.Data;
        if (bytes.Length == 0) return "(empty file; no bytes to inspect)";
        int start = startOffset & ~15;
        var builder = new StringBuilder();
        for (long line = 0; line < maxLines; line++)
        {
            long row = (long)start + line * 16;
            if (row >= bytes.Length) break;

            builder.Append("file+0x").Append(row.ToString("X8")).Append("   ");
            var ascii = new StringBuilder(16);
            for (int i = 0; i < 16; i++)
            {
                long at = row + i;
                if (at >= bytes.Length)
                {
                    builder.Append("   ");
                    ascii.Append(' ');
                    continue;
                }
                byte value = bytes[(int)at];
                builder.Append(value.ToString("X2")).Append(' ');
                ascii.Append(value is >= 32 and <= 126 ? (char)value : '.');
            }
            builder.Append("  ").AppendLine(ascii.ToString());
        }
        return builder.ToString();
    }
}
