using Iced.Intel;

namespace DosDis.Core;

public readonly record struct Dos16Address(ushort Segment, ushort Offset);
public sealed record Dos16FlowInstruction(Dos16Address Address, int Length,
    long SourceFileOffset, string OriginalBytes, string Assembly, string FlowControl);
public sealed record Dos16FlowEdge(Dos16Address Source, Dos16Address? Destination,
    string Kind, string Evidence);
public sealed record Dos16FlowDiagnostic(Dos16Address Address,
    string Code, string Explanation);
public sealed record Dos16FlowResult(
    string SourceSha256, string MappingEvidence, string AnalysisStatus,
    Dos16Address Entry, int FileOffset, int RegionByteLength,
    int CandidateInstructionBytes, int UnknownFileBackedBytes,
    bool Truncated, IReadOnlyList<Dos16FlowInstruction> Instructions,
    IReadOnlyList<Dos16FlowEdge> Edges, IReadOnlyList<Dos16FlowDiagnostic> Diagnostics);

/// <summary>
/// Conservative x86-16 recursive reachability on one already mapped region.
/// No far/indirect target, DOS interrupt return, overlay fixup or segment
/// assignment is guessed. MZ/COM/overlay supply the exact same region type.
/// </summary>
public static class Dos16ControlFlowAnalysis
{
    public static Dos16FlowResult Analyze(Dos16CodeRegion region,
        ushort entry, int maxInstructions = 100_000)
    {
        ArgumentNullException.ThrowIfNull(region);
        if (maxInstructions is < 1 or > 1_000_000)
            throw new ArgumentOutOfRangeException(nameof(maxInstructions));
        if (entry < region.Origin || entry >= region.EndExclusive)
            throw new ArgumentOutOfRangeException(nameof(entry));

        int[] owners = new int[region.ByteLength];
        Array.Fill(owners, -1);
        var instructions = new Dictionary<ushort, Dos16FlowInstruction>();
        var edges = new List<Dos16FlowEdge>();
        var diagnostics = new List<Dos16FlowDiagnostic>();
        var queue = new Queue<ushort>();
        var queued = new HashSet<ushort>();
        queue.Enqueue(entry);
        queued.Add(entry);
        bool truncated = false;

        void Note(ushort at, string code, string message)
        {
            if (diagnostics.Count < 2048)
                diagnostics.Add(new Dos16FlowDiagnostic(new Dos16Address(region.Segment, at),
                    code, message));
        }

        void Edge(ushort at, int? destination, string kind, string evidence)
        {
            var source = new Dos16Address(region.Segment, at);
            if (destination is null)
            {
                edges.Add(new Dos16FlowEdge(source, null, kind, evidence));
                if (kind.StartsWith("unresolved-", StringComparison.Ordinal))
                    Note(at, kind, evidence);
                return;
            }
            int dest = destination.Value;
            if (dest < region.Origin || dest >= region.EndExclusive)
            {
                edges.Add(new Dos16FlowEdge(source, null, "out-of-region-" + kind,
                    "Destination not in the explicitly backed 16-bit load window; " + evidence));
                Note(at, "out-of-region-target", "Target cannot be confirmed inside this mapped segment.");
                return;
            }
            ushort ip = checked((ushort)dest);
            edges.Add(new Dos16FlowEdge(source,
                new Dos16Address(region.Segment, ip), kind, evidence));
            if (owners[ip - region.Origin] != -1 && !instructions.ContainsKey(ip))
            {
                Note(ip, "mid-instruction-target",
                    "Candidate branch targets a previously claimed instruction interior.");
                return;
            }
            if (queued.Add(ip)) queue.Enqueue(ip);
        }

        while (queue.Count > 0)
        {
            if (instructions.Count >= maxInstructions)
            {
                truncated = true;
                Note(queue.Peek(), "analysis-budget-reached",
                    "Candidate graph stopped at requested instruction budget.");
                break;
            }
            ushort current = queue.Dequeue();
            if (instructions.ContainsKey(current)) continue;
            int relative = current - region.Origin;
            if (owners[relative] >= 0)
            {
                Note(current, "mid-instruction-seed",
                    "A reachable candidate begins within a previously decoded instruction.");
                continue;
            }
            var (physical, remaining) = region.Map(current);
            var candidate = X86RealModeDecoder.Decode(region.Source,
                physical, remaining, region.Segment, current, 1);
            if (candidate.Count == 0 || !candidate[0].Valid)
            {
                Note(current, "invalid-x86-16",
                    "Decoder did not produce a valid instruction at this backed address.");
                continue;
            }
            var decodedLine = candidate[0];
            int length = decodedLine.OriginalBytes.Length / 2;
            if (length is < 1 or > 15 || relative + length > owners.Length ||
                (long)current + length > 0x10000)
            {
                Note(current, "instruction-exceeds-region",
                    "Instruction crosses the source-backed or 16-bit IP boundary.");
                continue;
            }
            bool overlap = false;
            for (int i = relative; i < relative + length; i++)
                if (owners[i] >= 0) { overlap = true; break; }
            if (overlap)
            {
                Note(current, "overlapping-interpretation",
                    "Alternative candidate overlaps retained instruction bytes; no replacement made.");
                continue;
            }

            // Decode the same original bytes for precise Iced control-transfer
            // operands; the display representation stays the shared decoder's.
            var code = region.Source.Data.Slice(physical, length).ToArray();
            var icedReader = new ByteArrayCodeReader(code);
            var decoder = Decoder.Create(16, icedReader);
            decoder.IP = current;
            decoder.Decode(out var instruction);
            if (instruction.Code == Code.INVALID || instruction.Length != length)
                throw new InvalidDataException("Displayed x86-16 candidate and CFG decoder disagree.");

            for (int i = relative; i < relative + length; i++) owners[i] = relative;
            var source = new Dos16Address(region.Segment, current);
            instructions.Add(current, new Dos16FlowInstruction(source, length,
                physical, decodedLine.OriginalBytes, decodedLine.Assembly, decodedLine.FlowControl));
            int after = current + length;
            bool isNear = instruction.Op0Kind is OpKind.NearBranch16 or OpKind.NearBranch32;
            // Operand-size overrides may produce a 32-bit destination.
            // Do not overflow or truncate it into a fabricated 16-bit IP.
            ulong nearTarget = instruction.NearBranchTarget;
            int? direct = isNear && nearTarget <= ushort.MaxValue
                ? (int)nearTarget : null;
            const string relativeEvidence = "16-bit-relative-target;original-bytes;same-segment-hypothesis";

            switch (instruction.FlowControl)
            {
                case FlowControl.UnconditionalBranch:
                    if (direct.HasValue) Edge(current, direct, "branch", relativeEvidence);
                    else Edge(current, null, "unresolved-far-or-dynamic-jump",
                        "Far branch segment selector or runtime target is not statically established.");
                    break;
                case FlowControl.ConditionalBranch:
                    if (direct.HasValue) Edge(current, direct, "branch-taken", relativeEvidence);
                    else Edge(current, null, "unresolved-conditional-target",
                        "Conditional transfer operand is not a recognized near branch.");
                    Edge(current, after, "branch-not-taken", "candidate-sequential-fallthrough");
                    break;
                case FlowControl.Call:
                    if (direct.HasValue) Edge(current, direct, "call", relativeEvidence);
                    else Edge(current, null, "unresolved-far-call",
                        "Far call destination requires a runtime selector or relocation context.");
                    Edge(current, after, "call-return-candidate",
                        "Potential continuation; callee return is not proven.");
                    break;
                case FlowControl.IndirectBranch:
                    Edge(current, null, "unresolved-indirect-jump",
                        "Register/memory target requires runtime state or validated table evidence.");
                    break;
                case FlowControl.IndirectCall:
                    Edge(current, null, "unresolved-indirect-call",
                        "Register/memory call destination remains dynamic.");
                    Edge(current, after, "call-return-candidate",
                        "Potential continuation; callee return is not proven.");
                    break;
                case FlowControl.Interrupt:
                    Edge(current, null, "unresolved-interrupt",
                        "Service semantics and return behavior are not modeled.");
                    Edge(current, after, "interrupt-return-candidate",
                        "Speculative return to sequential code after software interrupt.");
                    break;
                case FlowControl.Return:
                case FlowControl.Exception:
                    break;
                case FlowControl.XbeginXabortXend:
                    Edge(current, null, "unresolved-transaction-control",
                        "Transactional control transfer is not modeled in DOS x86-16 candidate flow.");
                    Edge(current, after, "transaction-fallthrough-candidate",
                        "Sequential continuation is conditional on unsupported transactional semantics.");
                    break;
                default:
                    Edge(current, after, "fallthrough", "candidate-sequential-execution");
                    break;
            }
        }

        int known = owners.Count(x => x >= 0);
        return new Dos16FlowResult(region.SourceSha256, region.MappingEvidence,
            "candidate-unverified;one-explicit-segment;no-runtime-relocation",
            new Dos16Address(region.Segment, entry),
            region.FileOffset, region.ByteLength, known, region.ByteLength - known,
            truncated, instructions.Values.OrderBy(x => x.Address.Offset).ToArray(),
            edges.ToArray(), diagnostics.ToArray());
    }
}
