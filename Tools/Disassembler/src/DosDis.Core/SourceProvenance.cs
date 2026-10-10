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
        string? sourceArchive = null, string? internalPath = null,
        SourceIdentityRegistry? registry = null)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        ValidateHash(artifact.Sha256);
        if (image is not null && !string.Equals(image.ArtifactSha256, artifact.Sha256,
                StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("Provenance LE image belongs to another binary.");

        SourceIdentityEntry? matched = registry?.Find(artifact);
        if (matched is not null && gameId is not null && gameId != matched.GameId)
            throw new InvalidDataException("Game ID conflicts with this external catalog's exact source fingerprint.");
        if (matched is not null && module is not null &&
            !string.Equals(module, matched.Module, StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("Module conflicts with this external catalog's exact source fingerprint.");
        string labelGame = matched?.GameId ?? gameId ?? "unassigned";
        string labelModule = matched?.Module ?? module ?? artifact.Name;
        // A host filename never supplies unique identity; retain unsupported
        // basenames as unassigned rather than refusing to inspect the bytes.
        if (matched is null && module is null &&
            (labelModule.Length > 128 ||
             !labelModule.All(c => c is >= 'A' and <= 'Z' or >= 'a' and <= 'z'
                 or >= '0' and <= '9' or '.' or '_' or '-')))
            labelModule = "unassigned";
        ValidateSlug(labelGame, nameof(gameId));
        ValidateModule(labelModule);
        ValidateRelative(sourceArchive, nameof(sourceArchive));
        ValidateRelative(internalPath, nameof(internalPath));

        return new SourceProvenance(CurrentSchema, labelGame, labelModule,
            artifact.Sha256, artifact.Length, image is null ? "unknown" : "LE",
            matched is not null ? "catalog-matched-analyst-supplied"
                : gameId is null ? "source-hash-only-game-unassigned"
                    : "analyst-declared-game-unverified",
            labelGame + "/" + artifact.Sha256,
            sourceArchive, internalPath);
    }

    /// <summary>Reject forged verified fingerprints and namespace mismatches.</summary>
    public void ValidateClaim()
    {
        ValidateHash(SourceSha256);
        ValidateSlug(GameId, nameof(GameId));
        ValidateModule(Module);
        ValidateRelative(SourceArchive, nameof(SourceArchive));
        ValidateRelative(InternalPath, nameof(InternalPath));
        if (SchemaVersion != CurrentSchema ||
            ResearchNamespace != GameId + "/" + SourceSha256.ToLowerInvariant())
            throw new InvalidDataException("Inconsistent source provenance namespace or schema.");
        if (ByteLength is null)
        {
            if (GameId != "unassigned" || Module != "unassigned" ||
                IdentityStatus != "source-hash-only-game-unassigned")
                throw new InvalidDataException("Unbound graph-only provenance cannot assert game identity.");
            return;
        }
        if (ByteLength < 1)
            throw new InvalidDataException("Source length must be positive.");
        // The identity catalog itself is analyst supplied; even an exact
        // match is not automatic proof of any executable's semantics.
        if (IdentityStatus is not ("catalog-matched-analyst-supplied" or
            "analyst-declared-game-unverified" or "source-hash-only-game-unassigned"))
            throw new InvalidDataException("Unsupported source identity evidence status.");
        if (IdentityStatus == "source-hash-only-game-unassigned" && GameId != "unassigned")
            throw new InvalidDataException("Unassigned identity status cannot claim a named game.");
        if (IdentityStatus != "source-hash-only-game-unassigned" && GameId == "unassigned")
            throw new InvalidDataException("Named identity status must supply a game ID.");
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
