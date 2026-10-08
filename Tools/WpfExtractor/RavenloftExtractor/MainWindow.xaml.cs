using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text.Json;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;

namespace RavenloftExtractor;

public partial class MainWindow : Window
{
    private Process? _backend;
    private bool _cancelRequested;
    private readonly List<CheckBox> _archiveChecks = [];
    private readonly Dictionary<string, StackPanel> _pages;
    private string _activePage = "source";
    private bool _ready;

    [DllImport("dwmapi.dll", PreserveSig = true)]
    private static extern int DwmSetWindowAttribute(
        IntPtr hwnd, int attribute, ref int value, int size);

    public MainWindow()
    {
        InitializeComponent();
        _pages = new()
        {
            ["source"] = PageSource,
            ["graphics"] = PageGraphics,
            ["audio"] = PageAudio,
            ["cinematics"] = PageCinematics,
            ["support"] = PageSupport,
            ["all"] = PageAll
        };
        DestinationPath.Text = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
            "RLOFT_DUMP"
        );

        foreach (var index in new[] { 0, 1, 2, 3, 4, 7, 8, 9, 11, 12 })
        {
            var checkbox = new CheckBox
            {
                Content = $"RES{index}",
                Tag = index,
                Margin = new Thickness(0, 0, 17, 9),
                IsChecked = true
            };
            _archiveChecks.Add(checkbox);
            GraphicsArchives.Children.Add(checkbox);
        }

        _ready = true;
        Navigate("source");
        SourcePath.TextChanged += (_, _) => UpdateSourceIndicator();

        SourceInitialized += (_, _) =>
        {
            // Only color the operating system's real title bar. Do NOT remove
            // native borders, caption, system resize grips or hit testing.
            int dark = 1;
            _ = DwmSetWindowAttribute(
                new WindowInteropHelper(this).Handle, 20, ref dark, sizeof(int));
        };
    }

    private void Navigation_Checked(object sender, RoutedEventArgs e)
    {
        if (!_ready || sender is not RadioButton radio || radio.Tag is not string page)
            return;
        Navigate(page);
    }

    private void Navigate(string page)
    {
        _activePage = page;
        foreach (var (id, panel) in _pages)
            panel.Visibility = id == page ? Visibility.Visible : Visibility.Collapsed;

        (PageTitle.Text, PageSubtitle.Text, RunButton.Content) = page switch
        {
            "graphics" => ("Graphics & textures",
                "Original names, correct palette routing and semantic frame sets",
                "Extract graphics"),
            "audio" => ("Audio & music",
                "Faithful audio assets and lossless conversion", "Extract audio"),
            "cinematics" => ("Cinematics",
                "Original frame timing and optional video reconstruction", "Extract cinematics"),
            "support" => ("Support assets",
                "Inspect palettes, game text, INIT and renderer tables", "Extract support"),
            "all" => ("Extract everything",
                "One complete media archive from any recognized installation", "Extract all"),
            _ => ("Choose your game source",
                "Installations, CD images and loose game files", "Scan source")
        };

        bool onSource = page == "source";
        ChangeSourceButton.Visibility = onSource ? Visibility.Collapsed : Visibility.Visible;
        ActiveSource.Visibility = onSource ? Visibility.Collapsed : Visibility.Visible;
        UpdateSourceIndicator();
        SettingsScroller.ScrollToTop();
    }

    private void GoToSource_Click(object sender, RoutedEventArgs e)
    {
        NavSource.IsChecked = true;
    }

    private void UpdateSourceIndicator()
    {
        if (!_ready) return;
        var path = SourcePath.Text.Trim();
        ActiveSource.Text = path.Length == 0 ? "SOURCE · None selected" : $"SOURCE · {path}";
        ActiveSource.ToolTip = path;
    }

    private static bool IsChecked(CheckBox box) => box.IsChecked == true;
    private static string Selected(ComboBox box) =>
        (box.SelectedItem as ComboBoxItem)?.Content?.ToString() ?? "";

    private void AutomaticPalette_Changed(object sender, RoutedEventArgs e)
    {
        if (ManualPalette is not null && AutomaticPalette is not null)
            ManualPalette.IsEnabled = AutomaticPalette.IsChecked != true;
    }

    private void BrowseSourceFolder_Click(object sender, RoutedEventArgs e)
    {
        var dlg = new OpenFolderDialog { Title = "Select a Ravenloft game installation" };
        if (dlg.ShowDialog(this) == true)
        {
            SourcePath.Text = dlg.FolderName;
            ValidationSummary.Text = "Folder selected. Run Scan & validate.";
        }
    }

    private void BrowseSourceImage_Click(object sender, RoutedEventArgs e)
    {
        var dlg = new OpenFileDialog
        {
            Title = "Select game.gog or original CD media image",
            Filter = "CD images (*.gog;*.iso;*.bin;*.img)|*.gog;*.iso;*.bin;*.img|All files (*.*)|*.*"
        };
        if (dlg.ShowDialog(this) == true)
        {
            SourcePath.Text = dlg.FileName;
            ValidationSummary.Text = "Image selected. Run Scan & validate.";
        }
    }

    private void BrowseDestination_Click(object sender, RoutedEventArgs e)
    {
        var dlg = new OpenFolderDialog { Title = "Select extraction destination" };
        if (dlg.ShowDialog(this) == true)
            DestinationPath.Text = dlg.FolderName;
    }

    private async void Validate_Click(object sender, RoutedEventArgs e)
    {
        await RunAsync("validate");
    }

    private async void Run_Click(object sender, RoutedEventArgs e)
    {
        await RunAsync(_activePage == "source" ? "validate" : _activePage);
    }

    private void Cancel_Click(object sender, RoutedEventArgs e)
    {
        if (_backend is not { HasExited: false })
            return;
        _cancelRequested = true;
        StatusText.Text = "Cancelling operation…";
        try
        {
            _backend.Kill(entireProcessTree: true);
        }
        catch (InvalidOperationException) { }
        catch (Exception ex)
        {
            StatusDetail.Text = ex.Message;
        }
    }

    private Dictionary<string, object?> BuildRequest(string action)
    {
        var source = SourcePath.Text.Trim();
        if (string.IsNullOrWhiteSpace(source) || !File.Exists(source) && !Directory.Exists(source))
            throw new InvalidOperationException("Choose an existing game directory or CD image.");

        var destination = DestinationPath.Text.Trim();
        if (action != "validate" && string.IsNullOrWhiteSpace(destination))
            throw new InvalidOperationException("Choose a destination folder first.");

        int? palette = null;
        if ((action == "graphics" || action == "all") && AutomaticPalette.IsChecked != true)
        {
            if (!int.TryParse(ManualPalette.Text.Trim(), out var index) || index is < 0 or > 14)
                throw new InvalidOperationException("Manual palette bank must be 0–14.");
            palette = index;
        }

        var names = CinematicNames.Text
            .Split([',', ';', ' ', '\r', '\n', '\t'], StringSplitOptions.RemoveEmptyEntries)
            .Select(s => s.ToUpperInvariant()).ToArray();

        if (action is "cinematics" or "all")
        {
            foreach (var name in names)
                if (name.Length != 10 || !name.StartsWith("CINE", StringComparison.Ordinal) ||
                    !name.EndsWith(".ANM", StringComparison.Ordinal) ||
                    !int.TryParse(name.Substring(4, 2), out int n) || n < 0 || n > 12)
                    throw new InvalidOperationException($"Invalid movie name: {name}. Use CINE00.ANM–CINE12.ANM.");
        }

        double? fps = null;
        if ((action == "all" || action == "cinematics") && !string.IsNullOrWhiteSpace(CinematicFps.Text))
        {
            if (!double.TryParse(CinematicFps.Text, out var parsed) ||
                double.IsNaN(parsed) || double.IsInfinity(parsed) || parsed <= 0 || parsed > 240)
                throw new InvalidOperationException("Custom cinematic FPS must be between 0 and 240.");
            fps = parsed;
        }

        var families = new List<string>();
        if (IsChecked(GroupNpcs)) families.Add("npcs");
        if (IsChecked(GroupObjects)) families.Add("objects");
        if (IsChecked(GroupEffects)) families.Add("effects");
        if (IsChecked(GroupScenes)) families.Add("scene_descriptors");
        var archives = _archiveChecks.Where(IsChecked).Select(c => (int)c.Tag).ToArray();

        if (action is "graphics" or "all")
        {
            if (archives.Length == 0)
                throw new InvalidOperationException("Select at least one RES graphics archive.");
            if (Selected(GraphicsOrganization) != "Raw only" && families.Count == 0)
                throw new InvalidOperationException("Select at least one multipart family, or switch to Raw only.");
        }

        return new Dictionary<string, object?>
        {
            ["action"] = action,
            ["source"] = source,
            ["destination"] = action == "all" || action == "validate"
                ? destination : Path.Combine(destination, action),
            ["graphics_format"] = Selected(GraphicsFormat),
            ["graphics_organization"] = Selected(GraphicsOrganization),
            ["graphics_archives"] = archives,
            ["palette_index"] = palette,
            ["group_families"] = families,
            ["graphics_indices"] = IsChecked(GraphicsIndices),
            ["graphics_metadata"] = IsChecked(GraphicsMetadata),
            ["graphics_native"] = IsChecked(GraphicsNative),
            ["graphics_masks"] = IsChecked(GraphicsMasks),
            ["group_contact_sheets"] = IsChecked(GroupSheets),
            ["group_previews"] = IsChecked(GroupPreviews),
            ["group_alias_maps"] = IsChecked(GroupAliases),
            ["sampled_audio"] = IsChecked(ExtractAudio),
            ["audio_format"] = Selected(AudioFormat),
            ["music"] = IsChecked(ExtractMusic),
            ["music_format"] = Selected(MusicFormat),
            ["preserve_native"] = IsChecked(AudioPreserve),
            ["cinematic_preserve_native"] = IsChecked(CinematicOriginals),
            ["cinematic_names"] = names,
            ["cinematic_frames"] = IsChecked(CinematicFrames),
            ["cinematic_videos"] = IsChecked(CinematicVideos),
            ["cinematic_timeline"] = IsChecked(CinematicTimelines),
            ["cinematic_frame_format"] = Selected(CinematicFrameFormat),
            ["cinematic_video_format"] = Selected(CinematicVideoFormat),
            ["cinematic_sync_audio"] = IsChecked(CinematicSync),
            ["cinematic_fps"] = fps,
            ["export_support"] = IsChecked(AllSupport),
            ["support_colors"] = IsChecked(SupportColors),
            ["support_text"] = IsChecked(SupportText),
            ["support_sample"] = IsChecked(SupportSample),
            ["support_banks"] = IsChecked(SupportBanks),
            ["support_init"] = IsChecked(SupportInit),
            ["support_sintab"] = IsChecked(SupportSintab)
        };
    }

    private void HandleEvent(string line)
    {
        try
        {
            using var document = JsonDocument.Parse(line);
            var ev = document.RootElement;
            var type = ev.GetProperty("type").GetString();
            if (type == "progress")
            {
                var message = ev.GetProperty("message").GetString() ?? "Working…";
                var stage = ev.GetProperty("stage").GetString() ?? "extract";
                int done = ev.GetProperty("done").GetInt32();
                int total = ev.GetProperty("total").GetInt32();
                WorkProgress.IsIndeterminate = total < 1;
                WorkProgress.Value = total <= 0 ? 0 : Math.Clamp(
                    100.0 * done / total, 0, 100);
                StatusText.Text = $"{stage}: {message}";
                StatusDetail.Text = total > 0 ? $"{done} / {total}" : "Working…";
            }
            else if (type == "error")
            {
                var message = ev.GetProperty("message").GetString() ?? "Unknown error";
                StatusText.Text = $"Extraction error: {message}";
                StatusDetail.Text = "See game source, format selection, and output permissions.";
            }
            else if (type == "done")
            {
                StatusText.Text = "Completed successfully.";
                StatusDetail.Text = "The original files remain unchanged.";
                WorkProgress.IsIndeterminate = false;
                WorkProgress.Value = 100;

                if (ev.TryGetProperty("result", out var result) &&
                    result.ValueKind == JsonValueKind.Object &&
                    result.TryGetProperty("expected", out var expected))
                {
                    int total = expected.GetInt32();
                    int found = result.GetProperty("present").GetArrayLength();
                    int missing = result.GetProperty("missing").GetArrayLength();
                    int changed = result.GetProperty("mismatched").GetArrayLength();
                    var kind = result.GetProperty("kind").GetString() ?? "Game files";
                    bool certified = result.GetProperty("certified_complete").GetBoolean();
                    ValidationSummary.Text = certified
                        ? $"{kind}: all {total} audited resources match."
                        : $"{kind}: {found}/{total} recognized; {missing} missing; {changed} different from the reference. Alternate releases are accepted.";
                }
            }
        }
        catch (JsonException)
        {
            StatusDetail.Text = "Backend emitted an unrecognized progress response.";
        }
    }

    private async Task RunAsync(string action)
    {
        if (_backend is { HasExited: false })
            return;

        Dictionary<string, object?> request;
        try
        {
            request = BuildRequest(action);
        }
        catch (Exception ex)
        {
            StatusText.Text = ex.Message;
            return;
        }

        string executable = Path.Combine(AppContext.BaseDirectory, "RavenloftExtractorBackend.exe");
        if (!File.Exists(executable))
        {
            StatusText.Text = "Missing RavenloftExtractorBackend.exe.";
            StatusDetail.Text = "Extract the complete distribution folder; do not move the frontend executable alone.";
            return;
        }

        WorkProgress.Value = 0;
        WorkProgress.IsIndeterminate = true;
        StatusText.Text = action == "validate" ? "Scanning game source…" : $"Extracting {action}…";
        StatusDetail.Text = "Reading and preserving game resources.";
        _cancelRequested = false;
        RunButton.IsEnabled = false;
        CancelButton.IsEnabled = true;

        try
        {
            using var process = new Process();
            process.StartInfo = new ProcessStartInfo(executable)
            {
                UseShellExecute = false,
                CreateNoWindow = true,
                RedirectStandardInput = true,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                WorkingDirectory = AppContext.BaseDirectory
            };
            _backend = process;
            if (!process.Start())
                throw new InvalidOperationException("The extraction engine did not start.");

            var errors = process.StandardError.ReadToEndAsync();
            await process.StandardInput.WriteAsync(JsonSerializer.Serialize(request));
            process.StandardInput.Close();
            string? line;
            while ((line = await process.StandardOutput.ReadLineAsync()) != null)
                HandleEvent(line);
            await process.WaitForExitAsync();
            var errorText = await errors;
            if (_cancelRequested)
            {
                StatusText.Text = "Operation cancelled.";
                StatusDetail.Text = "Any completed files remain in the output folder.";
            }
            else if (process.ExitCode != 0)
            {
                if (!StatusText.Text.StartsWith("Extraction error:", StringComparison.Ordinal))
                    StatusText.Text = $"Extraction engine stopped (exit {process.ExitCode}).";
                StatusDetail.Text = string.IsNullOrWhiteSpace(errorText)
                    ? "Review the source files and selected output."
                    : errorText.Length > 220 ? errorText[^220..] : errorText;
            }
        }
        catch (Exception ex)
        {
            StatusText.Text = "Extraction failed: " + ex.Message;
        }
        finally
        {
            _backend = null;
            RunButton.IsEnabled = true;
            CancelButton.IsEnabled = false;
            WorkProgress.IsIndeterminate = false;
        }
    }
}
