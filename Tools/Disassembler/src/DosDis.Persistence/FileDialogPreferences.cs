using System.Text.Json;

namespace DosDis.Persistence;

/// <summary>
/// Stores only the last source-file dialog folder on the local machine.
/// No original executable bytes, instruction data or comments are stored.
/// Corrupt, inaccessible, or stale settings never prevent a file dialog.
/// </summary>
public sealed class FileDialogPreferences
{
    private sealed record SavedSettings(string LastBinaryDirectory);
    private readonly string _path;

    public FileDialogPreferences(string? settingsPath = null)
    {
        _path = Path.GetFullPath(settingsPath ??
            Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                "DOSDis", "desktop-preferences.json"));
    }

    public string? LastBinaryDirectory
    {
        get
        {
            try
            {
                if (!File.Exists(_path)) return null;
                var settings = JsonSerializer.Deserialize<SavedSettings>(
                    File.ReadAllText(_path));
                var directory = settings?.LastBinaryDirectory;
                return directory is not null && Directory.Exists(directory)
                    ? directory : null;
            }
            catch (Exception ex) when (ex is IOException or UnauthorizedAccessException
                or JsonException or ArgumentException or NotSupportedException)
            {
                return null;
            }
        }
    }

    public void RememberBinary(string filename)
    {
        string? temporary = null;
        try
        {
            string? directory = Path.GetDirectoryName(Path.GetFullPath(filename));
            if (directory is null || !Directory.Exists(directory)) return;
            string parent = Path.GetDirectoryName(_path)!;
            Directory.CreateDirectory(parent);
            temporary = _path + "." + Guid.NewGuid().ToString("N") + ".tmp";
            File.WriteAllText(temporary,
                JsonSerializer.Serialize(new SavedSettings(directory)));
            File.Move(temporary, _path, overwrite: true);
        }
        catch (Exception ex) when (ex is IOException or UnauthorizedAccessException
            or ArgumentException or NotSupportedException)
        {
            // Preference saving is best-effort; never discard loaded binaries.
        }
        finally
        {
            if (temporary is not null)
            {
                try { if (File.Exists(temporary)) File.Delete(temporary); }
                catch (IOException) { }
                catch (UnauthorizedAccessException) { }
            }
        }
    }
}
