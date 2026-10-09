using System.Buffers.Binary;

namespace DosDis.Core;

/// <summary>
/// MZ-resident Turbo Pascal overlay descriptors. These describe stored
/// byte extents, not final loaded segments or verified executable paths.
/// </summary>
public sealed record DosOverlayUnit(
    int Index, long ResidentDescriptorFileOffset, int ResidentDescriptorImageOffset,
    int OverlayCodeFileOffset, int CodeByteLength,
    int OverlayFixupFileOffset, int FixupByteLength,
    int ProcedureCount, int UnitNumber, string Evidence);

public sealed record DosOverlayDirectory(
    string Format, string ResidentSourceSha256, string OverlaySourceSha256,
    int HeaderBytes, int CoveredPayloadBytes, string VerificationStatus,
    IReadOnlyList<DosOverlayUnit> Units);

public static class DosOverlayDirectoryRecovery
{
    private const int DescriptorBytes = 32;
    private const int ProcedureStubBytes = 5;
    private const int MaximumProcedures = 4096;
    private const int MaximumUnits = 4096;

    /// <summary>
    /// Recover an unambiguous, complete Turbo Pascal overlay layout from
    /// resident MZ descriptors and the separate TPOV/8-byte-FBOV source.
    /// Ambiguity and incomplete coverage are errors, not partial successes.
    /// </summary>
    public static DosOverlayDirectory Recover(DosMzImage resident, DosOverlayImage overlay)
    {
        ArgumentNullException.ThrowIfNull(resident);
        ArgumentNullException.ThrowIfNull(overlay);
        var header = overlay.Envelope;
        if (header.Format is not ("tpov" or "fbov") ||
            (header.Format == "fbov" && header.Variant != "fbov-eight-byte-envelope"))
            throw new NotSupportedException(
                "This resident-directory parser supports TPOV and the 8-byte FBOV overlay variant only.");
        if (resident.FileBackedImageLength > 16 * 1024 * 1024)
            throw new NotSupportedException("Resident MZ image exceeds bounded overlay descriptor scanning.");
        byte[] residentBytes = resident.ReadStoredImageBytes(0, resident.FileBackedImageLength);
        var byOverlayOffset = new Dictionary<int, List<Descriptor>>();
        for (int i = 0; i <= residentBytes.Length - DescriptorBytes; i++)
        {
            // MZ overlay manager trap marker, with reserved zero word.
            if (residentBytes[i] != 0xCD || residentBytes[i + 1] != 0x3F ||
                residentBytes[i + 2] != 0 || residentBytes[i + 3] != 0)
                continue;
            ReadOnlySpan<byte> record = residentBytes.AsSpan(i, 16);
            uint overlayOffset = BinaryPrimitives.ReadUInt32LittleEndian(record.Slice(4, 4));
            int codeBytes = BinaryPrimitives.ReadUInt16LittleEndian(record.Slice(8, 2));
            int fixupBytes = BinaryPrimitives.ReadUInt16LittleEndian(record.Slice(10, 2));
            int procedureCount = BinaryPrimitives.ReadUInt16LittleEndian(record.Slice(12, 2));
            int unitNumber = BinaryPrimitives.ReadUInt16LittleEndian(record.Slice(14, 2));
            long end = (long)overlayOffset + codeBytes + fixupBytes;
            if (overlayOffset < header.HeaderBytes || overlayOffset > int.MaxValue ||
                end <= overlayOffset || end > header.OriginalFileLength ||
                procedureCount > MaximumProcedures ||
                (long)i + DescriptorBytes + (long)procedureCount * ProcedureStubBytes > residentBytes.Length)
                continue;

            bool stubsValid = true;
            for (int proc = 0; proc < procedureCount; proc++)
            {
                int stub = i + DescriptorBytes + proc * ProcedureStubBytes;
                if (residentBytes[stub] != 0xCD || residentBytes[stub + 1] != 0x3F)
                {
                    stubsValid = false;
                    break;
                }
            }
            if (!stubsValid) continue;

            var candidate = new Descriptor(i, (int)overlayOffset, codeBytes,
                fixupBytes, procedureCount, unitNumber);
            if (!byOverlayOffset.TryGetValue(candidate.OverlayOffset, out var group))
            {
                group = new List<Descriptor>();
                byOverlayOffset.Add(candidate.OverlayOffset, group);
            }
            group.Add(candidate);
        }

        int next = header.HeaderBytes;
        var units = new List<DosOverlayUnit>();
        while (next < header.OriginalFileLength)
        {
            if (!byOverlayOffset.TryGetValue(next, out var candidates) ||
                candidates.Count != 1)
                throw new InvalidDataException(
                    "Overlay directory has a missing or ambiguous resident descriptor at a required byte offset.");
            Descriptor candidate = candidates[0];
            if (units.Count == MaximumUnits)
                throw new InvalidDataException("Overlay unit count exceeds 4096.");
            units.Add(new DosOverlayUnit(units.Count + 1,
                resident.LoadImageFileOffset + candidate.ResidentImageOffset,
                candidate.ResidentImageOffset, candidate.OverlayOffset,
                candidate.CodeBytes,
                checked(candidate.OverlayOffset + candidate.CodeBytes),
                candidate.FixupBytes, candidate.ProcedureCount,
                candidate.UnitNumber,
                "mz-resident-trap-marker;checked-procedure-stubs;unique-contiguous-extent"));
            next = checked(candidate.OverlayOffset + candidate.CodeBytes + candidate.FixupBytes);
        }
        if (units.Count == 0 || next != header.OriginalFileLength)
            throw new InvalidDataException(
                "No complete unambiguous overlay unit chain covers the original payload.");
        return new DosOverlayDirectory(header.Format, resident.SourceSha256,
            header.SourceSha256, header.HeaderBytes,
            header.OriginalFileLength - header.HeaderBytes,
            "resident-descriptors-and-stored-extents-validated;runtime-segments-unverified",
            units);
    }

    private sealed record Descriptor(int ResidentImageOffset, int OverlayOffset,
        int CodeBytes, int FixupBytes, int ProcedureCount, int UnitNumber);
}
