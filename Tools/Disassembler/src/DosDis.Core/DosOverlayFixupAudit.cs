using System.Security.Cryptography;

namespace DosDis.Core;

/// <summary>
/// Verifiable stored overlay extents. Fixup payloads remain opaque and cannot
/// be used to infer relocations or runtime segment addresses.
/// </summary>
public sealed record DosOverlayStoredUnit(
    int Index, int UnitNumber, int CodeFileOffset, int CodeByteLength,
    string? CodeSha256, int FixupFileOffset, int FixupByteLength,
    string? FixupSha256, string FixupInterpretation);

public sealed record DosOverlayStorageAudit(
    string ResidentSourceSha256, string OverlaySourceSha256, string Format,
    int HeaderByteLength, int CoveredStoredBytes, int TotalCodeBytes,
    int TotalOpaqueFixupBytes, string AnalysisStatus,
    IReadOnlyList<DosOverlayStoredUnit> Units);

public static class DosOverlayFixupAudit
{
    /// <summary>
    /// Checks a previously recovered paired-directory against its original
    /// overlay source. No fixup word format is guessed.
    /// </summary>
    public static DosOverlayStorageAudit Analyze(DosOverlayImage overlay,
        DosOverlayDirectory directory)
    {
        ArgumentNullException.ThrowIfNull(overlay);
        ArgumentNullException.ThrowIfNull(directory);
        var original = overlay.Envelope;
        if (!string.Equals(directory.OverlaySourceSha256, original.SourceSha256,
                StringComparison.OrdinalIgnoreCase) ||
            directory.Format != original.Format ||
            directory.HeaderBytes != original.HeaderBytes ||
            directory.CoveredPayloadBytes != original.PayloadBytes ||
            directory.Units is null ||
            directory.Units.Count is < 1 or > 4096 ||
            directory.ResidentSourceSha256 is not { Length: 64 })
            throw new InvalidDataException(
                "Overlay unit map and original stored source identity do not agree.");

        int nextOffset = original.HeaderBytes;
        long codeBytes = 0, fixupBytes = 0;
        var units = new List<DosOverlayStoredUnit>(directory.Units.Count);
        foreach (var unit in directory.Units)
        {
            if (unit is null || unit.Index != units.Count + 1 ||
                unit.OverlayCodeFileOffset != nextOffset ||
                unit.CodeByteLength < 0 || unit.FixupByteLength < 0 ||
                (long)unit.CodeByteLength + unit.FixupByteLength < 1 ||
                (long)unit.OverlayCodeFileOffset + unit.CodeByteLength !=
                    unit.OverlayFixupFileOffset ||
                (long)unit.OverlayFixupFileOffset + unit.FixupByteLength >
                    original.OriginalFileLength)
                throw new InvalidDataException(
                    "Overlay unit code/fixup extents are ambiguous or outside the original source.");

            byte[] code = overlay.ReadStoredBytes(unit.OverlayCodeFileOffset, unit.CodeByteLength);
            byte[] fixups = overlay.ReadStoredBytes(unit.OverlayFixupFileOffset, unit.FixupByteLength);
            static string? Fingerprint(byte[] bytes) => bytes.Length == 0 ? null :
                Convert.ToHexString(SHA256.HashData(bytes)).ToLowerInvariant();

            units.Add(new DosOverlayStoredUnit(unit.Index, unit.UnitNumber,
                unit.OverlayCodeFileOffset, unit.CodeByteLength, Fingerprint(code),
                unit.OverlayFixupFileOffset, unit.FixupByteLength,
                Fingerprint(fixups), "opaque-original-bytes;relocation-encoding-not-established"));
            codeBytes += unit.CodeByteLength;
            fixupBytes += unit.FixupByteLength;
            nextOffset = checked(unit.OverlayFixupFileOffset + unit.FixupByteLength);
        }
        if (nextOffset != original.OriginalFileLength ||
            codeBytes + fixupBytes != original.PayloadBytes)
            throw new InvalidDataException("Overlay stored extents do not account for the full payload.");
        return new DosOverlayStorageAudit(directory.ResidentSourceSha256,
            directory.OverlaySourceSha256, directory.Format,
            original.HeaderBytes, original.PayloadBytes,
            checked((int)codeBytes), checked((int)fixupBytes),
            "stored-extent-verified;opaque-fixup-semantics-unknown", units);
    }
}
