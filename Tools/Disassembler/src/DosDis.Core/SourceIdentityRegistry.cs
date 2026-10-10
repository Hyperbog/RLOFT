using System.Text.Json;

namespace DosDis.Core;

/// <summary>
/// Optional identity data, supplied separately from the analyzer. An entry
/// identifies exact original bytes; it never changes executable interpretation.
/// </summary>
public sealed record SourceIdentityEntry(
    string Sha256, long ByteLength, string GameId, string Module,
    string SourceEvidence, string? Software = null, string? Version = null,
    string? Developer = null, string? Publisher = null,
    string? OriginalYear = null, string? EngineFamily = null);

public sealed record SourceIdentityDocument(
    int SchemaVersion, IReadOnlyList<SourceIdentityEntry> Entries);

public sealed class SourceIdentityRegistry
{
    public const int SchemaVersion = 1;
    private const int MaximumFileBytes = 1024 * 1024;
    private readonly IReadOnlyDictionary<string, SourceIdentityEntry> _entries;

    private SourceIdentityRegistry(Dictionary<string, SourceIdentityEntry> entries)
    {
        _entries = entries;
    }

    public int Count => _entries.Count;

    /// <summary>No built-in game list; identity registries are opt-in files.</summary>
    public static SourceIdentityRegistry FromJson(string json)
    {
        ArgumentNullException.ThrowIfNull(json);
        if (System.Text.Encoding.UTF8.GetByteCount(json) > MaximumFileBytes)
            throw new InvalidDataException("Identity registry exceeds 1 MiB.");
        SourceIdentityDocument document;
        try
        {
            document = JsonSerializer.Deserialize<SourceIdentityDocument>(json,
                new JsonSerializerOptions(JsonSerializerDefaults.Web))
                ?? throw new InvalidDataException("Identity registry is empty.");
        }
        catch (JsonException ex)
        {
            throw new InvalidDataException("Identity registry JSON is invalid.", ex);
        }
        if (document.SchemaVersion != SchemaVersion ||
            document.Entries is null || document.Entries.Count > 4096)
            throw new InvalidDataException("Unsupported or oversized identity registry.");
        var byHash = new Dictionary<string, SourceIdentityEntry>(StringComparer.OrdinalIgnoreCase);
        foreach (var item in document.Entries)
        {
            if (item is null || !ValidHash(item.Sha256) || item.ByteLength < 1 ||
                !ValidSlug(item.GameId) || !ValidModule(item.Module) ||
                !ValidMetadata(item.SourceEvidence, required: true) ||
                !ValidMetadata(item.Software) || !ValidMetadata(item.Version) ||
                !ValidMetadata(item.Developer) || !ValidMetadata(item.Publisher) ||
                !ValidMetadata(item.OriginalYear) || !ValidMetadata(item.EngineFamily))
                throw new InvalidDataException("Invalid source identity entry.");
            var normalized = item with { Sha256 = item.Sha256.ToLowerInvariant() };
            if (!byHash.TryAdd(normalized.Sha256, normalized))
                throw new InvalidDataException("Duplicate SHA-256 identity claims are ambiguous.");
        }
        return new SourceIdentityRegistry(byHash);
    }

    public static SourceIdentityRegistry ReadFile(string path)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);
        var info = new FileInfo(path);
        if (info.Length > MaximumFileBytes)
            throw new InvalidDataException("Identity registry exceeds 1 MiB.");
        return FromJson(File.ReadAllText(path));
    }

    public SourceIdentityEntry? Find(BinaryArtifact artifact)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        if (!_entries.TryGetValue(artifact.Sha256, out var entry))
            return null;
        if (entry.ByteLength != artifact.Length)
            throw new InvalidDataException("Identity registry SHA-256 has an inconsistent original length.");
        return entry;
    }

    public SourceIdentityEntry? Find(string sha256, long length)
    {
        if (!ValidHash(sha256) || length < 1)
            throw new ArgumentException("Invalid fingerprint query.");
        if (!_entries.TryGetValue(sha256, out var entry)) return null;
        if (entry.ByteLength != length)
            throw new InvalidDataException("Identity registry original length differs.");
        return entry;
    }

    private static bool ValidHash(string? hash) => hash is { Length: 64 } &&
        hash.All(c => c is >= '0' and <= '9' or >= 'a' and <= 'f' or >= 'A' and <= 'F');
    private static bool ValidSlug(string? id) => id is { Length: > 0 and <= 64 } &&
        id.All(c => c is >= 'a' and <= 'z' or >= '0' and <= '9' or '-');
    private static bool ValidModule(string? module) => module is { Length: > 0 and <= 128 } &&
        module.All(c => c is >= 'A' and <= 'Z' or >= 'a' and <= 'z' or
            >= '0' and <= '9' or '.' or '_' or '-');
    private static bool ValidMetadata(string? value, bool required = false) =>
        value is null ? !required :
        value.Length is > 0 and <= 512 &&
        !value.Any(char.IsControl);
}
