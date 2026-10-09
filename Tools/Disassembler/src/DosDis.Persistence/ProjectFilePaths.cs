namespace DosDis.Persistence;

/// <summary>
/// Human-readable Keep Both naming for user-owned research projects.
/// Never preemptively deletes or overwrites any destination.
/// </summary>
public static class ProjectFilePaths
{
    public static string NextAvailable(string selectedPath)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(selectedPath);
        string selected = Path.GetFullPath(selectedPath);
        if (!File.Exists(selected) && !Directory.Exists(selected))
            return selected;

        string directory = Path.GetDirectoryName(selected)!;
        string name = Path.GetFileNameWithoutExtension(selected);
        string extension = Path.GetExtension(selected);
        for (int number = 1; number <= 10_000; number++)
        {
            string candidate = Path.Combine(directory,
                name + " (" + number + ")" + extension);
            if (!File.Exists(candidate) && !Directory.Exists(candidate))
                return candidate;
        }
        throw new IOException("No available numbered file name was found.");
    }

    /// <summary>
    /// Replacing a project with live SQLite WAL sidecars risks losing
    /// research transactions. Explicit replacement is forbidden in this case.
    /// </summary>
    public static void RequireSafeReplacement(string path)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);
        string resolved = Path.GetFullPath(path);
        if (File.Exists(resolved + "-wal") || File.Exists(resolved + "-shm"))
            throw new IOException("This project has active SQLite journal files. " +
                "Close the other project first, or choose Keep both.");
    }
}
