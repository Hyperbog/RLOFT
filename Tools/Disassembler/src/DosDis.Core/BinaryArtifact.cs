using System.Security.Cryptography;

namespace DosDis.Core;

/// <summary>A read-only snapshot; analysis never edits the input file.</summary>
public sealed class BinaryArtifact
{
    private const long MaxPrototypeSize = 128L * 1024 * 1024;
    private readonly byte[] _bytes;
    private BinaryArtifact(byte[] bytes, string name)
    {
        _bytes = (byte[])bytes.Clone();
        Name = name;
        Sha256 = Convert.ToHexString(SHA256.HashData(_bytes)).ToLowerInvariant();
    }

    public string Name { get; }
    public string Sha256 { get; }
    public int Length => _bytes.Length;
    public ReadOnlySpan<byte> Data => _bytes;

    public static BinaryArtifact FromBytes(byte[] bytes, string name = "fixture")
    {
        ArgumentNullException.ThrowIfNull(bytes);
        if (bytes.LongLength > MaxPrototypeSize)
            throw new InvalidDataException("File exceeds prototype 128 MiB safety limit.");
        return new BinaryArtifact(bytes, name);
    }

    public static BinaryArtifact Open(string path)
    {
        var info = new FileInfo(path);
        if (info.Length > MaxPrototypeSize)
            throw new InvalidDataException("File exceeds prototype 128 MiB safety limit.");
        return FromBytes(File.ReadAllBytes(path), info.Name);
    }
}
