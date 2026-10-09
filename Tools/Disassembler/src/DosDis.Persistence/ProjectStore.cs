using System.Globalization;
using System.IO.Compression;
using System.Text;
using System.Text.Json;
using DosDis.Core;
using Microsoft.Data.Sqlite;

namespace DosDis.Persistence;

/// <summary>
/// Local, versioned single-user research project. It stores only identities,
/// analysis results and analyst notes; cached instructions do include binary
/// excerpts but the full proprietary input and file path are not embedded.
/// </summary>
public sealed class ProjectStore : IDisposable
{
    private const int SchemaVersion = 2;
    private const int MaximumSnapshotBytes = 128 * 1024 * 1024;
    private readonly SqliteConnection _db;
    private bool _disposed;

    public string FilePath { get; }

    private ProjectStore(string path)
    {
        FilePath = Path.GetFullPath(path);
        var builder = new SqliteConnectionStringBuilder
        {
            DataSource = FilePath, Mode = SqliteOpenMode.ReadWriteCreate,
            Cache = SqliteCacheMode.Private, Pooling = false
        };
        _db = new SqliteConnection(builder.ToString());
        _db.Open();
        try
        {
            Exec("PRAGMA foreign_keys=ON;");
            Exec("PRAGMA busy_timeout=5000;");
            Exec("PRAGMA journal_mode=WAL;");
            InitializeSchema();
        }
        catch
        {
            _db.Dispose();
            throw;
        }
    }

    public static ProjectStore Open(string path)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);
        return new ProjectStore(path);
    }

    private void Exec(string sql)
    {
        using var command = _db.CreateCommand();
        command.CommandText = sql;
        command.ExecuteNonQuery();
    }

    private int ScalarInt(string sql)
    {
        using var command = _db.CreateCommand();
        command.CommandText = sql;
        return Convert.ToInt32(command.ExecuteScalar(), CultureInfo.InvariantCulture);
    }

    private void InitializeSchema()
    {
        int version = ScalarInt("PRAGMA user_version;");
        if (version is not (0 or 1 or SchemaVersion))
            throw new InvalidDataException($"Unsupported research project schema version {version}; expected {SchemaVersion}. No migration attempted.");
        if (version == SchemaVersion) return;
        if (version == 1 &&
            ScalarInt("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name IN ('artifacts','analysis_runs','annotations');") != 3)
            throw new InvalidDataException("Legacy project is missing required V1 research tables.");
        if (version == 0)
        {
        // Refuse to import another SQLite database merely because it has user_version=0.
        if (ScalarInt("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';") != 0)
            throw new InvalidDataException("This SQLite file is not a new DOSDis project.");
        using var tx = _db.BeginTransaction();
        using var command = _db.CreateCommand();
        command.Transaction = tx;
        command.CommandText = """
            CREATE TABLE artifacts (
                sha256 TEXT PRIMARY KEY NOT NULL CHECK(length(sha256)=64),
                original_name TEXT NOT NULL,
                byte_length INTEGER NOT NULL CHECK(byte_length>=0),
                imported_utc TEXT NOT NULL
            ) STRICT;
            CREATE TABLE analysis_runs (
                id INTEGER PRIMARY KEY,
                artifact_sha TEXT NOT NULL REFERENCES artifacts(sha256),
                created_utc TEXT NOT NULL,
                analyzer_version TEXT NOT NULL,
                truncated INTEGER NOT NULL CHECK(truncated IN (0,1)),
                instruction_count INTEGER NOT NULL CHECK(instruction_count>=0),
                uncompressed_bytes INTEGER NOT NULL,
                payload_brotli BLOB NOT NULL
            ) STRICT;
            CREATE INDEX ix_analysis_artifact ON analysis_runs(artifact_sha, id DESC);
            CREATE TABLE annotations (
                artifact_sha TEXT NOT NULL REFERENCES artifacts(sha256),
                object_no INTEGER NOT NULL CHECK(object_no>=1),
                object_offset INTEGER NOT NULL CHECK(object_offset>=0),
                kind TEXT NOT NULL CHECK(kind IN ('comment','label','classification-note')),
                body TEXT NOT NULL,
                revision INTEGER NOT NULL CHECK(revision>=1),
                updated_utc TEXT NOT NULL,
                PRIMARY KEY(artifact_sha,object_no,object_offset,kind)
            ) STRICT;
            PRAGMA user_version=1;
            """;
        command.ExecuteNonQuery();
        tx.Commit();
        }

        // V1 projects retain cached graphs and legacy analyst labels verbatim.
        // No old label is silently promoted into an evidence-verified name.
        using var namesTx = _db.BeginTransaction();
        using var namesCommand = _db.CreateCommand();
        namesCommand.Transaction = namesTx;
        namesCommand.CommandText = """
            CREATE TABLE source_identities (
                sha256 TEXT PRIMARY KEY NOT NULL REFERENCES artifacts(sha256),
                game_id TEXT NOT NULL,
                module TEXT NOT NULL,
                byte_length INTEGER NOT NULL CHECK(byte_length>=0),
                identity_status TEXT NOT NULL,
                source_archive TEXT,
                internal_path TEXT,
                UNIQUE(sha256,game_id)
            ) STRICT;
            CREATE TABLE named_symbols (
                artifact_sha TEXT NOT NULL,
                game_id TEXT NOT NULL,
                object_no INTEGER NOT NULL CHECK(object_no>=1),
                object_offset INTEGER NOT NULL CHECK(object_offset>=0),
                role TEXT NOT NULL CHECK(role IN ('analyst','inferred','verified')),
                name TEXT NOT NULL,
                evidence_id TEXT NOT NULL,
                revision INTEGER NOT NULL CHECK(revision>=1),
                updated_utc TEXT NOT NULL,
                PRIMARY KEY(artifact_sha,game_id,object_no,object_offset,role),
                FOREIGN KEY(artifact_sha,game_id)
                    REFERENCES source_identities(sha256,game_id)
            ) STRICT;
            PRAGMA user_version=2;
            """;
        namesCommand.ExecuteNonQuery();
        namesTx.Commit();
    }

    public void EnsureArtifact(BinaryArtifact artifact)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        using var tx = _db.BeginTransaction();
        using (var insert = _db.CreateCommand())
        {
            insert.Transaction = tx;
            insert.CommandText = """
                INSERT OR IGNORE INTO artifacts(sha256,original_name,byte_length,imported_utc)
                VALUES($hash,$name,$length,$time);
                """;
            insert.Parameters.AddWithValue("$hash", artifact.Sha256);
            insert.Parameters.AddWithValue("$name", artifact.Name);
            insert.Parameters.AddWithValue("$length", artifact.Length);
            insert.Parameters.AddWithValue("$time", UtcNow());
            insert.ExecuteNonQuery();
        }
        using (var query = _db.CreateCommand())
        {
            query.Transaction = tx;
            query.CommandText = "SELECT byte_length FROM artifacts WHERE sha256=$hash;";
            query.Parameters.AddWithValue("$hash", artifact.Sha256);
            if (Convert.ToInt64(query.ExecuteScalar(), CultureInfo.InvariantCulture) != artifact.Length)
                throw new InvalidDataException("Existing artifact identity has inconsistent byte length.");
        }
        tx.Commit();
    }

    public IReadOnlyList<ProjectArtifact> ListArtifacts()
    {
        using var command = _db.CreateCommand();
        command.CommandText = "SELECT sha256,original_name,byte_length FROM artifacts ORDER BY imported_utc,sha256;";
        using var reader = command.ExecuteReader();
        var results = new List<ProjectArtifact>();
        while (reader.Read()) results.Add(new ProjectArtifact(reader.GetString(0),reader.GetString(1),reader.GetInt64(2)));
        return results;
    }

    public long SaveAnalysis(FlowAnalysisResult graph)
    {
        ArgumentNullException.ThrowIfNull(graph);
        byte[] raw = JsonSerializer.SerializeToUtf8Bytes(graph);
        if (raw.Length > MaximumSnapshotBytes)
            throw new InvalidDataException("Analysis exceeds prototype 128 MiB snapshot limit.");
        using var compressed = new MemoryStream();
        using (var compressor = new BrotliStream(compressed, CompressionLevel.Fastest, leaveOpen: true))
            compressor.Write(raw);
        using var tx = _db.BeginTransaction();
        using var command = _db.CreateCommand();
        command.Transaction = tx;
        command.CommandText = """
            INSERT INTO analysis_runs(artifact_sha,created_utc,analyzer_version,truncated,
                                      instruction_count,uncompressed_bytes,payload_brotli)
            VALUES($hash,$time,$version,$truncated,$count,$size,$payload);
            SELECT last_insert_rowid();
            """;
        command.Parameters.AddWithValue("$hash", graph.ArtifactSha256);
        command.Parameters.AddWithValue("$time", UtcNow());
        command.Parameters.AddWithValue("$version", "part18-candidate-cfg-v1");
        command.Parameters.AddWithValue("$truncated", graph.Truncated ? 1 : 0);
        command.Parameters.AddWithValue("$count", graph.Instructions.Count);
        command.Parameters.AddWithValue("$size", raw.Length);
        command.Parameters.AddWithValue("$payload", compressed.ToArray());
        long id = Convert.ToInt64(command.ExecuteScalar(), CultureInfo.InvariantCulture);
        tx.Commit();
        return id;
    }

    public FlowAnalysisResult? LoadLatestAnalysis(string artifactSha, LeImage? verifiedOriginal = null)
    {
        using var command = _db.CreateCommand();
        command.CommandText = """
            SELECT truncated,instruction_count,uncompressed_bytes,payload_brotli FROM analysis_runs
            WHERE artifact_sha=$hash ORDER BY id DESC LIMIT 1;
            """;
        command.Parameters.AddWithValue("$hash", artifactSha);
        using var reader = command.ExecuteReader();
        if (!reader.Read()) return null;
        long storedTruncated = reader.GetInt64(0);
        long storedInstructionCount = reader.GetInt64(1);
        long expected = reader.GetInt64(2);
        if (storedTruncated is not (0 or 1) || storedInstructionCount < 0 || storedInstructionCount > 1_000_000)
            throw new InvalidDataException("Invalid stored analysis metadata.");
        if (expected < 0 || expected > MaximumSnapshotBytes)
            throw new InvalidDataException("Invalid stored analysis snapshot length.");
        using var input = new MemoryStream((byte[])reader.GetValue(3), writable: false);
        using var decompressed = new BrotliStream(input, CompressionMode.Decompress);
        using var output = new MemoryStream();
        var chunk = new byte[32 * 1024];
        int n;
        while ((n = decompressed.Read(chunk)) != 0)
        {
            if (output.Length + n > expected || output.Length + n > MaximumSnapshotBytes)
                throw new InvalidDataException("Stored snapshot exceeded its declared uncompressed length.");
            output.Write(chunk, 0, n);
        }
        if (output.Length != expected)
            throw new InvalidDataException("Stored snapshot length mismatch.");
        FlowAnalysisResult result = JsonSerializer.Deserialize<FlowAnalysisResult>(output.ToArray())
            ?? throw new InvalidDataException("Stored analysis payload is empty.");
        if (!string.Equals(result.ArtifactSha256, artifactSha, StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("Stored snapshot artifact identity mismatch.");
        if (result.Instructions is null || result.Edges is null || result.Blocks is null ||
            result.FunctionCandidates is null || result.Ranges is null || result.Diagnostics is null ||
            result.DataReferences is null || result.JumpTables is null || result.Seeds is null ||
            result.ProtectedData is null || result.Instructions.Count != storedInstructionCount ||
            result.Truncated != (storedTruncated == 1))
            throw new InvalidDataException("Stored analysis payload disagrees with its metadata.");
        // The SQLite payload can be structurally valid but contain incorrect
        // instruction bytes/addresses. If the original is open, verify every
        // saved instruction against its exact immutable source-file mapping.
        if (verifiedOriginal is not null)
            SnapshotEvidenceVerifier.Verify(result, verifiedOriginal);
        return result;
    }

    public IReadOnlyList<ProjectAnnotation> ListAnnotations(string artifactSha)
    {
        using var command = _db.CreateCommand();
        command.CommandText = """
            SELECT object_no,object_offset,kind,body,revision,updated_utc
            FROM annotations WHERE artifact_sha=$hash ORDER BY object_no,object_offset,kind;
            """;
        command.Parameters.AddWithValue("$hash", artifactSha);
        using var reader = command.ExecuteReader();
        var items = new List<ProjectAnnotation>();
        while (reader.Read())
            items.Add(new ProjectAnnotation(artifactSha,reader.GetInt32(0),checked((uint)reader.GetInt64(1)),
                reader.GetString(2),reader.GetString(3),reader.GetInt64(4),reader.GetString(5)));
        return items;
    }

    public ProjectAnnotation? GetAnnotation(string hash,int objectNumber,uint offset,string kind)
    {
        using var command = _db.CreateCommand();
        command.CommandText = """
            SELECT body,revision,updated_utc FROM annotations
            WHERE artifact_sha=$hash AND object_no=$object AND object_offset=$offset AND kind=$kind;
            """;
        AddLocation(command,hash,objectNumber,offset,kind);
        using var reader = command.ExecuteReader();
        return reader.Read() ? new ProjectAnnotation(hash,objectNumber,offset,kind,
            reader.GetString(0),reader.GetInt64(1),reader.GetString(2)) : null;
    }

    /// <summary>Optimistic revision check; caller must review on stale edits.</summary>
    public ProjectAnnotation SaveAnnotation(string hash,int objectNumber,uint offset,string kind,
        string body,long expectedRevision)
    {
        if (kind is not ("comment" or "label" or "classification-note"))
            throw new ArgumentException("Unsupported annotation kind.",nameof(kind));
        if (objectNumber < 1 || body.Length > 16000 || expectedRevision < 0)
            throw new ArgumentOutOfRangeException(nameof(expectedRevision));
        using var tx = _db.BeginTransaction();
        using var command = _db.CreateCommand();
        command.Transaction = tx;
        string time = UtcNow();
        if (expectedRevision == 0)
        {
            command.CommandText = """
                INSERT OR IGNORE INTO annotations
                    (artifact_sha,object_no,object_offset,kind,body,revision,updated_utc)
                VALUES($hash,$object,$offset,$kind,$body,1,$time);
                """;
        }
        else
        {
            command.CommandText = """
                UPDATE annotations SET body=$body, revision=revision+1, updated_utc=$time
                WHERE artifact_sha=$hash AND object_no=$object AND object_offset=$offset
                      AND kind=$kind AND revision=$expected;
                """;
            command.Parameters.AddWithValue("$expected", expectedRevision);
        }
        AddLocation(command,hash,objectNumber,offset,kind);
        command.Parameters.AddWithValue("$body",body);
        command.Parameters.AddWithValue("$time",time);
        if (command.ExecuteNonQuery() != 1)
            throw new InvalidOperationException("Annotation changed since it was opened. Reload it before saving.");
        tx.Commit();
        return new ProjectAnnotation(hash,objectNumber,offset,kind,body,expectedRevision+1,time);
    }

    /// <summary>
    /// Writes a validated SQLite backup to a unique sibling file, then
    /// atomically moves it into the requested destination without overwrite.
    /// This prevents a crash or a concurrent new file from exposing an
    /// incomplete backup as a saved research project.
    /// </summary>
    public void CreateBackup(string destinationPath)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(destinationPath);
        string destination = Path.GetFullPath(destinationPath);
        if (string.Equals(destination, FilePath, StringComparison.OrdinalIgnoreCase))
            throw new InvalidOperationException("A project backup must have a different path.");
        if (File.Exists(destination))
            throw new IOException("Backup destination exists; refusing to overwrite a project.");
        string? parent = Path.GetDirectoryName(destination);
        if (parent is null || !Directory.Exists(parent))
            throw new DirectoryNotFoundException("Backup destination directory does not exist.");

        // The staging database must be on the same volume as the destination
        // for the final non-overwriting rename to be atomic. It is never a
        // public .ddproj path until SQLite has closed and verification passes.
        string temporary = Path.Combine(parent,
            "." + Path.GetFileName(destination) + "." + Guid.NewGuid().ToString("N") + ".part");
        var builder = new SqliteConnectionStringBuilder
        {
            DataSource = temporary,
            Mode = SqliteOpenMode.ReadWriteCreate,
            Cache = SqliteCacheMode.Private,
            Pooling = false
        };
        try
        {
            using (var backup = new SqliteConnection(builder.ToString()))
            {
                backup.Open();
                _db.BackupDatabase(backup);
                // Ensure the staged copy is a self-contained database file,
                // not a database whose newest pages still live in a -wal file.
                using (var journal = backup.CreateCommand())
                {
                    journal.CommandText = "PRAGMA journal_mode=DELETE;";
                    if (!string.Equals(journal.ExecuteScalar()?.ToString(), "delete",
                        StringComparison.OrdinalIgnoreCase))
                        throw new InvalidDataException("SQLite backup could not enter standalone journal mode.");
                }
                using var check = backup.CreateCommand();
                check.CommandText = "PRAGMA integrity_check;";
                if (!string.Equals(check.ExecuteScalar()?.ToString(), "ok",
                    StringComparison.OrdinalIgnoreCase))
                    throw new InvalidDataException("SQLite backup failed integrity validation.");
                check.CommandText = "PRAGMA foreign_key_check;";
                using var violations = check.ExecuteReader();
                if (violations.Read())
                    throw new InvalidDataException("SQLite backup contains foreign-key violations.");
            }

            // Reopen after closing the writer. Integrity verification at this
            // point proves the main file can be read independently of WAL.
            if (File.Exists(temporary + "-wal") || File.Exists(temporary + "-shm"))
                throw new InvalidDataException("SQLite backup still has live journal sidecars.");
            var readOnlyBuilder = new SqliteConnectionStringBuilder
            {
                DataSource = temporary, Mode = SqliteOpenMode.ReadOnly,
                Cache = SqliteCacheMode.Private, Pooling = false
            };
            using (var verify = new SqliteConnection(readOnlyBuilder.ToString()))
            {
                verify.Open();
                using var check = verify.CreateCommand();
                check.CommandText = "PRAGMA integrity_check;";
                if (!string.Equals(check.ExecuteScalar()?.ToString(), "ok",
                    StringComparison.OrdinalIgnoreCase))
                    throw new InvalidDataException("Standalone SQLite backup failed validation.");
            }

            // File.Move with overwrite:false refuses an existing destination,
            // including one created by another process after our first check.
            File.Move(temporary, destination, overwrite: false);
        }
        finally
        {
            // Clean only OUR unique staging artifacts. Never delete the
            // requested destination even when another process created it.
            foreach (string suffix in new[] { "", "-wal", "-shm" })
            {
                string fragment = temporary + suffix;
                try { if (File.Exists(fragment)) File.Delete(fragment); }
                catch (IOException) { /* Cleanup best-effort; do not obscure original error. */ }
                catch (UnauthorizedAccessException) { /* Same. */ }
            }
        }
    }

    /// <summary>
    /// Binds one game research namespace to an existing immutable artifact.
    /// Rejects conflicting claims; never trusts the filename as identity.
    /// </summary>
    public void EnsureSourceIdentity(SourceProvenance identity)
    {
        ArgumentNullException.ThrowIfNull(identity);
        identity.ValidateClaim();
        if (identity.GameId == "unassigned" || identity.ByteLength is not long length)
            throw new ArgumentException("A named research namespace requires an explicit game identity.");
        using var tx = _db.BeginTransaction();
        using (var insert = _db.CreateCommand())
        {
            insert.Transaction = tx;
            insert.CommandText = """
                INSERT OR IGNORE INTO source_identities
                    (sha256,game_id,module,byte_length,identity_status,source_archive,internal_path)
                VALUES($hash,$game,$module,$length,$status,$archive,$internal);
                """;
            insert.Parameters.AddWithValue("$hash",identity.SourceSha256);
            insert.Parameters.AddWithValue("$game",identity.GameId);
            insert.Parameters.AddWithValue("$module",identity.Module);
            insert.Parameters.AddWithValue("$length",length);
            insert.Parameters.AddWithValue("$status",identity.IdentityStatus);
            insert.Parameters.AddWithValue("$archive",(object?)identity.SourceArchive ?? DBNull.Value);
            insert.Parameters.AddWithValue("$internal",(object?)identity.InternalPath ?? DBNull.Value);
            insert.ExecuteNonQuery();
        }
        using (var read = _db.CreateCommand())
        {
            read.Transaction = tx;
            read.CommandText = """
                SELECT game_id,module,byte_length,identity_status,source_archive,internal_path
                FROM source_identities WHERE sha256=$hash;
                """;
            read.Parameters.AddWithValue("$hash",identity.SourceSha256);
            using var row = read.ExecuteReader();
            if (!row.Read() || row.GetString(0) != identity.GameId ||
                row.GetString(1) != identity.Module || row.GetInt64(2) != length ||
                row.GetString(3) != identity.IdentityStatus ||
                (row.IsDBNull(4) ? null : row.GetString(4)) != identity.SourceArchive ||
                (row.IsDBNull(5) ? null : row.GetString(5)) != identity.InternalPath)
                throw new InvalidDataException("Conflicting persistent game/source namespace assignment.");
        }
        tx.Commit();
    }

    public IReadOnlyList<ProjectNamedSymbol> ListNamedSymbols(string gameId,string sourceSha)
    {
        using var command = _db.CreateCommand();
        command.CommandText = """
            SELECT object_no,object_offset,role,name,evidence_id,revision,updated_utc
            FROM named_symbols WHERE game_id=$game AND artifact_sha=$hash
            ORDER BY object_no,object_offset,role;
            """;
        command.Parameters.AddWithValue("$game",gameId);
        command.Parameters.AddWithValue("$hash",sourceSha);
        using var reader = command.ExecuteReader();
        var items = new List<ProjectNamedSymbol>();
        while(reader.Read())
            items.Add(new ProjectNamedSymbol(gameId,sourceSha,reader.GetInt32(0),
                checked((uint)reader.GetInt64(1)),reader.GetString(2),
                reader.GetString(3),reader.GetString(4),
                reader.GetInt64(5),reader.GetString(6)));
        return items;
    }

    /// <summary>Separate mutable analyst, inferred and evidence-backed name tiers.</summary>
    public ProjectNamedSymbol SaveNamedSymbol(SourceProvenance source,
        int objectNumber,uint offset,string role,string name,
        string evidenceId,long expectedRevision)
    {
        ArgumentNullException.ThrowIfNull(source);
        source.ValidateClaim();
        if (source.GameId == "unassigned" ||
            role is not ("analyst" or "inferred" or "verified") ||
            string.IsNullOrWhiteSpace(name) || name.Length > 128 ||
            evidenceId is null || evidenceId.Length > 512 ||
            (role == "verified" && string.IsNullOrWhiteSpace(evidenceId)) ||
            objectNumber < 1 || expectedRevision < 0)
            throw new ArgumentException("Invalid source-bound symbol name, role or evidence.");
        // Check the complete game/module/length claim, not just the foreign
        // key's hash+game pair, before admitting another independent name.
        EnsureSourceIdentity(source);
        using var tx = _db.BeginTransaction();
        using var cmd = _db.CreateCommand();
        cmd.Transaction = tx;
        cmd.CommandText = expectedRevision == 0 ? """
            INSERT OR IGNORE INTO named_symbols
                (artifact_sha,game_id,object_no,object_offset,role,name,evidence_id,revision,updated_utc)
            VALUES($hash,$game,$object,$offset,$role,$name,$evidence,1,$time);
            """ : """
            UPDATE named_symbols SET name=$name,evidence_id=$evidence,
                revision=revision+1,updated_utc=$time
            WHERE artifact_sha=$hash AND game_id=$game AND object_no=$object
                AND object_offset=$offset AND role=$role AND revision=$expected;
            """;
        cmd.Parameters.AddWithValue("$hash",source.SourceSha256);
        cmd.Parameters.AddWithValue("$game",source.GameId);
        cmd.Parameters.AddWithValue("$object",objectNumber);
        cmd.Parameters.AddWithValue("$offset",(long)offset);
        cmd.Parameters.AddWithValue("$role",role);
        cmd.Parameters.AddWithValue("$name",name);
        cmd.Parameters.AddWithValue("$evidence",evidenceId);
        string now = UtcNow();
        cmd.Parameters.AddWithValue("$time",now);
        if(expectedRevision!=0)cmd.Parameters.AddWithValue("$expected",expectedRevision);
        if(cmd.ExecuteNonQuery()!=1)
            throw new InvalidOperationException("Named symbol changed or its source namespace was not registered.");
        tx.Commit();
        return new ProjectNamedSymbol(source.GameId,source.SourceSha256,
            objectNumber,offset,role,name,evidenceId,expectedRevision+1,now);
    }

    public void VerifyIntegrity()
    {
        using var command = _db.CreateCommand();
        command.CommandText = "PRAGMA integrity_check;";
        if (!string.Equals(command.ExecuteScalar()?.ToString(),"ok",StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("SQLite integrity check failed.");
        command.CommandText = "PRAGMA foreign_key_check;";
        using var rows = command.ExecuteReader();
        if (rows.Read()) throw new InvalidDataException("SQLite foreign key check failed.");
    }

    private static void AddLocation(SqliteCommand command,string hash,int obj,uint offset,string kind)
    {
        command.Parameters.AddWithValue("$hash",hash);
        command.Parameters.AddWithValue("$object",obj);
        command.Parameters.AddWithValue("$offset",(long)offset);
        command.Parameters.AddWithValue("$kind",kind);
    }
    private static string UtcNow() => DateTimeOffset.UtcNow.ToString("O",CultureInfo.InvariantCulture);
    public void Dispose() { if (_disposed) return; _disposed = true; _db.Dispose(); }
}

public sealed record ProjectArtifact(string Sha256,string OriginalName,long Length);
public sealed record ProjectAnnotation(string ArtifactSha256,int ObjectNumber,uint Offset,
    string Kind,string Body,long Revision,string UpdatedUtc);
public sealed record ProjectNamedSymbol(string GameId,string ArtifactSha256,
    int ObjectNumber,uint Offset,string Role,string Name,string EvidenceId,
    long Revision,string UpdatedUtc);
