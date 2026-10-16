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
    private readonly string _preferredLogsDirectory;
    private readonly string _fallbackLogsDirectory;
    private string? _lastIncidentPath;

    /// <summary>
    /// Portable deployments write next to the executable by default.
    /// A separately writable LocalAppData folder is used when the
    /// executable resides under a protected directory (e.g. Program Files).
    /// Explicit paths are for original-free synthetic tests only.
    /// </summary>
    public DiagnosticSession(int capacity = DefaultCapacity,
        string? preferredLogsDirectory = null, string? fallbackLogsDirectory = null)
    {
        if (capacity is < 8 or > 4096)
            throw new ArgumentOutOfRangeException(nameof(capacity), "Diagnostic event capacity must be 8–4096.");
        _capacity = capacity;
        _preferredLogsDirectory = Path.GetFullPath(preferredLogsDirectory ??
            Path.Combine(AppContext.BaseDirectory, "Logs"));
        _fallbackLogsDirectory = Path.GetFullPath(fallbackLogsDirectory ??
            Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                "DOSDis", "Logs"));
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
    /// Return (and create) the directory to open in Explorer. Prefer the
    /// directory where this session last wrote an incident, otherwise the
    /// portable Logs folder, with per-user fallback if not writable.
    /// Never open or modify the original source file.
    /// </summary>
    public string? TryGetLogsDirectory()
    {
        lock (_gate)
        {
            if (_lastIncidentPath is { } last &&
                Directory.Exists(Path.GetDirectoryName(last)))
                return Path.GetDirectoryName(last);
        }
        foreach (string folder in CandidateLogsDirectories())
        {
            try
            {
                Directory.CreateDirectory(folder);
                // Directory existence is not proof of write permission.
                // Probe atomically without replacing any existing file.
                string probe = Path.Combine(folder, "." +
                    Guid.NewGuid().ToString("N", CultureInfo.InvariantCulture) + ".probe");
                using (new FileStream(probe, FileMode.CreateNew, FileAccess.Write,
                    FileShare.None, 1, FileOptions.DeleteOnClose)) { }
                return folder;
            }
            catch { /* read-only install, I/O error, invalid path: try fallback */ }
        }
        return null;
    }

    /// <summary>
    /// Best-effort LOCAL report for handled/unhandled failures, always in
    /// a Logs folder. Never replaces an existing report, never automatically
    /// uploads anything, and never leaks original bytes, paths, messages or
    /// stack traces. Fatal/native crashes may bypass managed handlers.
    /// </summary>
    public bool TryWriteLocalIncident() => TryWriteLocalIncident(out _);

    public bool TryWriteLocalIncident(out string? incidentPath)
    {
        incidentPath = null;
        string filename = "incident-" +
            DateTimeOffset.UtcNow.ToString("yyyyMMddTHHmmssfff", CultureInfo.InvariantCulture) +
            "-" + Guid.NewGuid().ToString("N", CultureInfo.InvariantCulture) + ".json";
        foreach (string folder in CandidateLogsDirectories())
        {
            try
            {
                Directory.CreateDirectory(folder);
                string destination = Path.Combine(folder, filename);
                WriteReportNew(destination);
                lock (_gate) _lastIncidentPath = destination;
                incidentPath = destination;
                return true;
            }
            catch { /* logging must never cause or mask a second crash */ }
        }
        return false;
    }

    private IEnumerable<string> CandidateLogsDirectories()
    {
        yield return _preferredLogsDirectory;
        if (!string.Equals(_preferredLogsDirectory, _fallbackLogsDirectory,
            StringComparison.OrdinalIgnoreCase))
            yield return _fallbackLogsDirectory;
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
    AppClosed,
    FileOpenedMz,
    FileOpenMappingComplete,
    FileOpenPreviewComplete,
    FileOpenPresentationStarted,
    FileOpenPresentationComplete
}

public sealed record DiagnosticEvent(long Sequence, string TimestampUtc,
    DiagnosticStage Stage, string? ArtifactSha256, int? ObjectNumber,
    uint? ObjectOffset, int? InstructionCount, string? ErrorType);
