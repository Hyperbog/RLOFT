using DosDis.Core;

internal static class Dos16CorrelatedDispatchSmoke
{
    private static void Check(bool condition, string label)
    {
        if (!condition)
            throw new Exception("x86-16 correlated dispatch: " + label);
    }

    private static (Dos16CodeRegion Region, Dos16FlowResult Graph)
        Guarded(byte[] setup, byte[] stride, ushort table = 0x120)
    {
        byte[] bytes = new byte[0x50];
        int offset = 0;
        setup.CopyTo(bytes, offset);
        offset += setup.Length;
        // CMP BX,2; JA rejection. Fallthrough is the only accepted
        // unsigned range 0..2. Branch goes to the RET after dispatch.
        byte[] cmp = { 0x83, 0xFB, 0x02 };
        cmp.CopyTo(bytes, offset);
        offset += cmp.Length;
        bytes[offset++] = 0x77; // JA rel8 (reject values above 2)
        bytes[offset++] = checked((byte)(stride.Length + 5));
        stride.CopyTo(bytes, offset);
        offset += stride.Length;
        // CS:[BX+positive-disp16], /4 indirect near JMP.
        bytes[offset++] = 0x2E;
        bytes[offset++] = 0xFF;
        bytes[offset++] = 0xA7;
        bytes[offset++] = (byte)table;
        bytes[offset++] = (byte)(table >> 8);
        bytes[offset] = 0xC3;
        // Three original-file-backed near pointers, outside decoded code.
        bytes[0x20] = 0x30; bytes[0x21] = 0x01;
        bytes[0x22] = 0x32; bytes[0x23] = 0x01;
        bytes[0x24] = 0x34; bytes[0x25] = 0x01;
        var region = DosComImage.LoadExplicit(BinaryArtifact.FromBytes(
            bytes, "synthetic-correlated-only.com")).CreateCodeRegion();
        return (region, Dos16ControlFlowAnalysis.Analyze(region, 0x100));
    }

    internal static void Run()
    {
        var good = Guarded(new byte[] { 0xBB, 0x01, 0x00 },
            new byte[] { 0xD1, 0xE3 });
        var goodReport = Dos16CorrelatedDispatchRecovery.Inspect(
            good.Region, good.Graph);
        Check(!goodReport.Incomplete && goodReport.InspectedSites == 1 &&
            goodReport.GuardedStorageSites == 1 &&
            goodReport.BoundedIndexSites == 1 &&
            goodReport.UnresolvedSites == 0 &&
            goodReport.CodeDataConflicts == 0,
            "JA guard, exact BX doubling and stored pointer all correlate");
        var first = goodReport.Sites.Single();
        Check(first.GuardedEntryCount == 3 &&
            first.GuardStatus == "candidate-guarded-CS-BX-table" &&
            first.Status == "candidate-guarded-and-finite-index-stored-pointers" &&
            first.CandidateIndices.SequenceEqual(new[] { 1 }) &&
            first.CandidateStoredEntries.Count == 1 &&
            first.CandidateStoredEntries[0].Index == 1 &&
            first.CandidateStoredEntries[0].Target.Offset == 0x132 &&
            goodReport.AnalysisStatus.Contains("no-CFG-or-function-promotion"),
            "finite BX=2 selects only stored entry one; execution unproven");

        var add = Guarded(new byte[] { 0xBB, 0x02, 0x00 },
            new byte[] { 0x01, 0xDB });
        var addReport = Dos16CorrelatedDispatchRecovery.Inspect(
            add.Region, add.Graph);
        Check(!addReport.Incomplete && addReport.BoundedIndexSites == 1 &&
            addReport.Sites[0].CandidateIndices.SequenceEqual(new[] { 2 }),
            "ADD BX,BX uses exact correlated unary doubling");

        var alternateAdd = Guarded(new byte[] { 0xBB, 0x01, 0x00 },
            new byte[] { 0x03, 0xDB });
        Check(Dos16CorrelatedDispatchRecovery.Inspect(
            alternateAdd.Region, alternateAdd.Graph).BoundedIndexSites == 1,
            "opposite ADD encoding has identical correlated semantics");

        var c1Shift = Guarded(new byte[] { 0xBB, 0x01, 0x00 },
            new byte[] { 0xC1, 0xE3, 0x01 });
        Check(Dos16CorrelatedDispatchRecovery.Inspect(
            c1Shift.Region, c1Shift.Graph).BoundedIndexSites == 1,
            "SHL BX,imm8=1 is independently decoded and bounded");

        var unknown = Guarded(new byte[] { 0x90, 0x90, 0x90 },
            new byte[] { 0xD1, 0xE3 });
        var unknownReport = Dos16CorrelatedDispatchRecovery.Inspect(
            unknown.Region, unknown.Graph);
        Check(!unknownReport.Incomplete &&
            unknownReport.GuardedStorageSites == 1 &&
            unknownReport.BoundedIndexSites == 0 &&
            unknownReport.Sites[0].Status ==
                "unresolved-guarded-storage-index:unresolved-BX-not-small-exact-set" &&
            unknownReport.Sites[0].CandidateStoredEntries.Count == 0,
            "valid guard and stored table alone cannot invent runtime index");

        var outOfBounds = Guarded(new byte[] { 0xBB, 0x04, 0x00 },
            new byte[] { 0xD1, 0xE3 });
        var outReport = Dos16CorrelatedDispatchRecovery.Inspect(
            outOfBounds.Region, outOfBounds.Graph);
        Check(!outReport.Incomplete &&
            outReport.BoundedIndexSites == 0 &&
            outReport.Sites[0].CandidateStoredEntries.Count == 0,
            "a candidate path inconsistent with the table count is unresolved");

        var stale = Dos16CorrelatedDispatchRecovery.Inspect(
            good.Region, good.Graph with { Truncated = true });
        Check(stale.Incomplete && stale.Sites.Count == 0,
            "truncated CFG cannot authorize any combined dispatch evidence");

        var forged = good.Graph with
        {
            Edges = good.Graph.Edges.Select(x =>
                x.Kind == "branch-taken"
                    ? x with { Destination = new Dos16Address(0, 0x100) }
                    : x).ToArray()
        };
        var forgedReport = Dos16CorrelatedDispatchRecovery.Inspect(
            good.Region, forged);
        Check(forgedReport.Incomplete && forgedReport.Sites.Count == 0,
            "forged unsigned conditional edge blocks all correlations");

        var overlapping = Guarded(new byte[] { 0xBB, 0x01, 0x00 },
            new byte[] { 0xD1, 0xE3 }, table: 0x10A);
        var conflict = Dos16CorrelatedDispatchRecovery.Inspect(
            overlapping.Region, overlapping.Graph);
        Check(!conflict.Incomplete && conflict.GuardedStorageSites == 0 &&
            conflict.BoundedIndexSites == 0 &&
            conflict.CodeDataConflicts == 1,
            "candidate decoded dispatch bytes cannot be table storage");

        Console.WriteLine(
            "PASS: independent unsigned-guard and finite BX indexed pointer correlation");
    }
}
