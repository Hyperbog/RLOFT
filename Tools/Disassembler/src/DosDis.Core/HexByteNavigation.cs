using System.Globalization;

namespace DosDis.Core;

/// <summary>
/// Read-only addressable byte tokens for the existing fixed-column
/// RawHexPreview / LE hex text. Parsing explicitly ignores the ASCII
/// column and unbacked LE ?? cells; never infers executable code.
/// </summary>
public sealed record HexByteToken(int CharacterStart, int PhysicalFileOffset);

public static class HexByteNavigation
{
    public const int MaximumPatternBytes = 64;

    public static IReadOnlyList<HexByteToken> MapRenderedBytes(
        string text, Func<int, uint, int?>? leToPhysical = null)
    {
        ArgumentNullException.ThrowIfNull(text);
        if (text.Length > 300_000)
            throw new ArgumentOutOfRangeException(nameof(text), "Hex preview exceeds bounded scope.");
        var tokens = new List<HexByteToken>();
        int lineStart = 0;
        while (lineStart < text.Length)
        {
            int newline = text.IndexOf('\n', lineStart);
            int lineEnd = newline < 0 ? text.Length : newline;
            int realEnd = lineEnd > lineStart && text[lineEnd - 1] == '\r'
                ? lineEnd - 1 : lineEnd;
            ReadOnlySpan<char> line = text.AsSpan(lineStart, realEnd - lineStart);
            int split = line.IndexOf("   ".AsSpan());
            if (split > 0)
            {
                ReadOnlySpan<char> header = line[..split];
                bool physicalHeader = false;
                int baseFile = 0;
                int objectNumber = 0;
                uint baseVirtual = 0;
                if (header.StartsWith("file+0x".AsSpan(), StringComparison.Ordinal) &&
                    int.TryParse(header[7..], NumberStyles.HexNumber,
                        CultureInfo.InvariantCulture, out baseFile) && baseFile >= 0)
                    physicalHeader = true;
                else
                {
                    int colon = header.IndexOf(':');
                    if (colon > 0 &&
                        int.TryParse(header[..colon], NumberStyles.None,
                            CultureInfo.InvariantCulture, out objectNumber) &&
                        objectNumber > 0 &&
                        uint.TryParse(header[(colon + 1)..], NumberStyles.HexNumber,
                            CultureInfo.InvariantCulture, out baseVirtual))
                    {
                        // Non-physical LE addresses require explicit original
                        // object:offset translation by the validated LE loader.
                    }
                    else
                    {
                        split = -1;
                    }
                }
                if (split >= 0)
                {
                    int bytesStart = split + 3;
                    for (int column = 0; column < 16; column++)
                    {
                        int charPos = bytesStart + column * 3;
                        if (charPos + 2 >= line.Length || line[charPos + 2] != ' ')
                            break;
                        if (!Uri.IsHexDigit(line[charPos]) || !Uri.IsHexDigit(line[charPos + 1]))
                            continue; // LE ?? or unbacked cells.
                        int? physical = null;
                        if (physicalHeader && (long)baseFile + column <= int.MaxValue)
                            physical = baseFile + column;
                        else if (!physicalHeader && leToPhysical is not null &&
                                 (ulong)baseVirtual + (uint)column <= uint.MaxValue)
                            physical = leToPhysical(objectNumber, baseVirtual + (uint)column);
                        if (physical is >= 0)
                            tokens.Add(new HexByteToken(lineStart + charPos, physical.Value));
                    }
                }
            }
            if (newline < 0) break;
            lineStart = newline + 1;
        }
        return tokens;
    }

    public static HexByteToken? HitTest(IReadOnlyList<HexByteToken> tokens, int characterIndex)
    {
        ArgumentNullException.ThrowIfNull(tokens);
        foreach (var token in tokens)
            if (characterIndex >= token.CharacterStart &&
                characterIndex < token.CharacterStart + 2)
                return token;
        return null; // spaces, row prefix, ASCII column, invalid LE bytes.
    }

    public static HexByteToken? FindPhysical(
        IReadOnlyList<HexByteToken> tokens, int physicalFileOffset)
    {
        ArgumentNullException.ThrowIfNull(tokens);
        return tokens.FirstOrDefault(token => token.PhysicalFileOffset == physicalFileOffset);
    }

    /// <summary>
    /// Hex data search: CD / CD 21 / 4604CD21 / hex:46 04 CD 21.
    /// Returns false for a CS:IP or file address, odd digits or oversized input.
    /// </summary>
    public static bool TryParseBytePattern(string? input, out byte[] bytes)
    {
        bytes = Array.Empty<byte>();
        if (input is null) return false;
        string query = input.Trim();
        if (query.StartsWith("hex:", StringComparison.OrdinalIgnoreCase))
            query = query[4..].Trim();
        else if (query.Contains(':')) return false;
        if (query.Length == 0 || query.Length > MaximumPatternBytes * 3 + 16)
            return false;
        string digits = new(query.Where(c => !char.IsWhiteSpace(c)).ToArray());
        if (digits.Length is < 2 or > (MaximumPatternBytes * 2) ||
            digits.Length % 2 != 0 || digits.Any(c => !Uri.IsHexDigit(c)))
            return false;
        bytes = Convert.FromHexString(digits);
        return true;
    }

    public static int FindNext(ReadOnlySpan<byte> original,
        ReadOnlySpan<byte> pattern, int startFileOffset)
    {
        if (pattern.Length is < 1 or > MaximumPatternBytes ||
            startFileOffset < 0 || startFileOffset > original.Length)
            throw new ArgumentOutOfRangeException(nameof(startFileOffset));
        if (original.Length < pattern.Length) return -1;
        int start = Math.Min(startFileOffset, original.Length);
        int next = original[start..].IndexOf(pattern);
        if (next >= 0) return start + next;
        if (start > 0)
        {
            int wrapped = original[..start].IndexOf(pattern);
            if (wrapped >= 0) return wrapped;
        }
        return -1;
    }
}
