using System.Text.Json;

namespace DosDis.Core;

/// <summary>
/// Opt-in evidence for conservative CFG decisions, not a game execution trace.
/// No original bytes, arbitrary prose, exception details or analyst notes.
/// </summary>
public enum AnalysisDecisionRule
{
    EntrySeed, AnalystProtection, ByteOwnership, FileBacking,
    X86Decoder, Reachability, RelocationReference, FlowTarget, AnalysisBudget
}

public enum AnalysisDecisionOutcome
{
    SeedQueued, ProtectedRangeDeclared, ProtectedTargetRejected,
    MidInstructionRejected, UnbackedTargetRejected, InvalidInstructionRejected,
    OverlappingDecodeRejected, DecodedCandidate, PointerTableCandidate,
    DirectTargetQueued, IndirectTargetQueued, UnbackedFlowTargetRejected,
    RuntimeTargetUnresolved, BudgetExceeded
}

public sealed record AnalysisDecisionEvent(
    long Sequence, AnalysisDecisionRule Rule, AnalysisDecisionOutcome Outcome,
    FlowAddress Address, long? FileOffset, FlowAddress? Destination,
    long? DestinationFileOffset, bool IsSpeculative);

public sealed class AnalysisDecisionTrace
{
    public const int SchemaVersion = 1;
    public const int DefaultCapacity = 512;
    private readonly object _gate = new();
    private readonly Queue<AnalysisDecisionEvent> _events = new();
    private readonly int _capacity;
    private long _sequence;
    private long _dropped;
    private bool _started;

    public AnalysisDecisionTrace(int capacity = DefaultCapacity)
    {
        if (capacity is < 8 or > 4096)
            throw new ArgumentOutOfRangeException(nameof(capacity));
        _capacity = capacity;
    }

    public string? ArtifactSha256 { get; private set; }
    public long DroppedEvents { get { lock (_gate) return _dropped; } }

    internal void Begin(LeImage image)
    {
        ArgumentNullException.ThrowIfNull(image);
        lock (_gate)
        {
            if (_started) throw new InvalidOperationException("Decision traces are single-use.");
            ArtifactSha256 = image.ArtifactSha256;
            _started = true;
        }
    }

    internal void Record(LeImage image, AnalysisDecisionRule rule,
        AnalysisDecisionOutcome outcome, FlowAddress address,
        FlowAddress? destination = null, bool speculative = false)
    {
        if (!Enum.IsDefined(rule) || !Enum.IsDefined(outcome))
            throw new ArgumentOutOfRangeException(nameof(rule), "Unknown decision rule or outcome.");
        var entry = new AnalysisDecisionEvent(0, rule, outcome, address,
            Map(image, address), destination, destination is { } target ? Map(image, target) : null,
            speculative);
        lock (_gate)
        {
            if (!_started || image.ArtifactSha256 != ArtifactSha256)
                throw new InvalidOperationException("Decision trace source identity mismatch.");
            if (_events.Count == _capacity)
            {
                _events.Dequeue();
                _dropped++;
            }
            _events.Enqueue(entry with { Sequence = ++_sequence });
        }
    }

    private static long? Map(LeImage image, FlowAddress address) =>
        address.Object >= 1 && address.Object <= image.Objects.Count &&
        address.Offset < image.Objects[address.Object - 1].VirtualSize
            ? image.ToFileOffset(address.Object, address.Offset) : null;

    public IReadOnlyList<AnalysisDecisionEvent> Snapshot()
    {
        lock (_gate) return _events.ToArray();
    }

    /// <summary>In-memory JSON only: no automatic files or uploads.</summary>
    public string BuildJson()
    {
        AnalysisDecisionEvent[] events;
        long dropped;
        string? source;
        lock (_gate)
        {
            events = _events.ToArray();
            dropped = _dropped;
            source = ArtifactSha256;
        }
        return JsonSerializer.Serialize(new
        {
            format = "dosdis-analysis-decision-trace",
            schemaVersion = SchemaVersion,
            sourceSha256 = source,
            recordedEvents = events.Length,
            droppedEvents = dropped,
            privacy = new
            {
                originalBytesIncluded = false,
                sourcePathsIncluded = false,
                analystCommentsIncluded = false,
                freeFormMessagesIncluded = false,
                automaticNetworkUpload = false
            },
            events = events.Select(e => new
            {
                sequence = e.Sequence,
                ruleId = e.Rule.ToString(),
                outcome = e.Outcome.ToString(),
                address = new { @object = e.Address.Object, offset = e.Address.Offset,
                    fileOffset = e.FileOffset },
                destination = e.Destination is { } target
                    ? new { @object = target.Object, offset = target.Offset,
                        fileOffset = e.DestinationFileOffset } : null,
                isSpeculative = e.IsSpeculative
            })
        }, new JsonSerializerOptions { WriteIndented = true });
    }
}
