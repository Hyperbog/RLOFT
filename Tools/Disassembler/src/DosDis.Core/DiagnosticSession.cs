using System.Globalization;
using System.Text;
using System.Text.Json;

namespace DosDis.Core;

/// <summary>
/// Small, bounded diagnostic event history. No paths, source bytes, analyst
/// comments, arbitrary strings or exception messages enter the event schema.
/// This is an application-operation trace, NOT an x86 instruction-level debugger.
/// </summary>
public sealed class DiagnosticSession
{
    public const int SchemaVersion = 1;
    public const int DefaultCapacity = 512;
    private readonly object _gate = new();
    private readonly Queue<DiagnosticEvent> _events = new();
    private readonly int _capacity;
    private long _sequence;
    private long _dropped;

    public DiagnosticSession(int capacity = DefaultCapacity)
    {
        if (capacity is < 8 or > 4096)
            throw new ArgumentOutOfRangeException(nameof(capacity), "Diagnostic event capacity must be 8–4096.");
        _capacity = capacity;
        SessionId = Guid.NewGuid().ToString("D", CultureInfo.InvariantCulture);
        StartedUtc = DateTimeOffset.UtcNow.ToString("O", CultureInfo.InvariantCulture);
    }

    public string SessionId { get; }
    public string StartedUtc { get; }

    public void Record(DiagnosticStage stage, string? artifactSha256 = null,
        int? objectNumber = null, uint? objectOffset = null,
        int? instructionCount = null, Exception? error = null)
    {
        if (!Enum.IsDefined(stage))
            throw new ArgumentOutOfRangeException(nameof(stage));
        if (artifactSha256 is not null &&
            (artifactSha256.Length != 64 || !artifactSha256.All(
                 c => c is >= '0' and <= '9' or >= 'a' and <= 'f' or >= 'A' and <= 'F')))
            throw new ArgumentException("Artifact identity must be a SHA-256 digest.", nameof(artifactSha256));
        if (objectNumber is < 1)
            throw new ArgumentOutOfRangeException(nameof(objectNumber));
        if (instructionCount is < 0)
            throw new ArgumentOutOfRangeException(nameof(instructionCount));

        // No exception.Message / exception.StackTrace; both may embed sensitive
        // filenames, machine paths, and even user-controlled source data.
        string? errorType = error?.GetType().FullName;
        lock (_gate)
        {
            if (_events.Count == _capacity)
            {
                _events.Dequeue();
                _dropped++;
            }
            _events.Enqueue(new DiagnosticEvent(++_sequence,
                DateTimeOffset.UtcNow.ToString("O", CultureInfo.InvariantCulture),
                stage, artifactSha256?.ToLowerInvariant(), objectNumber,
                objectOffset, instructionCount, errorType));
        }
    }

    public IReadOnlyList<DiagnosticEvent> Snapshot()
    {
        lock (_gate) return _events.ToArray();
    }

    public string BuildJson()
    {
        DiagnosticEvent[] events;
        long dropped;
        lock (_gate) { events = _events.ToArray(); dropped = _dropped; }
        return JsonSerializer.Serialize(new
        {
            format = "dosdis-diagnostic-session",
            schemaVersion = SchemaVersion,
            sessionId = SessionId,
            startedUtc = StartedUtc,
            exportedUtc = DateTimeOffset.UtcNow.ToString("O", CultureInfo.InvariantCulture),
            toolVersion = typeof(DiagnosticSession).Assembly.GetName().Version?.ToString() ?? "unknown",
            privacy = new
            {
                originalExecutableIncluded = false,
                instructionBytesIncluded = false,
                sourcePathsIncluded = false,
                analystCommentsIncluded = false,
                exceptionMessagesIncluded = false,
                stackTracesIncluded = false,
                automaticNetworkUpload = false
            },
            recordedEvents = events.Length,
            droppedEvents = dropped,
            events = events.Select(x => new
            {
                x.Sequence, x.TimestampUtc,
                stage = x.Stage.ToString(),
                x.ArtifactSha256, x.ObjectNumber, x.ObjectOffset,
                x.InstructionCount, x.ErrorType
            })
        }, new JsonSerializerOptions { WriteIndented = true });
    }

    /// <summary>
    /// Atomically publish to a user-approved destination; never overwrite a
    /// previous report. The temporary file is in the destination directory.
    /// </summary>
    public void WriteReportNew(string path)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);
        string final = Path.GetFullPath(path);
        string directory = Path.GetDirectoryName(final)!;
        if (!Directory.Exists(directory))
            throw new DirectoryNotFoundException("The diagnostic export directory does not exist.");
        if (File.Exists(final) || Directory.Exists(final))
            throw new IOException("The diagnostic destination already exists; no files were replaced.");
        string staging = Path.Combine(directory, "." + Guid.NewGuid().ToString("N", CultureInfo.InvariantCulture) + ".diagnostic.part");
        try
        {
            using (var fs = new FileStream(staging, FileMode.CreateNew, FileAccess.Write, FileShare.None))
            using (var writer = new StreamWriter(fs, new UTF8Encoding(false)))
            {
                writer.Write(BuildJson());
                writer.Flush();
                fs.Flush(flushToDisk: true);
            }
            File.Move(staging, final); // no overwrite, including cross-process races
        }
        finally
        {
            if (File.Exists(staging)) File.Delete(staging);
        }
    }

    /// <summary>
    /// Best-effort private crash report; safe for use from exception callbacks.
    /// Does not upload or overwrite. Not every fatal/native crash is recoverable.
    /// </summary>
    public bool TryWriteLocalIncident()
    {
        try
        {
            string root = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
            if (string.IsNullOrWhiteSpace(root)) return false;
            string folder = Path.Combine(root, "DOSDis", "Incidents");
            Directory.CreateDirectory(folder);
            string name = "incident-" + DateTimeOffset.UtcNow.ToString("yyyyMMddTHHmmssfff", CultureInfo.InvariantCulture)
                + "-" + Guid.NewGuid().ToString("N", CultureInfo.InvariantCulture) + ".json";
            WriteReportNew(Path.Combine(folder, name));
            return true;
        }
        catch { return false; /* reporting must not cause a second crash */ }
    }
}

public enum DiagnosticStage
{
    AppStarted,
    FileOpenStarted,
    FileOpenedLe,
    FileOpenedRaw,
    AnalysisStarted,
    AnalysisCompleted,
    AnalysisCancelled,
    AnalysisFailed,
    ProjectOpened,
    ProjectSaved,
    AnalystCommentSaved,
    ProjectBackupSaved,
    AnalysisReportExported,
    DiagnosticReportExported,
    Navigation,
    HandledError,
    UnhandledUiException,
    UnhandledProcessException,
    UnobservedTaskException,
    AppClosed
}

public sealed record DiagnosticEvent(long Sequence, string TimestampUtc,
    DiagnosticStage Stage, string? ArtifactSha256, int? ObjectNumber,
    uint? ObjectOffset, int? InstructionCount, string? ErrorType);
