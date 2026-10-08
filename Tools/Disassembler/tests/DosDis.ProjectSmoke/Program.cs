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
    Console.WriteLine("PASS: SQLite identity, compressed analysis round-trip, analyst note revision/concurrency, integrity and reopen");
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
