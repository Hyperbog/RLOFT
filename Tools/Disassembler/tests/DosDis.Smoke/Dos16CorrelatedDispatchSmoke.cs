using DosDis.Core;

internal static class Dos16CorrelatedDispatchSmoke
{
    private static void Check(bool condition, string label)
    {
        if (!condition)
            throw new Exception("x86-16 correlated dispatch: " + label);
    }

    private static (Dos16CodeRegion Region, Dos16FlowResult Graph)
        Guarded(byte[] setup, byte[] stride, ushort table = 0x120,
            byte[]? afterGuard = null, bool far = false)
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
        bytes[offset++] = checked((byte)((afterGuard?.Length ?? 0) +
            stride.Length + 5));
        if (afterGuard is not null)
        {
            afterGuard.CopyTo(bytes, offset);
            offset += afterGuard.Length;
        }
        stride.CopyTo(bytes, offset);
        offset += stride.Length;
        // CS:[BX+positive-disp16], /4 near or /5 far indirect JMP.
        bytes[offset++] = 0x2E;
        bytes[offset++] = 0xFF;
        bytes[offset++] = far ? (byte)0xAF : (byte)0xA7;
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

        // A known BX before an unsupported memory/segment operation is
        // not sufficient to infer a later table index: the transfer
        // invalidates all register values, independently of guard proof.
        var memoryClobber = Guarded(new byte[] {
            0xBB, 0x01, 0x00, 0x88, 0x07 // MOV BX,1; MOV [BX],AL
        }, new byte[] { 0xD1, 0xE3 });
        var memoryReport = Dos16CorrelatedDispatchRecovery.Inspect(
            memoryClobber.Region, memoryClobber.Graph);
        Check(!memoryReport.Incomplete &&
            memoryReport.GuardedStorageSites == 1 &&
            memoryReport.BoundedIndexSites == 0 &&
            memoryReport.Sites[0].CandidateStoredEntries.Count == 0,
            "unmodeled memory write invalidates the apparent finite BX index");

        // The transparent guard-to-scaling bridge must contain no
        // unsupported memory-side effects, even if a later MOV happens
        // to set BX back to a small literal value.
        var corruptedPath = Guarded(
            new byte[] { 0xBB, 0x01, 0x00 },
            new byte[] { 0xD1, 0xE3 },
            afterGuard: new byte[] { 0x88, 0x07 });
        var corruptedReport = Dos16CorrelatedDispatchRecovery.Inspect(
            corruptedPath.Region, corruptedPath.Graph);
        Check(!corruptedReport.Incomplete &&
            corruptedReport.GuardedStorageSites == 0 &&
            corruptedReport.BoundedIndexSites == 0,
            "memory mutation on guarded path invalidates unique transfer proof");

        // The same 16-bit source bytes with an alternate segment origin
        // must not be joined to a cached graph from the original window.
        var movedRegion = DosComImage.LoadExplicit(
            BinaryArtifact.FromBytes(good.Region.Source.Data.ToArray(),
                "synthetic-other-window.com"), 0x200).CreateCodeRegion();
        var moved = Dos16CorrelatedDispatchRecovery.Inspect(
            movedRegion, good.Graph);
        Check(moved.Incomplete && moved.Sites.Count == 0,
            "same source bytes cannot reuse candidate edges from a different origin");

        // A far indexed pointer needs MZ relocation and a matching
        // rebased segment view. Supplying source bytes alone does not
        // manufacture an intersegment target or allow runtime selectors.
        var farNoFixup = Guarded(new byte[] { 0xBB, 0x01, 0x00 },
            new byte[] { 0xC1, 0xE3, 0x02 }, far: true);
        var farReport = Dos16CorrelatedDispatchRecovery.Inspect(
            farNoFixup.Region, farNoFixup.Graph);
        Check(!farReport.Incomplete &&
            farReport.GuardedStorageSites == 0 &&
            farReport.BoundedIndexSites == 0 &&
            farReport.Sites[0].CandidateStoredEntries.Count == 0,
            "far pointer storage without MZ selector fixups stays unresolved");

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
