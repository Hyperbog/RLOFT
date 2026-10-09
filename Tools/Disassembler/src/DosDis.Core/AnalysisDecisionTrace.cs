using System.Text;
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
    DirectTargetQueued, IndirectTargetQueued,
    DirectTargetAlreadyKnown, IndirectTargetAlreadyKnown,
    UnbackedFlowTargetRejected, NonExecutableTargetRejected,
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
    private const int MaximumExportBytes = 8 * 1024 * 1024;
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

    /// <summary>
    /// Explicit, bounded, local-only export. Stage in the target directory and
    /// never replace an existing decision trace or leave a partial final file.
    /// </summary>
    public void WriteNew(string outputPath)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(outputPath);
        lock (_gate)
        {
            if (!_started)
                throw new InvalidOperationException("Run an analysis before exporting its trace.");
        }
        string destination = Path.GetFullPath(outputPath);
        if (!string.Equals(Path.GetExtension(destination), ".json", StringComparison.OrdinalIgnoreCase))
            throw new ArgumentException("Decision traces require a .json filename.", nameof(outputPath));
        string folder = Path.GetDirectoryName(destination)!;
        if (!Directory.Exists(folder))
            throw new DirectoryNotFoundException("Decision trace export directory does not exist.");
        if (File.Exists(destination) || Directory.Exists(destination))
            throw new IOException("Decision trace destination already exists; nothing was overwritten.");
        string json = BuildJson();
        if (Encoding.UTF8.GetByteCount(json) > MaximumExportBytes)
            throw new InvalidDataException("Decision trace exceeds the 8 MiB export bound.");
        string staging = Path.Combine(folder, "." + Path.GetFileName(destination) + "." +
            Guid.NewGuid().ToString("N") + ".part");
        try
        {
            using (var stream = new FileStream(staging, FileMode.CreateNew,
                       FileAccess.Write, FileShare.None))
            using (var writer = new StreamWriter(stream, new UTF8Encoding(false)))
            {
                writer.Write(json);
                writer.Flush();
                stream.Flush(flushToDisk: true);
            }
            File.Move(staging, destination); // no overwrite, even if another process raced
        }
        finally
        {
            if (File.Exists(staging)) File.Delete(staging);
        }
    }
}
