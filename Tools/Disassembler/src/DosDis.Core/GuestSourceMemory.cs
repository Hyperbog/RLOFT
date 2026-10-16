namespace DosDis.Core;

/// <summary>
/// Immutable source-backed interval for a named CPU bus and selected mapping
/// view. Two views may expose different bytes at the same CPU address.
/// </summary>
public sealed record GuestSourceWindow(
    string AddressSpace, string ViewId, uint GuestStart, uint ByteCount, long PhysicalStart);

public sealed record GuestSourceFragment(uint GuestStart, int ByteCount, long PhysicalStart);

public sealed record GuestSourceRead(byte[] Bytes, IReadOnlyList<GuestSourceFragment> Fragments);

/// <summary>
/// Format-neutral original-file mapping. This is neither a CPU decoder nor a
/// model of runtime RAM, I/O or self-modifying program state.
/// </summary>
public sealed class GuestSourceMemory
{
    private const int MaxWindows = 16_384;
    private const int MaxReadBytes = 4_096;
    private readonly BinaryArtifact _artifact;
    private readonly Dictionary<(string Space, string View), GuestSourceWindow[]> _windows;

    public string SourceSha256 => _artifact.Sha256;

    public GuestSourceMemory(BinaryArtifact artifact, IEnumerable<GuestSourceWindow> windows)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        ArgumentNullException.ThrowIfNull(windows);
        _artifact = artifact;
        GuestSourceWindow[] all = windows.ToArray();
        if (all.Length > MaxWindows)
            throw new InvalidDataException("Too many memory mapping intervals.");
        foreach (GuestSourceWindow window in all)
        {
            if (window is null)
                throw new InvalidDataException("Null memory mapping interval.");
            CheckId(window.AddressSpace);
            CheckId(window.ViewId);
            if (window.ByteCount == 0 ||
                (ulong)window.GuestStart + window.ByteCount > (ulong)uint.MaxValue + 1 ||
                window.PhysicalStart < 0 ||
                (ulong)window.PhysicalStart + window.ByteCount > (ulong)artifact.Length)
                throw new InvalidDataException("Memory mapping exceeds source or guest address limits.");
        }
        _windows = all.GroupBy(w => (w.AddressSpace, w.ViewId))
            .ToDictionary(g => (g.Key.AddressSpace, g.Key.ViewId), g =>
            {
                GuestSourceWindow[] sorted = g.OrderBy(w => w.GuestStart).ToArray();
                ulong end = 0;
                foreach (GuestSourceWindow w in sorted)
                {
                    if (w.GuestStart < end)
                        throw new InvalidDataException("Overlapping intervals in one selected memory view.");
                    end = (ulong)w.GuestStart + w.ByteCount;
                }
                return sorted;
            });
    }

    /// <summary>
    /// Read exactly source-backed bytes or return null without guessing.
    /// Adjacent guest intervals may map discontiguous source file offsets.
    /// Each independent physical interval is returned for evidence tracking.
    /// </summary>
    public GuestSourceRead? TryReadExact(
        string addressSpace, string viewId, uint guestStart, int count)
    {
        CheckId(addressSpace);
        CheckId(viewId);
        if (count is < 1 or > MaxReadBytes)
            throw new ArgumentOutOfRangeException(nameof(count));
        if ((ulong)guestStart + (uint)count > (ulong)uint.MaxValue + 1 ||
            !_windows.TryGetValue((addressSpace, viewId), out GuestSourceWindow[]? windows))
            return null;

        byte[] copy = new byte[count];
        var fragments = new List<GuestSourceFragment>();
        int written = 0;
        while (written < count)
        {
            uint address = (uint)((ulong)guestStart + (uint)written);
            GuestSourceWindow? window = FindWindow(windows, address);
            if (window is null)
                return null;
            int take = (int)Math.Min((ulong)(count - written),
                (ulong)window.GuestStart + window.ByteCount - address);
            long physical = window.PhysicalStart + (address - window.GuestStart);
            _artifact.Data.Slice((int)physical, take).CopyTo(copy.AsSpan(written));
            fragments.Add(new GuestSourceFragment(address, take, physical));
            written += take;
        }
        return new GuestSourceRead(copy, fragments.AsReadOnly());
    }

    private static GuestSourceWindow? FindWindow(GuestSourceWindow[] windows, uint address)
    {
        int lo = 0, hi = windows.Length - 1;
        GuestSourceWindow? found = null;
        while (lo <= hi)
        {
            int mid = lo + (hi - lo) / 2;
            if (windows[mid].GuestStart <= address)
            {
                found = windows[mid];
                lo = mid + 1;
            }
            else hi = mid - 1;
        }
        return found is not null &&
            (ulong)address < (ulong)found.GuestStart + found.ByteCount
            ? found : null;
    }

    private static void CheckId(string value)
    {
        if (string.IsNullOrEmpty(value) || value.Length > 96 ||
            !value.All(c => c is >= 'a' and <= 'z' or >= 'A' and <= 'Z'
                or >= '0' and <= '9' or '-' or '_' or '.' or ':'))
            throw new InvalidDataException("Address space and memory view require bounded identifiers.");
    }
}
