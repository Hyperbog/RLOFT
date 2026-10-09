namespace DosDis.Core;

/// <summary>
/// Game-specific research identity. Only source bytes (SHA-256 and length)
/// establish artifact identity; game/archive labels are evidence metadata.
/// </summary>
public sealed record SourceProvenance(
    int SchemaVersion, string GameId, string Module, string SourceSha256,
    long? ByteLength, string ExecutableFormat, string IdentityStatus,
    string ResearchNamespace, string? SourceArchive, string? InternalPath)
{
    public const int CurrentSchema = 1;
    private static readonly IReadOnlyDictionary<string, (string Game, string Module, int Size)> Known =
        new Dictionary<string, (string, string, int)>(StringComparer.OrdinalIgnoreCase)
        {
            ["fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed"] =
                ("strahd", "CODE.1", 277787),
            // Strahd CODE.2 is recognized only by its complete archived SHA.
            ["853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"] =
                ("strahd", "CODE.2", 0)
        };

    public static SourceProvenance FromGraph(FlowAnalysisResult graph)
    {
        ArgumentNullException.ThrowIfNull(graph);
        ValidateHash(graph.ArtifactSha256);
        return new SourceProvenance(CurrentSchema, "unassigned", "unassigned",
            graph.ArtifactSha256.ToLowerInvariant(), null, "LE",
            "source-hash-only-game-unassigned",
            "unassigned/" + graph.ArtifactSha256.ToLowerInvariant(), null, null);
    }

    public static SourceProvenance ForArtifact(BinaryArtifact artifact,
        LeImage? image = null, string? gameId = null, string? module = null,
        string? sourceArchive = null, string? internalPath = null)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        ValidateHash(artifact.Sha256);
        if (image is not null && !string.Equals(image.ArtifactSha256, artifact.Sha256,
                StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("Provenance LE image belongs to another binary.");

        bool known = Known.TryGetValue(artifact.Sha256, out var knownSource);
        if (known && knownSource.Size != 0 && artifact.Length != knownSource.Size)
            throw new InvalidDataException("Archived fingerprint has inconsistent size.");
        string labelGame = known ? knownSource.Game : gameId ?? "unassigned";
        string labelModule = known ? knownSource.Module : module ?? artifact.Name;
        if (known && gameId is not null && gameId != knownSource.Game)
            throw new InvalidDataException("The requested game identity conflicts with a verified source fingerprint.");
        if (known && module is not null && !string.Equals(module, knownSource.Module, StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("The requested module identity conflicts with a verified source fingerprint.");
        if (!known && module is not null) labelModule = module;
        ValidateSlug(labelGame, nameof(gameId));
        ValidateModule(labelModule);
        ValidateRelative(sourceArchive, nameof(sourceArchive));
        ValidateRelative(internalPath, nameof(internalPath));

        return new SourceProvenance(CurrentSchema, labelGame, labelModule,
            artifact.Sha256, artifact.Length, image is null ? "unknown" : "LE",
            known ? "verified-sha256-fingerprint"
                  : gameId is null ? "source-hash-only-game-unassigned" : "analyst-declared-game-unverified",
            labelGame + "/" + artifact.Sha256,
            sourceArchive, internalPath);
    }

    public void RequireGraph(FlowAnalysisResult graph)
    {
        ArgumentNullException.ThrowIfNull(graph);
        if (!string.Equals(SourceSha256, graph.ArtifactSha256, StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("Report provenance references another original executable.");
    }

    private static void ValidateHash(string hash)
    {
        if (hash.Length != 64 ||
            !hash.All(c => c is >= '0' and <= '9' or >= 'a' and <= 'f' or >= 'A' and <= 'F'))
            throw new InvalidDataException("Source identity must contain an exact 64-character SHA-256.");
    }
    private static void ValidateSlug(string name, string argument)
    {
        if (name.Length is < 1 or > 64 ||
            !name.All(c => c is >= 'a' and <= 'z' or >= '0' and <= '9' or '-'))
            throw new ArgumentException("Game ID must be lowercase letters, digits or hyphens.", argument);
    }
    private static void ValidateModule(string module)
    {
        if (module.Length is < 1 or > 128 ||
            !module.All(c => c is >= 'A' and <= 'Z' or >= 'a' and <= 'z'
                or >= '0' and <= '9' or '.' or '_' or '-'))
            throw new ArgumentException("Module must be a filename, not a source path.");
    }
    private static void ValidateRelative(string? value, string field)
    {
        if (value is null) return;
        if (value.Length is < 1 or > 512 || value.StartsWith('/') ||
            value.StartsWith('\\') || value.Contains('\\') || value.Contains(':') ||
            value.Split('/').Any(p => p is "" or "." or "..") ||
            value.Any(char.IsControl))
            throw new ArgumentException("Provenance path must be a relative archive/internal path without traversal.", field);
    }
}
