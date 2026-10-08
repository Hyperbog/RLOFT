using Iced.Intel;

namespace DosDis.Core;

/// <summary>LE object-relative location, never an unqualified process address.</summary>
public readonly record struct FlowAddress(int Object, uint Offset);
public sealed record ProtectedDataRange(int Object, uint Begin, uint EndExclusive,
    string Provenance = "analyst-specified");
public sealed record FlowInstruction(FlowAddress Address, int Length, string OriginalBytes,
    string Assembly, string FlowControl, string Evidence);
public sealed record FlowEdge(FlowAddress Source, FlowAddress? Destination, string Kind,
    string Evidence);
public sealed record FlowBlock(FlowAddress Start, uint EndExclusive,
    IReadOnlyList<FlowAddress> InstructionAddresses);
public sealed record FunctionCandidate(FlowAddress Entry,
    IReadOnlyList<FlowAddress> Blocks, string Evidence);
public sealed record ClassifiedRange(int Object, uint Begin, uint EndExclusive,
    string Classification, string Evidence);
public sealed record FlowDiagnostic(FlowAddress Address, string Code, string Description);
public sealed record FlowAnalysisResult(
    string ArtifactSha256,
    IReadOnlyList<FlowAddress> Seeds,
    IReadOnlyList<ProtectedDataRange> ProtectedData,
    IReadOnlyList<FlowInstruction> Instructions,
    IReadOnlyList<FlowEdge> Edges,
    IReadOnlyList<FlowBlock> Blocks,
    IReadOnlyList<FunctionCandidate> FunctionCandidates,
    IReadOnlyList<ClassifiedRange> Ranges,
    IReadOnlyList<FlowDiagnostic> Diagnostics,
    IReadOnlyList<DataCrossReference> DataReferences,
    IReadOnlyList<JumpTableCandidate> JumpTables,
    bool Truncated);

/// <summary>
/// Conservative recursive x86-32 traversal. Discovery is CANDIDATE, not a
/// claim that every decoded address is semantically proved executable.
/// No speculative linear gap sweep, no implicit relocation-pointer code seeds.
/// </summary>
public static class ControlFlowAnalysis
{
    private const int MaxRegionBytes = 16 * 1024 * 1024;
    private const int MaxSeeds = 4096;
    private const int MaxDiagnostics = 2000;

    public static FlowAnalysisResult Analyze(LeImage image,
        IEnumerable<FlowAddress>? seeds = null,
        IEnumerable<ProtectedDataRange>? protectedData = null,
        int maxInstructions = 100_000,
        bool relocated = false, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        ArgumentNullException.ThrowIfNull(image);
        if (maxInstructions is < 1 or > 1_000_000)
            throw new ArgumentOutOfRangeException(nameof(maxInstructions));
        var sourceSeeds = (seeds ?? new[] { new FlowAddress((int)image.EntryObject, image.EntryOffset) })
            .Distinct().ToArray();
        if (sourceSeeds.Length is < 1 or > MaxSeeds)
            throw new ArgumentOutOfRangeException(nameof(seeds), "Provide 1 to 4096 entry seeds.");
        var explicitData = (protectedData ?? Array.Empty<ProtectedDataRange>()).ToArray();
        if (explicitData.Length > 100_000)
            throw new ArgumentOutOfRangeException(nameof(protectedData));

        // A separate ownership array for each executable object. -1 = no
        // instruction; a nonnegative value identifies an instruction start.
        var owners = new Dictionary<int, int[]>();
        var status = new Dictionary<int, byte[]>(); // 0 unknown; 1 candidate; 2 protected
        long total = 0;
        foreach (var obj in image.Objects.Where(o => (o.Flags & 4u) != 0))
        {
            cancellationToken.ThrowIfCancellationRequested();
            if (obj.VirtualSize > MaxRegionBytes || (total += obj.VirtualSize) > MaxRegionBytes)
                throw new NotSupportedException("Prototype CFG analysis is bounded to 16 MiB of executable objects.");
            var o = new int[checked((int)obj.VirtualSize)];
            Array.Fill(o, -1);
            owners[obj.Number] = o;
            status[obj.Number] = new byte[o.Length];
        }
        var diagnostics = new List<FlowDiagnostic>();
        void Report(FlowAddress at, string code, string message)
        {
            if (diagnostics.Count < MaxDiagnostics)
                diagnostics.Add(new FlowDiagnostic(at, code, message));
        }
        foreach (var range in explicitData)
        {
            cancellationToken.ThrowIfCancellationRequested();
            if (!status.TryGetValue(range.Object, out byte[]? classifications) ||
                range.Begin >= range.EndExclusive || range.EndExclusive > classifications.Length)
                throw new ArgumentException("Protected range must lie inside one executable object.", nameof(protectedData));
            for (uint i = range.Begin; i < range.EndExclusive; i++)
            {
                if ((i & 4095u) == 0) cancellationToken.ThrowIfCancellationRequested();
                classifications[checked((int)i)] = 2;
            }
        }

        var instructions = new SortedDictionary<FlowAddress, FlowInstruction>(
            Comparer<FlowAddress>.Create((a, b) => a.Object != b.Object
                ? a.Object.CompareTo(b.Object) : a.Offset.CompareTo(b.Offset)));
        var edges = new List<FlowEdge>();
        var dataReferences = new List<DataCrossReference>();
        var jumpTables = new List<JumpTableCandidate>();
        var referenceRecovery = new ReferenceRecovery(image);
        var indirectFunctionEntries = new HashSet<FlowAddress>();
        var queued = new HashSet<FlowAddress>();
        var pending = new Queue<FlowAddress>();
        // Preserve whether a discovered instruction was reached only through a
        // candidate indirect edge rather than a direct or analyst seed.
        var speculativeOnly = new Dictionary<FlowAddress, bool>();
        var functionEntries = new HashSet<FlowAddress>(sourceSeeds);
        bool truncated = false;

        bool IsExecutable(FlowAddress loc) =>
            owners.TryGetValue(loc.Object, out int[]? arr) && loc.Offset < arr.Length;

        void QueueCandidate(FlowAddress loc, bool speculative = false)
        {
            if (!IsExecutable(loc)) return;
            if (!speculativeOnly.TryGetValue(loc, out bool previouslySpeculative))
                speculativeOnly[loc] = speculative;
            else if (previouslySpeculative && !speculative)
                speculativeOnly[loc] = false;
            if (queued.Add(loc)) pending.Enqueue(loc);
        }
        foreach (var seed in sourceSeeds)
        {
            if (!IsExecutable(seed))
                throw new ArgumentException($"Seed {seed} is outside an executable LE object.", nameof(seeds));
            QueueCandidate(seed);
        }

        while (pending.Count > 0)
        {
            cancellationToken.ThrowIfCancellationRequested();
            FlowAddress cursor = pending.Dequeue();
            if (instructions.ContainsKey(cursor)) continue;
            if (!IsExecutable(cursor)) continue;
            int[] ownership = owners[cursor.Object];
            byte[] classifications = status[cursor.Object];
            int pos = checked((int)cursor.Offset);
            if (classifications[pos] == 2)
            {
                Report(cursor, "protected-data-target", "Traversal would enter analyst-protected data; not decoded.");
                continue;
            }
            if (ownership[pos] >= 0)
            {
                Report(cursor, "mid-instruction-target", $"Control flow enters instruction at 0x{ownership[pos]:X}; alternate decode not assumed.");
                continue;
            }
            if (instructions.Count >= maxInstructions)
            {
                truncated = true;
                break;
            }

            // x86 maximum instruction length is 15 bytes. Stop at the first
            // non-file-backed byte; do not disassemble implicit virtual zeroes.
            int length = 0;
            while (length < 15 && (ulong)cursor.Offset + (uint)length < (uint)ownership.Length &&
                   image.ToFileOffset(cursor.Object, cursor.Offset + (uint)length) is not null)
                length++;
            if (length == 0)
            {
                Report(cursor, "unbacked-target", "No original-file bytes for this address.");
                continue;
            }
            byte[] raw = image.ReadStoredBytes(cursor.Object, cursor.Offset, length);
            byte[] bytes = relocated ? image.ReadRelocatedBytes(cursor.Object, cursor.Offset, length) : raw;
            var decoder = Decoder.Create(32, new ByteArrayCodeReader(bytes));
            decoder.IP = cursor.Offset;
            decoder.Decode(out var instruction);
            int size = instruction.Length;
            if (instruction.Code == Code.INVALID || size is < 1 or > 15 || size > length)
            {
                Report(cursor, "invalid-instruction", "Instruction could not be decoded without speculation.");
                continue;
            }
            bool conflict = false;
            for (int i = pos; i < pos + size; i++)
            {
                if (classifications[i] == 2 || ownership[i] >= 0)
                {
                    conflict = true;
                    break;
                }
            }
            if (conflict)
            {
                Report(cursor, "overlapping-interpretation", "Instruction overlaps another instruction or protected data.");
                continue;
            }
            var formatter = new NasmFormatter();
            formatter.Options.SpaceAfterOperandSeparator = true;
            var output = new StringOutput();
            formatter.Format(instruction, output);
            instructions.Add(cursor, new FlowInstruction(cursor, size,
                Convert.ToHexString(raw.AsSpan(0, size)), output.ToStringAndReset(),
                instruction.FlowControl.ToString(),
                speculativeOnly.GetValueOrDefault(cursor) ? "candidate-relocation-backed-indirect" :
                "candidate-reachable-from-seed"));
            for (int i = pos; i < pos + size; i++)
            {
                ownership[i] = pos;
                classifications[i] = 1;
            }

            var references = referenceRecovery.Inspect(cursor, size, instruction);
            dataReferences.AddRange(references.Data);
            if (references.Table is not null) jumpTables.Add(references.Table);

            FlowAddress next = new(cursor.Object, checked(cursor.Offset + (uint)size));
            bool hasNext = IsExecutable(next);
            bool nextBacked = hasNext && image.ToFileOffset(next.Object, next.Offset) != null;
            bool directNear = instruction.Op0Kind is OpKind.NearBranch16 or OpKind.NearBranch32 or OpKind.NearBranch64;
            FlowAddress? directTarget = null;
            if (directNear)
            {
                ulong target = instruction.NearBranchTarget;
                if (target <= uint.MaxValue)
                    directTarget = new FlowAddress(cursor.Object, (uint)target);
            }
            void AddEdge(FlowAddress? target, string kind, string evidence = "decoded-flow")
            {
                edges.Add(new FlowEdge(cursor, target, kind, evidence));
                if (target is not null)
                {
                    if (IsExecutable(target.Value) && image.ToFileOffset(target.Value.Object, target.Value.Offset) != null)
                        QueueCandidate(target.Value, kind.StartsWith("candidate-indirect-", StringComparison.Ordinal));
                    else Report(cursor, "unresolved-target", $"{kind} destination {target.Value} is not backed executable code in this object.");
                }
            }
            void Continuation(string kind = "fallthrough")
            {
                if (nextBacked) AddEdge(next, kind);
                else if (hasNext) Report(cursor, "fallthrough-unbacked", "Next byte is not backed by source file.");
            }
            switch (instruction.FlowControl)
            {
                case FlowControl.Call:
                    // Schedule continuation first so short callers are explored
                    // before large callees, avoiding premature budget exhaustion.
                    Continuation("call-continuation");
                    AddEdge(directTarget, directTarget is null ? "unresolved-far-call" : "call");
                    if (directTarget is { } callee && IsExecutable(callee)) functionEntries.Add(callee);
                    break;
                case FlowControl.IndirectCall:
                    Continuation("indirect-call-continuation");
                    foreach (var target in references.IndirectTargets)
                    {
                        AddEdge(target.Target, "candidate-indirect-call", target.Evidence);
                        indirectFunctionEntries.Add(target.Target);
                        functionEntries.Add(target.Target);
                    }
                    // Register state and runtime memory remain unknown, so
                    // candidate targets never eliminate the unresolved edge.
                    AddEdge(null, "unresolved-indirect-call");
                    break;
                case FlowControl.ConditionalBranch:
                    Continuation("branch-not-taken");
                    AddEdge(directTarget, directTarget is null ? "unresolved-conditional" : "branch-taken");
                    break;
                case FlowControl.UnconditionalBranch:
                    AddEdge(directTarget, directTarget is null ? "unresolved-far-jump" : "jump");
                    break;
                case FlowControl.IndirectBranch:
                    foreach (var target in references.IndirectTargets)
                        AddEdge(target.Target, "candidate-indirect-jump", target.Evidence);
                    AddEdge(null, "unresolved-indirect-jump");
                    break;
                case FlowControl.Return:
                    break;
                case FlowControl.Interrupt:
                    AddEdge(null, "interrupt-transfer", "runtime-outcome-unproven");
                    Continuation("assumed-interrupt-return");
                    break;
                case FlowControl.Exception:
                    AddEdge(null, "exception-transfer");
                    break;
                case FlowControl.XbeginXabortXend:
                    if (instruction.Mnemonic == Mnemonic.Xbegin)
                    {
                        Continuation("xbegin-not-taken");
                        AddEdge(directTarget, directTarget is null ? "unresolved-xbegin" : "xbegin-abort-target");
                    }
                    else Continuation("transaction-instruction-continuation");
                    break;
                default:
                    Continuation();
                    break;
            }
        }
        // Edges and instructions are separate: a branch can target a byte
        // already owned by a different instruction. Preserve that conflict.
        var decodedStarts = new HashSet<FlowAddress>(instructions.Keys);
        foreach (var edge in edges)
            if (edge.Destination is { } dest && IsExecutable(dest) &&
                !decodedStarts.Contains(dest) && ownershipAt(dest) >= 0)
                Report(edge.Source, "target-not-instruction-start",
                    $"{edge.Kind} points inside a decoded instruction at {dest}.");

        int ownershipAt(FlowAddress at) => owners[at.Object][checked((int)at.Offset)];
        var leaders = new HashSet<FlowAddress>(sourceSeeds.Where(decodedStarts.Contains));
        foreach (var entry in functionEntries.Where(decodedStarts.Contains)) leaders.Add(entry);
        foreach (var edge in edges)
        {
            if (edge.Kind != "fallthrough" && edge.Destination is { } dest && decodedStarts.Contains(dest))
                leaders.Add(dest);
        }
        // A block is a maximal contiguous instruction sequence between
        // leaders and non-fallthrough control-transfer instructions.
        var outgoing = edges.GroupBy(e => e.Source).ToDictionary(g => g.Key, g => g.ToArray());
        var blocks = new List<FlowBlock>();
        var blockOf = new Dictionary<FlowAddress, FlowAddress>();
        foreach (FlowAddress leader in leaders.OrderBy(x => x.Object).ThenBy(x => x.Offset))
        {
            cancellationToken.ThrowIfCancellationRequested();
            if (blockOf.ContainsKey(leader)) continue;
            var members = new List<FlowAddress>();
            FlowAddress at = leader;
            while (instructions.TryGetValue(at, out var line) && !blockOf.ContainsKey(at))
            {
                members.Add(at);
                blockOf[at] = leader;
                FlowAddress following = new(at.Object, checked(at.Offset + (uint)line.Length));
                if (leaders.Contains(following) || !outgoing.TryGetValue(at, out var ee) ||
                    ee.Length != 1 || ee[0].Kind != "fallthrough" || ee[0].Destination != following)
                    break;
                at = following;
            }
            if (members.Count > 0)
            {
                var last = instructions[members[^1]];
                blocks.Add(new FlowBlock(leader,
                    checked(last.Address.Offset + (uint)last.Length), members));
            }
        }
        // If a mid-instruction conflict appeared before some addresses became
        // leaders, no instruction should disappear from the block index.
        foreach (var address in decodedStarts.OrderBy(x => x.Object).ThenBy(x => x.Offset))
            if (!blockOf.ContainsKey(address))
            {
                var line = instructions[address];
                blocks.Add(new FlowBlock(address, checked(address.Offset + (uint)line.Length), new[] { address }));
                blockOf[address] = address;
            }
        blocks.Sort((a, b) => a.Start.Object != b.Start.Object
            ? a.Start.Object.CompareTo(b.Start.Object) : a.Start.Offset.CompareTo(b.Start.Offset));

        // Candidate function membership permits one block to belong to
        // multiple entries (shared RET, tail merging). Calls are not traversed
        // through into their callees; jump edges may cross possible functions.
        var blockEdges = new Dictionary<FlowAddress, HashSet<FlowAddress>>();
        foreach (var edge in edges)
        {
            if (edge.Kind is "call" or "unresolved-far-call" or "unresolved-indirect-call" or "interrupt-transfer" or "exception-transfer") continue;
            if (edge.Destination is not { } dest || !blockOf.TryGetValue(edge.Source, out var srcBlock) ||
                !blockOf.TryGetValue(dest, out var dstBlock)) continue;
            if (!blockEdges.TryGetValue(srcBlock, out var links)) blockEdges[srcBlock] = links = new HashSet<FlowAddress>();
            links.Add(dstBlock);
        }
        var functions = new List<FunctionCandidate>();
        foreach (FlowAddress entry in functionEntries.OrderBy(x => x.Object).ThenBy(x => x.Offset))
        {
            cancellationToken.ThrowIfCancellationRequested();
            if (!blockOf.TryGetValue(entry, out var start)) continue;
            var reachable = new HashSet<FlowAddress>();
            var frontier = new Queue<FlowAddress>();
            frontier.Enqueue(start);
            while (frontier.Count > 0)
            {
                cancellationToken.ThrowIfCancellationRequested();
                var b = frontier.Dequeue();
                if (!reachable.Add(b)) continue;
                if (blockEdges.TryGetValue(b, out var links))
                    foreach (var target in links) frontier.Enqueue(target);
            }
            functions.Add(new FunctionCandidate(entry,
                reachable.OrderBy(x => x.Object).ThenBy(x => x.Offset).ToArray(),
                sourceSeeds.Contains(entry) ? "analyst-or-module-entry-seed" :
                    indirectFunctionEntries.Contains(entry) ? "relocation-backed-indirect-call-candidate" :
                    "direct-call-candidate"));
        }

        // Emit maximal contiguous classification spans for executable objects.
        // Existing unknown bytes remain unknown; no linear sweep of gaps.
        var regions = new List<ClassifiedRange>();
        foreach (var (number, marks) in status.OrderBy(p => p.Key))
        {
            cancellationToken.ThrowIfCancellationRequested();
            int i = 0;
            while (i < marks.Length)
            {
                cancellationToken.ThrowIfCancellationRequested();
                byte state = marks[i];
                bool backed = image.ToFileOffset(number, (uint)i) != null;
                int j = i + 1;
                while (j < marks.Length && marks[j] == state &&
                       (image.ToFileOffset(number, (uint)j) != null) == backed)
                {
                    if ((j & 4095) == 0) cancellationToken.ThrowIfCancellationRequested();
                    j++;
                }
                string cls = state switch { 1 => "candidate-code", 2 => "protected-data", _ when backed => "unknown-file-backed", _ => "unbacked-virtual" };
                string evidence = state switch { 1 => "reachable-from-seed", 2 => "analyst-specified", _ => "unclassified" };
                regions.Add(new ClassifiedRange(number, (uint)i, (uint)j, cls, evidence));
                i = j;
            }
        }
        return new FlowAnalysisResult(image.ArtifactSha256, sourceSeeds, explicitData,
            instructions.Values.ToArray(), edges, blocks, functions, regions, diagnostics,
            dataReferences, jumpTables, truncated);
    }
}
