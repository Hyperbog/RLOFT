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
    private const int SchemaVersion = 1;
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
        if (version != 0 && version != SchemaVersion)
            throw new InvalidDataException($"Unsupported research project schema version {version}; expected {SchemaVersion}. No migration attempted.");
        if (version == SchemaVersion) return;
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

    public FlowAnalysisResult? LoadLatestAnalysis(string artifactSha)
    {
        using var command = _db.CreateCommand();
        command.CommandText = """
            SELECT uncompressed_bytes,payload_brotli FROM analysis_runs
            WHERE artifact_sha=$hash ORDER BY id DESC LIMIT 1;
            """;
        command.Parameters.AddWithValue("$hash", artifactSha);
        using var reader = command.ExecuteReader();
        if (!reader.Read()) return null;
        long expected = reader.GetInt64(0);
        if (expected < 0 || expected > MaximumSnapshotBytes)
            throw new InvalidDataException("Invalid stored analysis snapshot length.");
        using var input = new MemoryStream((byte[])reader.GetValue(1), writable: false);
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
    /// Creates a consistent, closed SQLite project snapshot, including WAL
    /// transactions. A raw file copy of an active WAL database is unsafe.
    /// The backup destination must be a new path: never overwrite research.
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
        var builder = new SqliteConnectionStringBuilder
        {
            DataSource = destination,
            Mode = SqliteOpenMode.ReadWriteCreate,
            Cache = SqliteCacheMode.Private,
            Pooling = false
        };
        try
        {
            using var backup = new SqliteConnection(builder.ToString());
            backup.Open();
            _db.BackupDatabase(backup);
        }
        catch
        {
            // Never leave a half-written backup that might be mistaken for a
            // verified project. The original source connection stays untouched.
            try { if (File.Exists(destination)) File.Delete(destination); }
            catch (IOException) { /* The incomplete copy remains unusable. */ }
            throw;
        }
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
