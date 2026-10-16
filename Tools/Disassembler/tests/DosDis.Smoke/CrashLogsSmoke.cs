using DosDis.Core;

/// <summary>
/// Original-free filesystem checks for user-accessible crash reports.
/// All files live beneath one test-owned temporary directory and are
/// cleaned up; never touches the user's real DOSDis Logs folder.
/// </summary>
internal static class CrashLogsSmoke
{
    private static void Require(bool ok, string reason)
    {
        if (!ok) throw new Exception("Crash Logs smoke: " + reason);
    }

    public static void Run()
    {
        string root = Path.Combine(Path.GetTempPath(),
            "dosdis-synthetic-crashlogs-" + Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(root);
        try
        {
            string portable = Path.Combine(root, "portable", "Logs");
            string fallback = Path.Combine(root, "user", "DOSDis", "Logs");
            var diagnostic = new DiagnosticSession(preferredLogsDirectory: portable,
                fallbackLogsDirectory: fallback);
            diagnostic.Record(DiagnosticStage.FileOpenPreviewComplete,
                new string('a', 64));
            diagnostic.Record(DiagnosticStage.UnhandledUiException,
                error: new InvalidOperationException(
                    "PRIVATE ORIGINAL FILE PATH C:\\never-share\\CODE.1"));
            Require(diagnostic.TryGetLogsDirectory() == portable,
                "writable portable Logs must be preferred");
            Require(diagnostic.TryWriteLocalIncident(out string? first) &&
                first is not null && Path.GetDirectoryName(first) == portable &&
                File.Exists(first), "portable incident is stored beside the executable");
            string firstPath = first ?? throw new Exception("Crash Logs: portable report missing");
            Require(Path.GetFileName(firstPath).StartsWith("incident-", StringComparison.Ordinal) &&
                firstPath.EndsWith(".json", StringComparison.Ordinal),
                "unique JSON incident naming is retained");
            string json = File.ReadAllText(firstPath);
            Require(json.Contains("UnhandledUiException", StringComparison.Ordinal) &&
                json.Contains("System.InvalidOperationException", StringComparison.Ordinal),
                "failure stage and sanitized exception class must be present");
            Require(!json.Contains("PRIVATE ORIGINAL", StringComparison.Ordinal) &&
                !json.Contains("never-share", StringComparison.Ordinal) &&
                !json.Contains("CODE.1", StringComparison.Ordinal),
                "incident must never serialize exception messages, private paths or bytes");
            Require(diagnostic.TryWriteLocalIncident(out string? second) &&
                second is not null && second != first && File.Exists(first) &&
                File.Exists(second), "subsequent crash must create a new file without overwrites");
            Require(diagnostic.TryGetLogsDirectory() == portable,
                "Logs button must find last incident directory");

            // Windows Program Files and locked-down deployments may block
            // writes beside an executable. An ordinary file makes the
            // would-be Logs folder deterministically unwritable on all OSes.
            string blocked = Path.Combine(root, "read-only-install", "Logs");
            Directory.CreateDirectory(Path.GetDirectoryName(blocked)!);
            File.WriteAllText(blocked, "synthetic obstacle");
            var protectedInstall = new DiagnosticSession(preferredLogsDirectory: blocked,
                fallbackLogsDirectory: fallback);
            protectedInstall.Record(DiagnosticStage.UnhandledProcessException,
                error: new InvalidDataException("PRIVATE ORIGINAL BYTE STRING"));
            Require(protectedInstall.TryGetLogsDirectory() == fallback,
                "unwritable portable Logs must resolve to per-user fallback");
            Require(protectedInstall.TryWriteLocalIncident(out string? other) &&
                other is not null && Path.GetDirectoryName(other) == fallback &&
                File.Exists(other), "fallback incident must land in per-user Logs folder");
            string fallbackReport = other ?? throw new Exception("Crash Logs: fallback report missing");
            Require(!File.ReadAllText(fallbackReport).Contains("PRIVATE ORIGINAL", StringComparison.Ordinal),
                "fallback must preserve private-report redaction");
            Require(protectedInstall.TryGetLogsDirectory() == fallback,
                "Logs button must expose the actual fallback directory");
            Require(File.ReadAllText(blocked) == "synthetic obstacle",
                "existing user files must not be overwritten");

            string blockedFallback = Path.Combine(root, "no-user-space", "Logs");
            Directory.CreateDirectory(Path.GetDirectoryName(blockedFallback)!);
            File.WriteAllText(blockedFallback, "another obstacle");
            var inaccessible = new DiagnosticSession(preferredLogsDirectory: blocked,
                fallbackLogsDirectory: blockedFallback);
            Require(inaccessible.TryGetLogsDirectory() is null,
                "Logs directory query must fail closed if both paths are blocked");
            Require(!inaccessible.TryWriteLocalIncident(out string? missing) &&
                missing is null, "unwritable crash reporting must never throw or claim success");
        }
        finally
        {
            Directory.Delete(root, recursive: true);
        }
        Console.WriteLine("PASS: crash incidents in portable Logs, fallback per-user Logs, " +
            "unique files, sanitized diagnostics, Explorer path and safe no-write failure");
    }
}
