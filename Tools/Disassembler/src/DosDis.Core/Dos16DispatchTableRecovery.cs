using System.Buffers.Binary;
using System.Text;
using System.Text.Json;

namespace DosDis.Core;

public sealed record Dos16TableRequest(ushort Site, ushort Table, int Count, string PointerKind, string EvidenceId);
public sealed record Dos16TableManifest(int SchemaVersion, string SourceSha256, IReadOnlyList<Dos16TableRequest> Tables);
public sealed record Dos16TableEntry(int Index, long PointerFileOffset, Dos16Address Target, long TargetFileOffset, int? FixupOrdinal);
public sealed record Dos16TableFinding(Dos16Address Site, ushort Table, int RequestedCount, string PointerKind,
    string Status, string Evidence, IReadOnlyList<Dos16TableEntry> Entries);
public sealed record Dos16TableReport(string SourceSha256, string AnalysisStatus,
    int Requested, int Candidates, IReadOnlyList<Dos16TableFinding> Findings);

/// <summary>
/// Requires an independently supplied, hash-bound count. Entries are storage
/// candidates, never proven execution paths or automatic CFG seeds.
/// </summary>
public static class Dos16DispatchTableRecovery
{
    public static Dos16TableManifest ReadManifest(string path)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);
        if (new FileInfo(path).Length > 1024 * 1024)
            throw new InvalidDataException("Dispatch manifest exceeds 1 MiB.");
        return ParseManifest(File.ReadAllText(path));
    }

    public static Dos16TableManifest ParseManifest(string json)
    {
        ArgumentNullException.ThrowIfNull(json);
        if (Encoding.UTF8.GetByteCount(json) > 1024 * 1024)
            throw new InvalidDataException("Dispatch manifest exceeds 1 MiB.");
        try
        {
            return JsonSerializer.Deserialize<Dos16TableManifest>(json,
                new JsonSerializerOptions(JsonSerializerDefaults.Web))
                ?? throw new InvalidDataException("Empty dispatch manifest.");
        }
        catch (JsonException ex) { throw new InvalidDataException("Malformed dispatch manifest.", ex); }
    }

    public static Dos16TableReport Inspect(Dos16CodeRegion region, Dos16FlowResult graph,
        Dos16TableManifest manifest, DosMzImage? mz = null, DosMzRebasedView? rebased = null)
    {
        ArgumentNullException.ThrowIfNull(region);
        ArgumentNullException.ThrowIfNull(graph);
        ArgumentNullException.ThrowIfNull(manifest);
        if (manifest.SchemaVersion != 1 || manifest.Tables is null ||
            manifest.Tables.Count is < 1 or > 128 ||
            manifest.SourceSha256 is not { Length: 64 } ||
            !manifest.SourceSha256.All(c => c is >= '0' and <= '9' or >= 'a' and <= 'f' or >= 'A' and <= 'F') ||
            !string.Equals(region.SourceSha256, manifest.SourceSha256, StringComparison.OrdinalIgnoreCase) ||
            !string.Equals(graph.SourceSha256, region.SourceSha256, StringComparison.OrdinalIgnoreCase) ||
            graph.FileOffset != region.FileOffset || graph.RegionByteLength != region.ByteLength ||
            graph.Entry.Segment != region.Segment || graph.MappingEvidence != region.MappingEvidence)
            throw new InvalidDataException("Dispatch manifest does not belong to this original mapped CFG.");
        if (rebased is not null && mz is null)
            throw new ArgumentException("Relocated view requires the matching original MZ.");
        if (mz is not null &&
            (!string.Equals(mz.SourceSha256, region.SourceSha256, StringComparison.OrdinalIgnoreCase) ||
             (rebased is not null &&
                (!string.Equals(rebased.Summary.SourceSha256, region.SourceSha256,
                    StringComparison.OrdinalIgnoreCase) ||
                 rebased.Summary.Writes.Count != mz.Relocations.Count))))
            throw new InvalidDataException("MZ relocation evidence belongs to a different source.");

        var knownSites = graph.Instructions.ToDictionary(i => i.Address.Offset);
        var fixupByOffset = new Dictionary<long, List<int>>();
        if (mz is not null)
            for (int i = 0; i < mz.Relocations.Count; i++)
            {
                long at = mz.Relocations[i].SourceFileOffset;
                if (!fixupByOffset.TryGetValue(at, out var indices))
                { indices = new List<int>(); fixupByOffset.Add(at, indices); }
                indices.Add(i);
            }
        List<int> Touching(long offset, int width)
        {
            var result = new List<int>();
            for (long p = offset - 1; p < offset + width; p++)
                if (fixupByOffset.TryGetValue(p, out var ordinals))
                    result.AddRange(ordinals);
            return result;
        }

        var findings = new List<Dos16TableFinding>();
        var seen = new HashSet<ushort>();
        foreach (var spec in manifest.Tables)
        {
            if (spec is null || spec.Count is < 2 or > 256 ||
                spec.PointerKind is not ("near16" or "far16") ||
                string.IsNullOrWhiteSpace(spec.EvidenceId) || spec.EvidenceId.Length > 512 ||
                !seen.Add(spec.Site))
                throw new InvalidDataException("Invalid or duplicate table specification.");
            int width = spec.PointerKind == "near16" ? 2 : 4;
            string status = "rejected-unsupported-site";
            var entries = new List<Dos16TableEntry>();
            if (knownSites.TryGetValue(spec.Site, out var site) &&
                site.Length == 5 && (site.FlowControl is "IndirectCall" or "IndirectBranch") &&
                site.Address.Segment == region.Segment &&
                site.Address.Offset == spec.Site &&
                spec.Site >= region.Origin && spec.Site < region.EndExclusive &&
                site.SourceFileOffset == (long)region.FileOffset +
                    spec.Site - region.Origin &&
                site.SourceFileOffset + 5 <= (long)region.FileOffset + region.ByteLength)
            {
                var op = region.Source.Data.Slice(checked((int)site.SourceFileOffset), 5);
                int slash = site.FlowControl == "IndirectCall" ?
                    (width == 2 ? 2 : 3) : (width == 2 ? 4 : 5);
                bool match = op[0] == 0x2E && op[1] == 0xFF &&
                    (op[2] >> 6) == 2 && ((op[2] >> 3) & 7) == slash &&
                    BinaryPrimitives.ReadUInt16LittleEndian(op.Slice(3, 2)) == spec.Table &&
                    site.OriginalBytes.Equals(Convert.ToHexString(op), StringComparison.OrdinalIgnoreCase);
                if (match)
                {
                    status = "rejected-unbacked-table-storage";
                    if (spec.Table >= region.Origin &&
                        (long)spec.Table + spec.Count * width <= region.EndExclusive)
                    {
                        bool complete = true;
                        for (int i = 0; i < spec.Count; i++)
                        {
                            long at = region.FileOffset + spec.Table - region.Origin + i * width;
                            var pointer = region.Source.Data.Slice(checked((int)at), width);
                            ushort ip = BinaryPrimitives.ReadUInt16LittleEndian(pointer.Slice(0, 2));
                            var touching = Touching(at, width);
                            Dos16Address target;
                            long targetFile;
                            int? ordinal = null;
                            if (width == 2)
                            {
                                if (touching.Count > 0 || ip < region.Origin || ip >= region.EndExclusive)
                                { complete = false; break; }
                                target = new Dos16Address(region.Segment, ip);
                                targetFile = region.FileOffset + ip - region.Origin;
                            }
                            else
                            {
                                if (mz is null || rebased is null || touching.Count != 1 ||
                                    mz.Relocations[touching[0]].SourceFileOffset != at + 2)
                                { complete = false; break; }
                                int n = touching[0];
                                var write = rebased!.Summary.Writes[n];
                                ushort stored = BinaryPrimitives.ReadUInt16LittleEndian(pointer.Slice(2, 2));
                                if (write.Ordinal != n || write.ValueBeforeWrite != stored)
                                { complete = false; break; }
                                var mapped = rebased.MapRuntimeFarPointer(write.ValueAfterWrite, ip);
                                if (mapped is null) { complete = false; break; }
                                target = new Dos16Address(mapped.RelativeSegment, ip);
                                targetFile = mapped.OriginalFileOffset;
                                ordinal = n;
                            }
                            entries.Add(new Dos16TableEntry(i, at, target, targetFile, ordinal));
                        }
                        status = complete ? "candidate-entries-source-backed" : "rejected-unverified-entry";
                        if (!complete) entries.Clear();
                    }
                }
            }
            findings.Add(new Dos16TableFinding(new Dos16Address(region.Segment, spec.Site),
                spec.Table, spec.Count, spec.PointerKind, status,
                "analyst-entry-bound-unverified;CS-indexed-FF-disp16;runtime-index-unknown;" +
                spec.EvidenceId, entries));
        }
        return new Dos16TableReport(region.SourceSha256,
            "candidate-unverified;no-CFG-reachability-promotion",
            findings.Count, findings.Count(x => x.Status == "candidate-entries-source-backed"), findings);
    }
}
