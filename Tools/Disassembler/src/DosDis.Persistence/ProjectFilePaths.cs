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
    /// Atomically publishes a completed export, preserving an old report if
    /// the writer fails. File replacement must already have been confirmed by
    /// the user in the desktop Save As dialog.
    /// </summary>
    public static void PublishNewOrReplace(string selectedPath, bool overwrite,
        Action<string> writeNew)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(selectedPath);
        ArgumentNullException.ThrowIfNull(writeNew);
        string destination = Path.GetFullPath(selectedPath);
        string? parent = Path.GetDirectoryName(destination);
        if (parent is null || !Directory.Exists(parent))
            throw new DirectoryNotFoundException("Export destination directory does not exist.");
        string temporary = Path.Combine(parent, "." +
            Path.GetFileName(destination) + "." + Guid.NewGuid().ToString("N") + ".part");
        try
        {
            writeNew(temporary);
            if (!File.Exists(temporary) || new FileInfo(temporary).Length == 0)
                throw new InvalidDataException("Export did not produce a complete output file.");
            if (overwrite && File.Exists(destination))
            {
                RequireSafeReplacement(destination);
                File.Replace(temporary, destination, null);
            }
            else
                File.Move(temporary, destination, overwrite: false);
        }
        finally
        {
            try { if (File.Exists(temporary)) File.Delete(temporary); }
            catch (IOException) { }
            catch (UnauthorizedAccessException) { }
        }
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
