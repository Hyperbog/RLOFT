using System.Buffers.Binary;
using DosDis.Core;
using DosDis.Persistence;

static void Require(bool value, string explanation)
{
    if (!value) throw new InvalidOperationException(explanation);
}

// Unencumbered nested MZ/LE fixture for original-byte cache validation.
static byte[] MakeStoredLeFixture()
{
    var bytes = new byte[0x600];
    static void Dword(byte[] array, int at, uint value) =>
        BinaryPrimitives.WriteUInt32LittleEndian(array.AsSpan(at, 4), value);
    bytes[0] = (byte)'M'; bytes[1] = (byte)'Z';
    Dword(bytes, 0x3C, 0x80);
    bytes[0x80] = (byte)'L'; bytes[0x81] = (byte)'E';
    Dword(bytes, 0x80 + 0x14, 1); // one page
    Dword(bytes, 0x80 + 0x18, 1); // entry object
    Dword(bytes, 0x80 + 0x1C, 0); // entry offset
    Dword(bytes, 0x80 + 0x28, 4096);
    Dword(bytes, 0x80 + 0x2C, 4); // four bytes stored
    Dword(bytes, 0x80 + 0x40, 0xC4); // object table
    Dword(bytes, 0x80 + 0x44, 1);
    Dword(bytes, 0x80 + 0x48, 0xDC); // page map
    Dword(bytes, 0x80 + 0x80, 0x200); // data page relative to module
    Dword(bytes, 0x80 + 0xC4, 4);
    Dword(bytes, 0x80 + 0xC8, 0x10000);
    Dword(bytes, 0x80 + 0xCC, 0x2005); // executable
    Dword(bytes, 0x80 + 0xD0, 1);
    Dword(bytes, 0x80 + 0xD4, 1);
    bytes[0x80 + 0xDC + 2] = 1;
    bytes[0x200] = 0x90; bytes[0x201] = 0xC3;
    bytes[0x202] = 0x90; bytes[0x203] = 0xC3;
    return bytes;
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
        var source = SourceProvenance.ForArtifact(artifact, gameId: "stone", module: "CODE.1");
        project.EnsureSourceIdentity(source);
        var analystName = project.SaveNamedSymbol(source, 1, 0,
            "analyst", "analyst_start", "", 0);
        var inferredName = project.SaveNamedSymbol(source, 1, 0,
            "inferred", "possible_start", "candidate-cfg", 0);
        var verifiedName = project.SaveNamedSymbol(source, 1, 0,
            "verified", "fixture_start", "synthetic-independent-evidence", 0);
        Require(analystName.Revision == 1 && inferredName.Revision == 1 &&
            verifiedName.Revision == 1, "independent symbol tiers cannot be saved");
        Require(project.ListNamedSymbols("stone", artifact.Sha256).Count == 3,
            "three name provenance tiers must coexist at the same address");
        Require(project.ListNamedSymbols("menzo", artifact.Sha256).Count == 0,
            "a different game cannot inherit Stone's function names");
        bool otherGameRejected = false;
        try
        {
            project.EnsureSourceIdentity(source with {
                GameId = "menzo",
                IdentityStatus = "analyst-declared-game-unverified",
                ResearchNamespace = "menzo/" + artifact.Sha256 });
        }
        catch (InvalidDataException) { otherGameRejected = true; }
        Require(otherGameRejected, "same SHA-256 must not silently change game identity");
        bool fakeVerifiedRejected = false;
        try { project.EnsureSourceIdentity(source with {
            IdentityStatus = "verified-sha256-fingerprint" }); }
        catch (InvalidDataException) { fakeVerifiedRejected = true; }
        Require(fakeVerifiedRejected, "unarchived fingerprint cannot become verified identity");
        bool verifiedWithoutEvidenceRejected = false;
        try { project.SaveNamedSymbol(source, 1, 2, "verified", "unproved", "", 0); }
        catch (ArgumentException) { verifiedWithoutEvidenceRejected = true; }
        Require(verifiedWithoutEvidenceRejected,
            "evidence-backed name tiers require explicit research evidence");
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
        Require(!Directory.EnumerateFiles(folder, "*.part", SearchOption.TopDirectoryOnly).Any(),
            "successful SQLite backup leaked a staging database");
        using (var copy = ProjectStore.Open(backup))
        {
            copy.VerifyIntegrity();
            Require(copy.LoadLatestAnalysis(artifact.Sha256)?.Instructions.Count == 1,
                "WAL-safe backup lost analysis snapshot");
            Require(copy.GetAnnotation(artifact.Sha256, 1, 0, "comment")?.Body == "needs review",
                "WAL-safe backup lost latest analyst note");
            Require(copy.ListNamedSymbols("stone", artifact.Sha256).Count == 3,
                "WAL-safe backup lost separate provenance names");
        }
        bool existingBackupRejected = false;
        try { project.CreateBackup(backup); }
        catch (IOException) { existingBackupRejected = true; }
        Require(existingBackupRejected, "backup silently overwrote existing research");
        using (var copy = ProjectStore.Open(backup))
            Require(copy.GetAnnotation(artifact.Sha256, 1, 0, "comment")?.Body == "needs review",
                "a refused overwrite changed the existing backup");
        Require(!Directory.EnumerateFiles(folder, "*.part", SearchOption.TopDirectoryOnly).Any(),
            "refused backup left a staging database behind");
    }
    using (var reopened = ProjectStore.Open(path))
    {
        reopened.VerifyIntegrity();
        var loaded = reopened.LoadLatestAnalysis(artifact.Sha256)
            ?? throw new InvalidOperationException("analysis snapshot failed to reload");
        Require(loaded.ArtifactSha256 == artifact.Sha256,
            "analysis snapshot failed to match original artifact");
        Require(loaded.Instructions.Count == 1 && loaded.Instructions[0].OriginalBytes == "90",
            "instruction evidence changed after round trip");
        var note = reopened.GetAnnotation(artifact.Sha256, 1, 0, "comment");
        Require(note is not null && note.Revision == 2 && note.Body == "needs review",
            "analyst annotation failed round trip");
        Require(reopened.ListNamedSymbols("stone", artifact.Sha256).Count == 3,
            "separate verified/inferred/analyst names failed reopen");
        Require(reopened.LoadLatestAnalysis(new string('0', 64)) == null,
            "unrelated artifact incorrectly matched");
    }
    // Simulate an authentic V1 database using the earlier schema and reopen.
    // Legacy notes survive migration, without silent promotion to verified names.
    string legacyPath = Path.Combine(folder, "legacy-v1.ddproj");
    using (var latest = ProjectStore.Open(path)) latest.CreateBackup(legacyPath);
    using (var connection = new Microsoft.Data.Sqlite.SqliteConnection($"Data Source={legacyPath}"))
    {
        connection.Open();
        using var command = connection.CreateCommand();
        command.CommandText = """
            DROP TABLE named_symbols;
            DROP TABLE source_identities;
            PRAGMA user_version=1;
            """;
        command.ExecuteNonQuery();
    }
    using (var upgraded = ProjectStore.Open(legacyPath))
    {
        upgraded.VerifyIntegrity();
        Require(upgraded.GetAnnotation(artifact.Sha256, 1, 0, "comment")?.Body == "needs review",
            "V1-to-V2 migration lost legacy annotations");
        Require(upgraded.LoadLatestAnalysis(artifact.Sha256)?.Instructions.Count == 1,
            "V1-to-V2 migration lost prior candidate analysis");
        Require(upgraded.ListNamedSymbols("stone", artifact.Sha256).Count == 0,
            "migration must not silently promote legacy notes into verified symbols");
        upgraded.EnsureSourceIdentity(SourceProvenance.ForArtifact(artifact,
            gameId: "stone", module: "CODE.1"));
    }
    // A matching SHA-256 and well-formed SQLite snapshot alone do not prove
    // that the CACHED instruction evidence matches the reopened original.
    var sourceArtifact = BinaryArtifact.FromBytes(MakeStoredLeFixture(), "stored-source-fixture.exe");
    var sourceImage = LeImage.Load(sourceArtifact);
    var sourceGraph = ControlFlowAnalysis.Analyze(sourceImage, maxInstructions: 16);
    Require(sourceGraph.Instructions.Count >= 2, "synthetic source graph was empty");
    string sourceProjectPath = Path.Combine(folder, "source-verified.ddproj");
    using (var sourceProject = ProjectStore.Open(sourceProjectPath))
    {
        sourceProject.EnsureArtifact(sourceArtifact);
        sourceProject.SaveAnalysis(sourceGraph);
        var valid = sourceProject.LoadLatestAnalysis(sourceArtifact.Sha256, sourceImage);
        Require(valid?.Instructions.Count == sourceGraph.Instructions.Count,
            "genuine saved instruction bytes failed source-file verification");

        var modified = sourceGraph.Instructions.Select((item, index) =>
            index == 0 ? item with { OriginalBytes = "CC" } : item).ToArray();
        sourceProject.SaveAnalysis(sourceGraph with { Instructions = modified });
        bool alteredBytesRejected = false;
        try { sourceProject.LoadLatestAnalysis(sourceArtifact.Sha256, sourceImage); }
        catch (InvalidDataException) { alteredBytesRejected = true; }
        Require(alteredBytesRejected, "altered cached instruction bytes were trusted");

        // Duplicate instruction entries must not be accepted as independent
        // evidence even if the SQLite instruction-count metadata matches.
        sourceProject.SaveAnalysis(sourceGraph with
        {
            Instructions = sourceGraph.Instructions.Concat(new[] { sourceGraph.Instructions[0] }).ToArray()
        });
        bool duplicateRejected = false;
        try { sourceProject.LoadLatestAnalysis(sourceArtifact.Sha256, sourceImage); }
        catch (InvalidDataException) { duplicateRejected = true; }
        Require(duplicateRejected, "duplicate saved instruction starts were trusted");
    }
    Require(sourceArtifact.Sha256 == BinaryArtifact.FromBytes(MakeStoredLeFixture()).Sha256,
        "source-byte verifier changed the original artifact");

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
            var restored = recovered.LoadLatestAnalysis(originalHash, originalImage);
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
