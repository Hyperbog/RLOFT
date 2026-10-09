namespace DosDis.Core;

/// <summary>
/// Candidate static BX byte offsets into independently source-validated
/// CS:[BX+disp16] pointer storage. Never verified execution destinations.
/// </summary>
public sealed record Dos16TableIndexSiteEvidence(
    Dos16Address Site, string Status, string StorageValidationStatus,
    string PointerKind, ushort Table, int DeclaredEntries,
    IReadOnlyList<int> PossibleIndices,
    IReadOnlyList<Dos16TableEntry> CandidateStoredEntries, string Evidence);

public sealed record Dos16TableIndexEvidenceReport(
    string SourceSha256, string MappingEvidence, string AnalysisStatus,
    bool Incomplete, int CandidateSites, int UnresolvedSites,
    int CodeDataConflicts, IReadOnlyList<Dos16TableIndexSiteEvidence> Sites);

/// <summary>
/// Read-only intersection of independently mapped pointer storage and
/// conservative candidate-flow register values. This never writes edges,
/// converts stored target bytes into code, or infers actual runtime memory.
/// </summary>
public static class Dos16TableIndexEvidence
{
    public static Dos16TableIndexEvidenceReport Inspect(
        Dos16CodeRegion region, Dos16FlowResult graph, Dos16TableManifest manifest,
        DosMzImage? mz = null, DosMzRebasedView? simulation = null)
    {
        ArgumentNullException.ThrowIfNull(region);
        ArgumentNullException.ThrowIfNull(graph);
        ArgumentNullException.ThrowIfNull(manifest);

        // Validate the entire candidate graph independently before passing
        // it to the existing table validator. This avoids trusting malformed
        // duplicates, stale edges, incomplete predecessor inventories, etc.
        var flow = Dos16DataflowAnalysis.Analyze(region, graph);
        if (flow.Incomplete)
            return new Dos16TableIndexEvidenceReport(region.SourceSha256,
                region.MappingEvidence, "incomplete-register-flow;" +
                flow.AnalysisStatus, true, 0, 0, 0,
                Array.Empty<Dos16TableIndexSiteEvidence>());

        // The manifest's SHA, explicit count, near/far layout, original
        // bytes, MZ fixups and code/data exclusions are validated by the
        // existing independent storage validator, not by this value pass.
        var stored = Dos16DispatchTableRecovery.Inspect(
            region, graph, manifest, mz, simulation);
        var beforeBySite = flow.States.ToDictionary(x => x.Address, x => x.Before);
        var results = new List<Dos16TableIndexSiteEvidence>();

        foreach (var finding in stored.Findings)
        {
            string status = "unresolved-stored-table";
            string evidence = "independently-validated-pointer-storage-required";
            IReadOnlyList<int> indices = Array.Empty<int>();
            IReadOnlyList<Dos16TableEntry> entries = Array.Empty<Dos16TableEntry>();

            if (finding.Status == "candidate-entries-source-backed" &&
                beforeBySite.TryGetValue(finding.Site, out var before))
            {
                int stride = finding.PointerKind == "near16" ? 2 : 4;
                var bx = before.Get(Dos16WordRegister.BX);

                // Exact finite values only. Unsigned intervals can include
                // other byte offsets, even if their endpoints are aligned.
                // No narrowing is inferred from a CMP/branch without flags.
                if (bx.Kind is Dos16ValueKind.Constant or Dos16ValueKind.FiniteSet)
                {
                    var byteOffsets = bx.Members;
                    var proposed = byteOffsets.Select(x => (int)x / stride)
                        .Distinct().OrderBy(x => x).ToArray();
                    bool valid = byteOffsets.Count > 0 &&
                        byteOffsets.Count <= Dos16AbstractValue.MaximumFiniteMembers &&
                        byteOffsets.All(x => x % stride == 0 &&
                            x / stride < finding.RequestedCount) &&
                        proposed.All(index => finding.Entries.Count > index &&
                            finding.Entries[index].Index == index &&
                            finding.Entries[index].PointerFileOffset ==
                                (long)region.FileOffset + finding.Table -
                                region.Origin + (long)index * stride);
                    if (valid)
                    {
                        indices = Array.AsReadOnly(proposed);
                        entries = Array.AsReadOnly(proposed.Select(index =>
                            finding.Entries[index]).ToArray());
                        status = "candidate-bounded-byte-offset-and-stored-entries";
                        evidence = "source-backed-index-site;finite-BX-byte-offset;" +
                            "external-entry-bound;immutable-pointer-bytes;" +
                            "no-runtime-index-or-memory-write-proof;no-CFG-promotion";
                    }
                    else
                    {
                        status = "unresolved-index-misaligned-or-out-of-bounds";
                        evidence = "candidate-BX-offset-cannot-index-all-validated-entries";
                    }
                }
                else
                {
                    status = "unresolved-BX-not-small-exact-set";
                    evidence = "unknown-or-interval-BX-not-sufficient-for-table-index";
                }
            }
            else if (finding.Status == "candidate-entries-source-backed")
            {
                status = "unresolved-index-site-no-flow-state";
                evidence = "site-not-reached-in-candidate-entry-flow";
            }
            results.Add(new Dos16TableIndexSiteEvidence(finding.Site,
                status, finding.Status, finding.PointerKind, finding.Table,
                finding.RequestedCount, indices, entries, evidence));
        }
        int accepted = results.Count(x =>
            x.Status == "candidate-bounded-byte-offset-and-stored-entries");
        return new Dos16TableIndexEvidenceReport(region.SourceSha256,
            region.MappingEvidence,
            "candidate-unverified;external-bound;no-CFG-or-runtime-target-promotion",
            false, accepted, results.Count - accepted,
            results.Count(x => x.StorageValidationStatus == "rejected-code-data-overlap"),
            results.AsReadOnly());
    }
}
