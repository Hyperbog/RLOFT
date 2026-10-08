using DosDis.Core;
using DosDis.Persistence;

static void Require(bool value, string explanation)
{
    if (!value) throw new InvalidOperationException(explanation);
}

string folder = Path.Combine(Path.GetTempPath(), "dosdis-project-smoke-" + Guid.NewGuid().ToString("N"));
Directory.CreateDirectory(folder);
string path = Path.Combine(folder, "fixture.ddproj");
try
{
    // Artificial data: the test never needs or distributes any game executable.
    byte[] fixture = { 0x4D, 0x5A, 0x90, 0xC3 };
    var artifact = BinaryArtifact.FromBytes(fixture, "synthetic-fixture.exe");
    var address = new FlowAddress(1, 0);
    var graph = new FlowAnalysisResult(artifact.Sha256,
        new[] { address }, Array.Empty<ProtectedDataRange>(),
        new[] { new FlowInstruction(address, 1, "90", "nop", "Next", "candidate-reachable-from-seed") },
        Array.Empty<FlowEdge>(),
        new[] { new FlowBlock(address, 1, new[] { address }) },
        new[] { new FunctionCandidate(address, new[] { address }, "analyst-or-module-entry-seed") },
        new[] { new ClassifiedRange(1, 0, 1, "candidate-code", "reachable-from-seed") },
        Array.Empty<FlowDiagnostic>(), Array.Empty<DataCrossReference>(),
        Array.Empty<JumpTableCandidate>(), false);

    using (var project = ProjectStore.Open(path))
    {
        project.EnsureArtifact(artifact);
        Require(project.ListArtifacts().Count == 1, "artifact identity missing");
        Require(project.SaveAnalysis(graph) >= 1, "analysis snapshot ID invalid");
        var note = project.SaveAnnotation(artifact.Sha256, 1, 0, "comment", "candidate, not verified", 0);
        Require(note.Revision == 1, "initial revision incorrect");
        var edited = project.SaveAnnotation(artifact.Sha256, 1, 0, "comment", "needs review", 1);
        Require(edited.Revision == 2, "revision did not increment");
        bool staleRejected = false;
        try { project.SaveAnnotation(artifact.Sha256, 1, 0, "comment", "wrong", 1); }
        catch (InvalidOperationException) { staleRejected = true; }
        Require(staleRejected, "stale annotation was permitted to overwrite the analyst's change");
        project.VerifyIntegrity();
        string backup = Path.Combine(folder, "backup.ddproj");
        project.CreateBackup(backup);
        Require(File.Exists(backup), "transactionally consistent SQLite backup missing");
        using (var copy = ProjectStore.Open(backup))
        {
            copy.VerifyIntegrity();
            Require(copy.LoadLatestAnalysis(artifact.Sha256)?.Instructions.Count == 1,
                "WAL-safe backup lost analysis snapshot");
            Require(copy.GetAnnotation(artifact.Sha256, 1, 0, "comment")?.Body == "needs review",
                "WAL-safe backup lost latest analyst note");
        }
        bool existingBackupRejected = false;
        try { project.CreateBackup(backup); }
        catch (IOException) { existingBackupRejected = true; }
        Require(existingBackupRejected, "backup silently overwrote existing research");
    }
    using (var reopened = ProjectStore.Open(path))
    {
        reopened.VerifyIntegrity();
        var loaded = reopened.LoadLatestAnalysis(artifact.Sha256);
        Require(loaded is not null && loaded.ArtifactSha256 == artifact.Sha256,
            "analysis snapshot failed to reload");
        Require(loaded.Instructions.Count == 1 && loaded.Instructions[0].OriginalBytes == "90",
            "instruction evidence changed after round trip");
        var note = reopened.GetAnnotation(artifact.Sha256, 1, 0, "comment");
        Require(note is not null && note.Revision == 2 && note.Body == "needs review",
            "analyst annotation failed round trip");
        Require(reopened.LoadLatestAnalysis(new string('0', 64)) == null,
            "unrelated artifact incorrectly matched");
    }
    // A malformed project must not display metadata inconsistent with the cached graph.
    string inconsistent = Path.Combine(folder, "inconsistent-snapshot.ddproj");
    using (var original = ProjectStore.Open(path)) original.CreateBackup(inconsistent);
    using (var connection = new Microsoft.Data.Sqlite.SqliteConnection($"Data Source={inconsistent}"))
    {
        connection.Open();
        using var command = connection.CreateCommand();
        command.CommandText = "UPDATE analysis_runs SET instruction_count=instruction_count+1;";
        Require(command.ExecuteNonQuery() == 1, "synthetic metadata mismatch fixture missing");
    }
    using (var modified = ProjectStore.Open(inconsistent))
    {
        bool mismatchRejected = false;
        try { modified.LoadLatestAnalysis(artifact.Sha256); }
        catch (InvalidDataException) { mismatchRejected = true; }
        Require(mismatchRejected, "inconsistent cached analysis metadata was trusted");
    }
    // Do not guess how to interpret newer project schemas.
    string incompatible = Path.Combine(folder, "future-schema.ddproj");
    using (var future = ProjectStore.Open(incompatible)) { }
    using (var connection = new Microsoft.Data.Sqlite.SqliteConnection($"Data Source={incompatible}"))
    {
        connection.Open();
        using var command = connection.CreateCommand();
        command.CommandText = "PRAGMA user_version=999;";
        command.ExecuteNonQuery();
    }
    bool incompatibleRejected = false;
    try { using var future = ProjectStore.Open(incompatible); }
    catch (InvalidDataException) { incompatibleRejected = true; }
    Require(incompatibleRejected, "unknown project schema was silently accepted");

    if (args.Length != 0 && args.Length != 2)
        throw new ArgumentException("Pass either no files or both verified original CODE.1 and CODE.2 paths.");
    foreach (string original in args)
    {
        var originalArtifact = BinaryArtifact.Open(original);
        string originalHash = originalArtifact.Sha256;
        var originalImage = LeImage.Load(originalArtifact);
        var limited = ControlFlowAnalysis.Analyze(originalImage, maxInstructions: 1000);
        Require(limited.Instructions.Count > 0, "original binary did not produce entry-reachable instructions");
        string originalProject = Path.Combine(folder, originalArtifact.Name + ".ddproj");
        using (var savedProject = ProjectStore.Open(originalProject))
        {
            savedProject.EnsureArtifact(originalArtifact);
            savedProject.SaveAnalysis(limited);
            savedProject.VerifyIntegrity();
        }
        using (var recovered = ProjectStore.Open(originalProject))
        {
            var restored = recovered.LoadLatestAnalysis(originalHash);
            Require(restored is not null && restored.Instructions.Count == limited.Instructions.Count &&
                restored.Edges.Count == limited.Edges.Count, "original-binary project graph mismatch after reopen");
        }
        Require(BinaryArtifact.Open(original).Sha256 == originalHash,
            "original binary content changed during project regression");
        Console.WriteLine($"PASS: {originalArtifact.Name} bounded original-binary project round trip");
    }
    Console.WriteLine("PASS: SQLite identity, WAL-safe backup, schema guard, snapshot metadata, analysis round-trip, annotation conflict, integrity and reopen");
    return 0;
}
catch (Exception error)
{
    Console.Error.WriteLine("FAIL: " + error);
    return 1;
}
finally
{
    try { Directory.Delete(folder, recursive: true); }
    catch (IOException) { /* Windows may briefly retain the SQLite journal handle. */ }
}
