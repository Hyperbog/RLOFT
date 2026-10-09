using System.Buffers.Binary;
using Iced.Intel;

namespace DosDis.Core;

/// <summary>Evidence-linked references. Every address is LE object-relative.
/// Candidate indirect targets are never promoted to verified functions or data.</summary>
public sealed record DataCrossReference(FlowAddress Source, FlowAddress Target,
    string Kind, string Evidence);
/// <summary>A shared indirect dispatch-table candidate, for CALL or JMP.</summary>
/// <remarks>The default keeps legacy stored candidate snapshots unclassified.</remarks>
public sealed record JumpTableCandidate(FlowAddress Source, FlowAddress Table,
    IReadOnlyList<FlowAddress> CandidateTargets, int EntryWidth, string Evidence,
    string DispatchKind = "unclassified-indirect-table");
internal sealed record IndirectCandidate(FlowAddress Target, string Kind, string Evidence);
internal sealed record RecoveredReferenceSet(
    IReadOnlyList<DataCrossReference> Data,
    IReadOnlyList<IndirectCandidate> IndirectTargets,
    JumpTableCandidate? Table);

// Only passed when CFG proves CMP index,constant -> JA default -> indexed JMP.
internal readonly record struct BoundedDispatchHint(Register Index, int EntryCount);

/// <summary>
/// Narrow x86-32 LE recognition: absolute DS memory operands; internal
/// relocation-backed immediate references; indirect calls/jumps through an
/// absolute pointer; and relocation-backed, indexed dword tables.
/// No symbolic index bounds, register value tracking or runtime memory model.
/// </summary>
internal sealed class ReferenceRecovery
{
    private const int MaxTableEntries = 128;
    private readonly LeImage image;
    private readonly Dictionary<FlowAddress, LeRelocation> fixups = new();
    private readonly HashSet<FlowAddress> ambiguousFixups = new();

    public ReferenceRecovery(LeImage image)
    {
        this.image = image;
        foreach (var f in image.Relocations)
        {
            if (f.SourceOffset < 0 || f.SourceOffset > uint.MaxValue) continue;
            var key = new FlowAddress(f.SourceObject, (uint)f.SourceOffset);
            if (ambiguousFixups.Contains(key)) continue;
            if (fixups.TryGetValue(key, out var previous) &&
                (previous.SourceType != f.SourceType || previous.TargetObject != f.TargetObject ||
                 previous.TargetOffset != f.TargetOffset || previous.Additive != f.Additive))
            {
                fixups.Remove(key);
                ambiguousFixups.Add(key);
            }
            else fixups[key] = f;
        }
    }

    private FlowAddress? MapLinear(ulong address)
    {
        FlowAddress? result = null;
        foreach (var obj in image.Objects)
        {
            ulong first = obj.PreferredBase;
            if (address < first || address - first >= obj.VirtualSize) continue;
            if (result is not null) return null; // overlapping preferred bases
            result = new FlowAddress(obj.Number, (uint)(address - first));
        }
        return result;
    }

    private bool IsBackedRange(FlowAddress address, int count)
    {
        if (count < 1 || (ulong)address.Offset + (uint)count > uint.MaxValue) return false;
        for (int i = 0; i < count; i++)
            if (image.ToFileOffset(address.Object, address.Offset + (uint)i) is null) return false;
        return true;
    }

    private FlowAddress? ResolvePointer(FlowAddress source)
    {
        if (!IsBackedRange(source, 4) || !fixups.TryGetValue(source, out var fixup) ||
            fixup.SourceType != 0x07) return null;
        if (fixup.TargetObject < 1 || fixup.TargetObject > image.Objects.Count) return null;
        var targetObject = image.Objects[fixup.TargetObject - 1];
        ulong targetOffset = (ulong)fixup.TargetOffset + fixup.Additive;
        if ((targetObject.Flags & 4u) == 0 || targetOffset >= targetObject.VirtualSize) return null;
        var target = new FlowAddress(fixup.TargetObject, (uint)targetOffset);
        if (!IsBackedRange(target, 1)) return null;
        // The parsed record, not a coincidental literal dword, establishes the pointer.
        // Relocated memory is a separate view; its bytes are not written to the file.
        uint expectedLinear = unchecked(targetObject.PreferredBase + (uint)targetOffset);
        byte[] relocated = image.ReadRelocatedBytes(source.Object, source.Offset, 4);
        if (BitConverter.ToUInt32(relocated, 0) != expectedLinear) return null;
        return target;
    }

    public RecoveredReferenceSet Inspect(FlowAddress address, int instructionLength,
        Instruction insn, BoundedDispatchHint? boundedDispatch = null)
    {
        var data = new List<DataCrossReference>();
        var indirect = new List<IndirectCandidate>();
        JumpTableCandidate? table = null;

        // A relocation explicitly attached to the encoded instruction may
        // describe an address operand even when its machine representation is
        // an immediate. Do not infer pointers from arbitrary literal constants.
        for (int j = 0; j + 4 <= instructionLength; j++)
        {
            var location = new FlowAddress(address.Object, checked(address.Offset + (uint)j));
            if (fixups.TryGetValue(location, out var relocation) && relocation.SourceType == 0x07)
            {
                var obj = image.Objects[relocation.TargetObject - 1];
                ulong offset = (ulong)relocation.TargetOffset + relocation.Additive;
                if (offset < obj.VirtualSize)
                    data.Add(new DataCrossReference(address,
                        new FlowAddress(relocation.TargetObject, (uint)offset),
                        "relocated-operand", "LE-internal-absolute-fixup"));
            }
        }

        // A CS-indexed transfer addresses the current executable LE object.
        // Other segment selectors and register-base address calculations remain
        // unresolved: the runtime selector base is not reconstructed.
        bool csIndexed = insn.SegmentPrefix == Register.CS &&
            (insn.FlowControl is FlowControl.IndirectBranch or FlowControl.IndirectCall) &&
            insn.MemoryIndex != Register.None && insn.MemoryIndexScale == 4;
        if ((insn.SegmentPrefix != Register.None && !csIndexed) ||
            insn.MemoryBase != Register.None)
            return new RecoveredReferenceSet(data, indirect, null);
        bool hasMemory = false;
        for (int i = 0; i < insn.OpCount; i++)
            if (insn.GetOpKind(i) == OpKind.Memory) { hasMemory = true; break; }
        if (!hasMemory || insn.MemoryDisplacement64 > uint.MaxValue)
            return new RecoveredReferenceSet(data, indirect, null);
        FlowAddress? absolute = csIndexed
            ? new FlowAddress(address.Object, checked((uint)insn.MemoryDisplacement64))
            : MapLinear(insn.MemoryDisplacement64);
        // LE binaries often store an unrelocated disp32 in the original code.
        // For indirect transfers an internal absolute fixup at the *last four
        // encoded bytes* provides a stronger object-relative address than the
        // raw displacement. This does not require modifying the original view.
        if (insn.FlowControl is FlowControl.IndirectBranch or FlowControl.IndirectCall &&
            instructionLength >= 4)
        {
            var displacementSlot = new FlowAddress(address.Object,
                checked(address.Offset + (uint)(instructionLength - 4)));
            if (fixups.TryGetValue(displacementSlot, out var relocation) &&
                relocation.SourceType == 0x07 &&
                (!csIndexed || relocation.TargetObject == address.Object))
            {
                var obj = image.Objects[relocation.TargetObject - 1];
                ulong offset = (ulong)relocation.TargetOffset + relocation.Additive;
                absolute = offset < obj.VirtualSize
                    ? new FlowAddress(relocation.TargetObject, (uint)offset) : null;
            }
        }
        if (absolute is null || !IsBackedRange(absolute.Value, 1))
            return new RecoveredReferenceSet(data, indirect, null);

        if (insn.MemoryIndex == Register.None)
        {
            data.Add(new DataCrossReference(address, absolute.Value, "absolute-memory-operand",
                "decoded-absolute-effective-address;segment-base-assumed"));
            if (insn.FlowControl is FlowControl.IndirectCall or FlowControl.IndirectBranch)
            {
                var target = ResolvePointer(absolute.Value);
                if (target is not null)
                    indirect.Add(new IndirectCandidate(target.Value, "absolute-pointer",
                        "LE-internal-absolute-fixup;candidate-indirect-control-flow"));
            }
        }
        else if (insn.MemoryIndexScale == 4 &&
                 insn.FlowControl is FlowControl.IndirectBranch or FlowControl.IndirectCall)
        {
            // First prefer a contiguous, relocation-backed table. A separate,
            // more speculative fallback below requires an exact unsigned
            // switch guard and validates EVERY literal destination.
            var candidates = new List<FlowAddress>();
            for (int entry = 0; entry < MaxTableEntries; entry++)
            {
                ulong slot = (ulong)absolute.Value.Offset + (uint)entry * 4;
                if (slot > uint.MaxValue) break;
                var location = new FlowAddress(absolute.Value.Object, (uint)slot);
                if (slot + 4 > image.Objects[location.Object - 1].VirtualSize) break;
                var target = ResolvePointer(location);
                if (target is null) break;
                candidates.Add(target.Value);
            }
            if (candidates.Count >= 2)
            {
                string evidence = csIndexed
                    ? "candidate-relocation-backed-table;cs-object-relative;index-bound-unproven"
                    : "candidate-relocation-backed-table;index-bound-unproven";
                data.Add(new DataCrossReference(address, absolute.Value,
                    "indexed-pointer-table-base", evidence));
                table = new JumpTableCandidate(address, absolute.Value, candidates.ToArray(), 4,
                    evidence, insn.FlowControl == FlowControl.IndirectCall
                        ? "indirect-call-table" : "indirect-jump-table");
                foreach (var target in candidates.Distinct())
                    indirect.Add(new IndirectCandidate(target, "indexed-pointer-table", evidence);
            }
            else if (insn.FlowControl == FlowControl.IndirectBranch &&
                     boundedDispatch is { } bound && bound.Index == insn.MemoryIndex &&
                     bound.EntryCount is >= 2 and <= MaxTableEntries)
            {
                // A verified CMP reg,N; JA default guard bounds unsigned indices
                // to exactly N+1 values if no mutation intervenes. This is
                // strictly a candidate: literal dwords are not LE fixup proof.
                var literalTargets = new List<FlowAddress>(bound.EntryCount);
                bool valid = true;
                for (int entry = 0; entry < bound.EntryCount; entry++)
                {
                    ulong slot = (ulong)absolute.Value.Offset + (uint)entry * 4;
                    if (slot > uint.MaxValue) { valid = false; break; }
                    var location = new FlowAddress(absolute.Value.Object, (uint)slot);
                    if (fixups.ContainsKey(location) || ambiguousFixups.Contains(location) ||
                        !IsBackedRange(location, 4))
                    {
                        valid = false;
                        break;
                    }
                    uint rawPointer = BinaryPrimitives.ReadUInt32LittleEndian(
                        image.ReadStoredBytes(location.Object, location.Offset, 4));
                    FlowAddress? target = csIndexed
                        ? new FlowAddress(address.Object, rawPointer)
                        : MapLinear(rawPointer);
                    if (target is not { } resolved ||
                        (image.Objects[resolved.Object - 1].Flags & 4u) == 0 ||
                        !IsBackedRange(resolved, 1))
                    {
                        valid = false;
                        break;
                    }
                    literalTargets.Add(resolved);
                }
                if (valid && literalTargets.Distinct().Count() >= 2)
                {
                    string evidence = csIndexed
                        ? "candidate-guarded-literal-table;cs-object-relative;unsigned-ja-bound;no-relocation-proof"
                        : "candidate-guarded-literal-table;unsigned-ja-bound;no-relocation-proof";
                    data.Add(new DataCrossReference(address, absolute.Value,
                        "indexed-pointer-table-base", evidence));
                    table = new JumpTableCandidate(address, absolute.Value,
                        literalTargets.ToArray(), 4, evidence, "indirect-jump-table");
                    foreach (var target in literalTargets.Distinct())
                        indirect.Add(new IndirectCandidate(target, "guarded-literal-table", evidence));
                }
            }
        }
        return new RecoveredReferenceSet(data, indirect, table);
    }
}
