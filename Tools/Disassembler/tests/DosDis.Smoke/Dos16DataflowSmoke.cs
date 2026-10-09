using System.Buffers.Binary;
using DosDis.Core;

internal static class Dos16DataflowSmoke
{
    private static void Check(bool condition, string detail)
    {
        if (!condition) throw new Exception("x86-16 worklist: " + detail);
    }

    private static Dos16CodeRegion Region(params byte[] bytes) =>
        DosComImage.LoadExplicit(BinaryArtifact.FromBytes(
            bytes, "synthetic-only.com")).CreateCodeRegion();

    private static Dos16DataflowState At(Dos16DataflowReport report, int offset) =>
        report.States.Single(x => x.Address.Offset == 0x100 + offset);

    internal static void Run()
    {
        // Two candidate branch paths set AX independently, then meet at
        // MOV BX,AX. The conditional decision is deliberately unknown.
        var joinedRegion = Region(
            0xB8, 0x01, 0x00,       // 00 MOV AX,1
            0x3D, 0x00, 0x00,       // 03 CMP AX,0
            0x74, 0x05,             // 06 JE 0x10D (candidate taken)
            0xB8, 0x02, 0x00,       // 08 MOV AX,2
            0xEB, 0x03,             // 11 JMP 0x110
            0xB8, 0x03, 0x00,       // 13 MOV AX,3
            0x89, 0xC3,             // 16 MOV BX,AX
            0xC3);                 // 18 RET
        var joinedGraph = Dos16ControlFlowAnalysis.Analyze(joinedRegion, 0x100);
        var joined = Dos16DataflowAnalysis.Analyze(joinedRegion, joinedGraph);
        Check(!joined.Incomplete && joined.States.Count == 8,
            "bounded joined path yields all backed candidate nodes");
        var before = At(joined, 16).Before.Get(Dos16WordRegister.AX);
        var after = At(joined, 16).After.Get(Dos16WordRegister.BX);
        Check(before.Kind == Dos16ValueKind.FiniteSet &&
            before.Members.SequenceEqual(new ushort[] { 2, 3 }),
            "conditional join retains both possible AX values");
        Check(after.Kind == Dos16ValueKind.FiniteSet &&
            after.Members.SequenceEqual(new ushort[] { 2, 3 }),
            "joined register copy retains both possible BX values");
        Check(joined.MergeChanges >= 1 && joined.ProcessedSteps >= 8 &&
            joined.SourceSha256 == joinedRegion.SourceSha256 &&
            joined.AnalysisStatus.Contains("candidate-unverified"),
            "join accounting and candidate-only provenance");

        // A backedge forms an increasing modular register sequence.
        // Once the finite domain exceeds the explicit revision budget,
        // widening must converge to Unknown (not a guessed loop count).
        var loopRegion = Region(
            0xB8, 0x00, 0x00,       // MOV AX,0
            0x05, 0x01, 0x00,       // ADD AX,1
            0xEB, 0xFB);            // JMP back to ADD
        var loopGraph = Dos16ControlFlowAnalysis.Analyze(loopRegion, 0x100);
        var loop = Dos16DataflowAnalysis.Analyze(loopRegion, loopGraph);
        Check(!loop.Incomplete && loop.States.Count == 3 &&
            loop.WidenedRegisterFacts > 0 &&
            At(loop, 3).Before.Get(Dos16WordRegister.AX).Kind ==
                Dos16ValueKind.Unknown,
            "loop widening converges conservatively to Unknown");

        var budget = Dos16DataflowAnalysis.Analyze(loopRegion, loopGraph,
            maximumSteps: 2);
        Check(budget.Incomplete && budget.States.Count == 0 &&
            budget.AnalysisStatus == "incomplete-worklist-step-budget",
            "exhausted processing budget cannot publish partial register facts");
        var graphBudget = Dos16DataflowAnalysis.Analyze(joinedRegion, joinedGraph,
            maximumInstructions: 4);
        Check(graphBudget.Incomplete && graphBudget.States.Count == 0,
            "bounded instruction inventory does not silently trim a graph");
        var edgeBudget = Dos16DataflowAnalysis.Analyze(joinedRegion, joinedGraph,
            maximumEdges: 1);
        Check(edgeBudget.Incomplete && edgeBudget.States.Count == 0,
            "bounded edge inventory cannot silently omit predecessors");
        var truncated = Dos16DataflowAnalysis.Analyze(joinedRegion,
            joinedGraph with { Truncated = true });
        Check(truncated.Incomplete && truncated.States.Count == 0,
            "truncated original CFG blocks any dataflow publication");

        var missing = Dos16DataflowAnalysis.Analyze(joinedRegion,
            joinedGraph with { Edges = joinedGraph.Edges.Skip(1).ToArray() });
        Check(missing.Incomplete && missing.AnalysisStatus ==
            "rejected-missing-or-extra-graph-edge" &&
            missing.States.Count == 0,
            "omitted graph edge cannot fake a unique incoming register value");

        var forged = joinedGraph.Edges.Select(edge =>
            edge.Kind == "branch-taken"
                ? edge with { Destination = new Dos16Address(0, 0x108) }
                : edge).ToArray();
        var forgedReport = Dos16DataflowAnalysis.Analyze(joinedRegion,
            joinedGraph with { Edges = forged });
        Check(forgedReport.Incomplete && forgedReport.States.Count == 0 &&
            forgedReport.AnalysisStatus == "rejected-forged-graph-edge",
            "branch direction must match source-backed relative operand");

        var spoofed = joinedGraph.Instructions.Select(instruction =>
            instruction.Address.Offset == 0x100
                ? instruction with { OriginalBytes = "B8FFFF" }
                : instruction).ToArray();
        Check(Dos16DataflowAnalysis.Analyze(joinedRegion,
                joinedGraph with { Instructions = spoofed }).Incomplete,
            "stale original instruction bytes cannot generate values");

        var duplicate = joinedGraph.Instructions.Concat(new[]
        {
            joinedGraph.Instructions[0]
        }).ToArray();
        Check(Dos16DataflowAnalysis.Analyze(joinedRegion,
                joinedGraph with { Instructions = duplicate }).Incomplete,
            "duplicate original instruction starts are rejected");

        var missingNode = joinedGraph.Instructions.Where(x =>
            x.Address.Offset != 0x110).ToArray();
        Check(Dos16DataflowAnalysis.Analyze(joinedRegion,
                joinedGraph with { Instructions = missingNode }).Incomplete,
            "an undecoded branch destination cannot silently disappear");

        var malformed = Dos16DataflowAnalysis.Analyze(joinedRegion,
            joinedGraph with { Instructions = null! });
        Check(malformed.Incomplete && malformed.States.Count == 0 &&
            malformed.AnalysisStatus == "rejected-malformed-graph",
            "deserialized null instruction inventory fails closed");
        var malformedEdge = Dos16DataflowAnalysis.Analyze(joinedRegion,
            joinedGraph with
            {
                Edges = joinedGraph.Edges.Concat(new Dos16FlowEdge[] { null! })
                    .ToArray()
            });
        Check(malformedEdge.Incomplete && malformedEdge.States.Count == 0,
            "deserialized null edge fails closed");

        var otherRegion = Region(0x90, 0xC3);
        var wrongSource = Dos16DataflowAnalysis.Analyze(otherRegion, joinedGraph);
        Check(wrongSource.Incomplete &&
            wrongSource.AnalysisStatus == "rejected-source-or-mapping-mismatch",
            "same offset in a different hash cannot borrow candidate values");

        // A direct CALL/INT must not leak old register values into
        // speculative continuation blocks or the candidate callee.
        var callRegion = Region(
            0xBB, 0x34, 0x12, // MOV BX,1234h
            0xE8, 0x03, 0x00, // CALL near 0x109
            0x89, 0xD8,       // MOV AX,BX (speculative return)
            0xC3,             // RET
            0xC3);            // callee RET
        var callReport = Dos16DataflowAnalysis.Analyze(callRegion,
            Dos16ControlFlowAnalysis.Analyze(callRegion, 0x100));
        Check(!callReport.Incomplete &&
            At(callReport, 6).Before.Get(Dos16WordRegister.BX).Kind ==
                Dos16ValueKind.Unknown &&
            At(callReport, 6).After.Get(Dos16WordRegister.AX).Kind ==
                Dos16ValueKind.Unknown &&
            At(callReport, 9).Before.Get(Dos16WordRegister.BX).Kind ==
                Dos16ValueKind.Unknown,
            "neither candidate call edge retains caller register assumptions");

        var interruptRegion = Region(
            0xBB, 0x34, 0x12, // MOV BX,1234h
            0xCD, 0x21,       // INT 21h
            0x89, 0xD8,       // MOV AX,BX (candidate interrupt return)
            0xC3);
        var interruptReport = Dos16DataflowAnalysis.Analyze(interruptRegion,
            Dos16ControlFlowAnalysis.Analyze(interruptRegion, 0x100));
        Check(!interruptReport.Incomplete &&
            At(interruptReport, 5).Before.Get(Dos16WordRegister.BX).Kind ==
                Dos16ValueKind.Unknown &&
            At(interruptReport, 5).After.Get(Dos16WordRegister.AX).Kind ==
                Dos16ValueKind.Unknown,
            "interrupt return is speculative and cannot preserve registers");

        // Valid prefixed Jcc is deliberately unsupported by the minimal
        // transfer subset. Decoder-backed CFG edges alone do not authorize
        // retention of register hypotheses through a prefixed branch.
        var prefixedRegion = Region(
            0xBB, 0x34, 0x12, // MOV BX,1234h
            0x2E, 0x74, 0x00, // CS: JE +0 (prefix makes transfer unsupported)
            0x89, 0xD8,       // MOV AX,BX
            0xC3);
        var prefixedReport = Dos16DataflowAnalysis.Analyze(prefixedRegion,
            Dos16ControlFlowAnalysis.Analyze(prefixedRegion, 0x100));
        Check(!prefixedReport.Incomplete &&
            At(prefixedReport, 6).Before.Get(Dos16WordRegister.BX).Kind ==
                Dos16ValueKind.Unknown,
            "prefix-bearing conditional branch does not retain unsafe facts");

        // The MZ loader permits a full 64 KiB candidate code window.
        // A branch physically ending at IP FFFF must not make an edge
        // wrapped to 0002 look like a proven nonwrapped file offset.
        byte[] highIpBytes = new byte[0x10020];
        highIpBytes[0] = (byte)'M';
        highIpBytes[1] = (byte)'Z';
        BinaryPrimitives.WriteUInt16LittleEndian(
            highIpBytes.AsSpan(2, 2), 32); // 129 pages, final page 32 bytes
        BinaryPrimitives.WriteUInt16LittleEndian(
            highIpBytes.AsSpan(4, 2), 129);
        BinaryPrimitives.WriteUInt16LittleEndian(
            highIpBytes.AsSpan(8, 2), 2); // 32-byte header, 65536-byte load
        highIpBytes[0x20 + 2] = 0xC3;
        highIpBytes[0x20 + 0xFFFE] = 0xEB;
        highIpBytes[0x20 + 0xFFFF] = 0x02; // runtime IP wraps from 10000 to 0002
        var highIpRegion = DosMzImage.Load(BinaryArtifact.FromBytes(
            highIpBytes, "high-ip-synthetic.exe")).CreateEntryCodeRegion();
        var highIpGraph = Dos16ControlFlowAnalysis.Analyze(highIpRegion, 0xFFFE);
        var highIpReport = Dos16DataflowAnalysis.Analyze(highIpRegion, highIpGraph);
        Check(highIpReport.Incomplete && highIpReport.States.Count == 0 &&
            highIpReport.AnalysisStatus == "rejected-wrapped-near16-relative-target",
            "wrapped 16-bit relative JMP cannot publish dataflow");

        Console.WriteLine(
            "PASS: bounded x86-16 joins, loop widening and graph-integrity rejection");
    }
}
