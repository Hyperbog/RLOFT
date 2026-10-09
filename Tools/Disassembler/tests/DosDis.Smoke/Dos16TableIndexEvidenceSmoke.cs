using DosDis.Core;

internal static class Dos16TableIndexEvidenceSmoke
{
    private static void Check(bool condition, string label)
    {
        if (!condition) throw new Exception("x86-16 table evidence: " + label);
    }

    private static (Dos16CodeRegion Region, Dos16FlowResult Graph,
        Dos16TableManifest Manifest) Fixture(byte[] before,
        ushort table = 0x110)
    {
        byte[] bytes = new byte[0x40];
        before.CopyTo(bytes, 0);
        int site = before.Length;
        // Unprefixed x86-16 CS:[BX+disp16] indirect near JMP,
        // mod=10 /4 r/m=111, exact source-backed displacement.
        bytes[site] = 0x2E;
        bytes[site + 1] = 0xFF;
        bytes[site + 2] = 0xA7;
        bytes[site + 3] = (byte)table;
        bytes[site + 4] = (byte)(table >> 8);
        bytes[0x10] = 0x20; bytes[0x11] = 0x01; // 0x120
        bytes[0x12] = 0x22; bytes[0x13] = 0x01; // 0x122
        var artifact = BinaryArtifact.FromBytes(bytes, "synthetic-only.bin");
        var region = DosComImage.LoadExplicit(artifact).CreateCodeRegion();
        var graph = Dos16ControlFlowAnalysis.Analyze(
            region, checked((ushort)0x100));
        var manifest = new Dos16TableManifest(1, artifact.Sha256,
            new[] { new Dos16TableRequest(checked((ushort)(0x100 + site)),
                table, 2, "near16", "synthetic-source-bound-analyst-count") });
        return (region, graph, manifest);
    }

    internal static void Run()
    {
        var good = Fixture(new byte[] { 0xBB, 0x02, 0x00 });
        var report = Dos16TableIndexEvidence.Inspect(
            good.Region, good.Graph, good.Manifest);
        Check(!report.Incomplete && report.CandidateSites == 1 &&
            report.UnresolvedSites == 0 && report.Sites.Count == 1,
            "exact CS indexed site with independent analyst bound");
        var site = report.Sites[0];
        Check(site.StorageValidationStatus == "candidate-entries-source-backed" &&
            report.CodeDataConflicts == 0,
            "independent storage evidence is retained in accepted report");
        Check(site.Status == "candidate-bounded-byte-offset-and-stored-entries" &&
            site.PossibleIndices.SequenceEqual(new[] { 1 }) &&
            site.CandidateStoredEntries.Count == 1 &&
            site.CandidateStoredEntries[0].Index == 1 &&
            site.CandidateStoredEntries[0].PointerFileOffset == 0x12 &&
            site.CandidateStoredEntries[0].Target.Offset == 0x122 &&
            report.AnalysisStatus.Contains("no-CFG-or-runtime-target-promotion"),
            "stored target remains non-executable candidate with exact pointer offset");

        var first = Fixture(new byte[] { 0xBB, 0x00, 0x00 });
        var firstReport = Dos16TableIndexEvidence.Inspect(
            first.Region, first.Graph, first.Manifest);
        Check(firstReport.CandidateSites == 1 &&
            firstReport.Sites[0].PossibleIndices.SequenceEqual(new[] { 0 }),
            "zero-byte BX offset selects first stored pointer");

        var odd = Fixture(new byte[] { 0xBB, 0x01, 0x00 });
        var oddReport = Dos16TableIndexEvidence.Inspect(
            odd.Region, odd.Graph, odd.Manifest);
        Check(oddReport.CandidateSites == 0 &&
            oddReport.Sites[0].Status == "unresolved-index-misaligned-or-out-of-bounds" &&
            oddReport.Sites[0].CandidateStoredEntries.Count == 0,
            "unaligned BX byte offset does not fabricate an indexed entry");

        var pastEnd = Fixture(new byte[] { 0xBB, 0x04, 0x00 });
        var pastEndReport = Dos16TableIndexEvidence.Inspect(
            pastEnd.Region, pastEnd.Graph, pastEnd.Manifest);
        Check(pastEndReport.CandidateSites == 0 &&
            pastEndReport.Sites[0].CandidateStoredEntries.Count == 0,
            "BX beyond independently declared count is unresolved");

        var unknown = Fixture(new byte[] { 0xB0, 0x09 });
        var unknownReport = Dos16TableIndexEvidence.Inspect(
            unknown.Region, unknown.Graph, unknown.Manifest);
        Check(unknownReport.CandidateSites == 0 &&
            unknownReport.Sites[0].Status == "unresolved-BX-not-small-exact-set",
            "unsupported byte register alias cannot seed a table index");

        // A control-flow join is permitted only when EVERY predecessor
        // gives a small exact and aligned BX byte offset. A single unsafe
        // incoming path must poison the entire joined value.
        (Dos16CodeRegion Region, Dos16FlowResult Graph,
            Dos16TableManifest Manifest) JoinedFixture(bool killAlternate)
        {
            byte[] bytes = new byte[0x50];
            byte[] instructions = {
                0xBB, 0x00, 0x00, // 00 MOV BX,0
                0x83, 0xFB, 0x00, // 03 CMP BX,0
                0x74, 0x05,       // 06 JE 0x10D (unknown direction)
                0xBB, 0x00, 0x00, // 08 MOV BX,0
                0xEB, 0x03,       // 11 JMP 0x110
                0xBB, 0x02, 0x00, // 13 MOV BX,2 or unsupported bytes
                0x2E, 0xFF, 0xA7, 0x20, 0x01 // 16 JMP CS:[BX+0120h]
            };
            if (killAlternate)
            {
                instructions[13] = 0xB0;
                instructions[14] = 0x7F;
                instructions[15] = 0x90;
            }
            instructions.CopyTo(bytes, 0);
            bytes[0x20] = 0x30; bytes[0x21] = 0x01;
            bytes[0x22] = 0x32; bytes[0x23] = 0x01;
            var image = BinaryArtifact.FromBytes(bytes, "synthetic-join.com");
            var region = DosComImage.LoadExplicit(image).CreateCodeRegion();
            var graph = Dos16ControlFlowAnalysis.Analyze(region, 0x100);
            var manifest = new Dos16TableManifest(1, image.Sha256,
                new[] { new Dos16TableRequest(0x110, 0x120, 2,
                    "near16", "synthetic-merged-branch-count") });
            return (region, graph, manifest);
        }

        var both = JoinedFixture(killAlternate: false);
        var bothReport = Dos16TableIndexEvidence.Inspect(
            both.Region, both.Graph, both.Manifest);
        Check(!bothReport.Incomplete && bothReport.CandidateSites == 1 &&
            bothReport.Sites[0].PossibleIndices.SequenceEqual(new[] { 0, 1 }) &&
            bothReport.Sites[0].CandidateStoredEntries.Count == 2,
            "two backed incoming byte offsets remain distinct and bounded");

        var poisoned = JoinedFixture(killAlternate: true);
        var poisonedReport = Dos16TableIndexEvidence.Inspect(
            poisoned.Region, poisoned.Graph, poisoned.Manifest);
        Check(!poisonedReport.Incomplete && poisonedReport.CandidateSites == 0 &&
            poisonedReport.Sites[0].Status == "unresolved-BX-not-small-exact-set" &&
            poisonedReport.Sites[0].CandidateStoredEntries.Count == 0,
            "one unsupported incoming path invalidates all table index evidence");

        var forgedEdges = good.Graph.Edges.Select(x =>
            x with { Kind = "branch" }).ToArray();
        var unsafeReport = Dos16TableIndexEvidence.Inspect(good.Region,
            good.Graph with { Edges = forgedEdges }, good.Manifest);
        Check(unsafeReport.Incomplete && unsafeReport.CandidateSites == 0 &&
            unsafeReport.Sites.Count == 0,
            "stale or forged CFG blocks any value-table intersection");

        var incompleteReport = Dos16TableIndexEvidence.Inspect(good.Region,
            good.Graph with { Truncated = true }, good.Manifest);
        Check(incompleteReport.Incomplete && incompleteReport.Sites.Count == 0,
            "truncated candidate graph cannot seed pointer selection");

        bool rejectedWrongManifest = false;
        try
        {
            Dos16TableIndexEvidence.Inspect(good.Region, good.Graph,
                good.Manifest with { SourceSha256 = new string('a', 64) });
        }
        catch (InvalidDataException) { rejectedWrongManifest = true; }
        Check(rejectedWrongManifest,
            "cross-source externally supplied table manifest rejected");

        var overlap = Fixture(new byte[] { 0xBB, 0x00, 0x00 }, 0x103);
        var conflict = Dos16TableIndexEvidence.Inspect(
            overlap.Region, overlap.Graph, overlap.Manifest);
        Check(!conflict.Incomplete && conflict.CandidateSites == 0 &&
            conflict.CodeDataConflicts == 1 &&
            conflict.Sites[0].Status == "unresolved-stored-table" &&
            conflict.Sites[0].StorageValidationStatus == "rejected-code-data-overlap" &&
            conflict.Sites[0].CandidateStoredEntries.Count == 0,
            "code/data overlap is separately recorded and cannot seed a target");

        Console.WriteLine(
            "PASS: bounded source-backed x86-16 table-index candidate evidence");
    }
}
