using System.ComponentModel;
using System.Globalization;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using DosDis.Core;
using DosDis.Persistence;
using Microsoft.Win32;

namespace DosDis.Desktop;

public partial class MainWindow : Window
{
    private BinaryArtifact? _artifact;
    private LeImage? _image;
    private FlowAnalysisResult? _graph;
    private ProjectStore? _project;
    private IReadOnlyList<AssemblyRow> _rows = Array.Empty<AssemblyRow>();
    private bool _working;
    private long _commentRevision;

    public MainWindow()
    {
        InitializeComponent();
        AnalyzeButton.IsEnabled = false;
        SaveProjectButton.IsEnabled = false;
        SaveNoteButton.IsEnabled = false;
    }

    private sealed record AssemblyRow(int Object, uint Offset, string Bytes,
        string Assembly, string Evidence)
    {
        public string AddressText => $"{Object}:0x{Offset:X8}";
    }
    private sealed record FunctionRow(FlowAddress Address, string DisplayName);
    private sealed record ReferenceRow(string Direction, string AddressText, string Kind, string Evidence);
    private sealed record IntelligenceRow(string Name, string DisplayValue, string Evidence);

    private void SetWorking(bool value, string? status = null)
    {
        _working = value;
        OpenBinaryButton.IsEnabled = !value;
        OpenProjectButton.IsEnabled = !value;
        AnalyzeButton.IsEnabled = !value && _image != null;
        SaveProjectButton.IsEnabled = !value && _artifact != null;
        SaveNoteButton.IsEnabled = !value && _artifact != null && _project != null && AssemblyGrid.SelectedItem is AssemblyRow;
        AssemblyGrid.IsEnabled = !value;
        FunctionsList.IsEnabled = !value;
        if (status != null) StatusText.Text = status;
    }
    private void Error(string context, Exception error)
    {
        StatusText.Text = $"{context}: {error.Message}";
        MessageBox.Show(this, $"{context}\n\n{error.Message}", "DOSDis", MessageBoxButton.OK, MessageBoxImage.Error);
    }

    private async void OpenBinary_Click(object sender, RoutedEventArgs e)
    {
        var picker = new OpenFileDialog
        {
            Title = "Open original DOS executable (read-only)",
            Filter = "All files (*.*)|*.*|Executables (*.exe;*.com)|*.exe;*.com",
            CheckFileExists = true
        };
        if (picker.ShowDialog(this) != true) return;
        SetWorking(true, "Reading and identifying original bytes…");
        try
        {
            var result = await Task.Run(() =>
            {
                var artifact = BinaryArtifact.Open(picker.FileName);
                LeImage? image;
                try { image = LeImage.Load(artifact); }
                catch (InvalidDataException) { image = null; }
                catch (NotSupportedException) { image = null; }
                var intelligence = FileIntelligence.Inspect(artifact, image);
                IReadOnlyList<DecodedLine> preview = Array.Empty<DecodedLine>();
                if (image != null)
                {
                    try { preview = Disassembly.Decode(image, (int)image.EntryObject, image.EntryOffset, 128); }
                    catch (InvalidDataException) { /* Entry not stored in the original file. */ }
                }
                return (artifact, image, intelligence, preview);
            });
            _artifact = result.artifact;
            _image = result.image;
            _graph = null;
            if (_project != null && !_project.ListArtifacts().Any(a => a.Sha256 == _artifact.Sha256))
            {
                _project.Dispose();
                _project = null;
            }
            BinaryName.Text = _artifact.Name;
            ArtifactHash.Text = $"SHA-256 {_artifact.Sha256}";
            FileSubtitle.Text = _image is null
                ? $"{_artifact.Length:N0} bytes · Unsupported/unknown format"
                : $"{_artifact.Length:N0} bytes · LE · {_image.Objects.Count} objects · {_image.Relocations.Count:N0} fixups";
            IntelligenceList.ItemsSource = result.intelligence.Select(x => new IntelligenceRow(x.Key,
                $"{x.Value.Value} ({x.Value.Status})", x.Value.Evidence)).ToArray();
            FunctionsList.ItemsSource = Array.Empty<FunctionRow>();
            AddressBox.Text = _image is null ? "1:0x0" : $"{_image.EntryObject}:0x{_image.EntryOffset:X}";
            SetRows(result.preview.Select(x => new AssemblyRow(_image is null ? 1 : (int)_image.EntryObject,
                x.ObjectOffset, x.Bytes, x.Assembly, "linear-preview-unverified")).ToArray());
            if (_project != null)
            {
                _graph = _project.LoadLatestAnalysis(_artifact.Sha256);
                if (_graph != null) ShowAnalysis(_graph);
            }
            StatusText.Text = _image is null ? "File identified where possible; this executable format is not yet supported."
                : _graph is not null ? "Restored saved candidate analysis. Original file hash verified."
                : "Showing an UNVERIFIED linear preview. Run Analyze for evidence-tagged reachable code.";
        }
        catch (Exception ex) { Error("Unable to open file", ex); }
        finally { SetWorking(false); }
    }

    private async void Analyze_Click(object sender, RoutedEventArgs e)
    {
        if (_image is null || _artifact is null || _working) return;
        SetWorking(true, "Analyzing direct and relocation-backed control flow…");
        try
        {
            LeImage image = _image;
            var graph = await Task.Run(() => ControlFlowAnalysis.Analyze(image, maxInstructions: 100_000));
            _graph = graph;
            ShowAnalysis(graph);
            if (_project != null)
            {
                ProjectStore project = _project;
                await Task.Run(() => project.SaveAnalysis(graph));
            }
            StatusText.Text = $"{graph.Instructions.Count:N0} candidate instructions; {graph.FunctionCandidates.Count:N0} function candidates; " +
                $"{graph.JumpTables.Count:N0} table candidates. " +
                (graph.Truncated ? "TRUNCATED by instruction budget." : "Remaining bytes are not automatically code.");
        }
        catch (Exception ex) { Error("Analysis failed", ex); }
        finally { SetWorking(false); }
    }

    private void ShowAnalysis(FlowAnalysisResult graph)
    {
        SetRows(graph.Instructions.Select(x => new AssemblyRow(x.Address.Object, x.Address.Offset,
            x.OriginalBytes, x.Assembly, x.Evidence)).ToArray());
        FunctionsList.ItemsSource = graph.FunctionCandidates.Select(f => new FunctionRow(f.Entry,
            $"{f.Entry.Object}:0x{f.Entry.Offset:X8}  [{f.Blocks.Count} blocks]")).ToArray();
    }
    private void SetRows(IReadOnlyList<AssemblyRow> items)
    {
        _rows = items;
        AssemblyGrid.ItemsSource = items;
        ReferencesGrid.ItemsSource = null;
        NoteEditor.Text = string.Empty;
        SelectedAddressText.Text = "Select an instruction";
        HexView.Text = string.Empty;
    }

    private void FunctionsList_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (FunctionsList.SelectedItem is FunctionRow row) Navigate(row.Address);
    }
    private void AssemblyGrid_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (AssemblyGrid.SelectedItem is not AssemblyRow row || _artifact is null) return;
        var address = new FlowAddress(row.Object, row.Offset);
        SelectedAddressText.Text = row.AddressText;
        AddressBox.Text = $"{row.Object}:0x{row.Offset:X}";
        var refs = new List<ReferenceRow>();
        if (_graph != null)
        {
            refs.AddRange(_graph.Edges.Where(x => x.Source == address).Select(x =>
                new ReferenceRow("Outgoing", Format(x.Target), x.Kind, x.Evidence)));
            refs.AddRange(_graph.Edges.Where(x => x.Destination == address).Select(x =>
                new ReferenceRow("Incoming", Format(x.Source), x.Kind, x.Evidence)));
            refs.AddRange(_graph.DataReferences.Where(x => x.Source == address).Select(x =>
                new ReferenceRow("Data", Format(x.Target), x.Kind, x.Evidence)));
        }
        ReferencesGrid.ItemsSource = refs;
        RenderHex(address);
        _commentRevision = 0;
        NoteEditor.Text = string.Empty;
        if (_project != null)
        {
            var note = _project.GetAnnotation(_artifact.Sha256, row.Object, row.Offset, "comment");
            if (note != null) { NoteEditor.Text = note.Body; _commentRevision = note.Revision; }
        }
        SaveNoteButton.IsEnabled = !_working && _project != null;
    }
    private static string Format(FlowAddress? at) =>
        at is { } a ? $"{a.Object}:0x{a.Offset:X8}" : "unresolved";

    private void RenderHex(FlowAddress address)
    {
        if (_image is null || address.Object < 1 || address.Object > _image.Objects.Count) return;
        uint virtualSize = _image.Objects[address.Object - 1].VirtualSize;
        uint start = address.Offset & ~15u;
        var output = new StringBuilder();
        for (uint line = 0; line < 48 && (ulong)start + line * 16u < virtualSize; ++line)
        {
            uint offset = start + line * 16u;
            output.Append($"{address.Object}:{offset:X8}   ");
            var ascii = new StringBuilder();
            for (int j = 0; j < 16; ++j)
            {
                uint p = offset + (uint)j;
                if (p >= virtualSize || _image.ToFileOffset(address.Object, p) is null)
                { output.Append("?? "); ascii.Append(' '); continue; }
                byte value = _image.ReadStoredBytes(address.Object, p, 1)[0];
                output.Append($"{value:X2} ");
                ascii.Append(value is >= 32 and < 127 ? (char)value : '.');
            }
            output.Append("  ").AppendLine(ascii.ToString());
        }
        HexView.Text = output.ToString();
    }

    private void GoTo_Click(object sender, RoutedEventArgs e)
    {
        if (_image is null) return;
        try
        {
            string[] parts = AddressBox.Text.Trim().Split(':', 2);
            if (parts.Length != 2) throw new FormatException("Expected object:0xOFFSET, for example 1:0x2975C.");
            int number = int.Parse(parts[0], CultureInfo.InvariantCulture);
            string raw = parts[1].StartsWith("0x", StringComparison.OrdinalIgnoreCase) ? parts[1][2..] : parts[1];
            uint offset = uint.Parse(raw, NumberStyles.HexNumber, CultureInfo.InvariantCulture);
            Navigate(new FlowAddress(number, offset));
        }
        catch (Exception ex) when (ex is FormatException or ArgumentException or OverflowException)
        { Error("Invalid address", ex); }
    }
    private void Navigate(FlowAddress address)
    {
        if (_image is null) return;
        AssemblyRow? found = _rows.FirstOrDefault(x => x.Object == address.Object && x.Offset == address.Offset);
        if (found is null)
        {
            try
            {
                var lines = Disassembly.Decode(_image, address.Object, address.Offset, 64);
                SetRows(lines.Select(x => new AssemblyRow(address.Object, x.ObjectOffset, x.Bytes,
                    x.Assembly, "linear-preview-unverified")).ToArray());
                StatusText.Text = "Temporary linear preview; not automatically classified as executable code.";
                found = _rows.FirstOrDefault();
            }
            catch (Exception ex) when (ex is InvalidDataException or ArgumentException or NotSupportedException)
            { Error("Cannot navigate to address", ex); return; }
        }
        if (found is null) return;
        MainTabs.SelectedIndex = 0;
        AssemblyGrid.SelectedItem = found;
        AssemblyGrid.ScrollIntoView(found);
    }

    private void SaveProject_Click(object sender, RoutedEventArgs e)
    {
        if (_artifact is null || _working) return;
        try
        {
            if (_project is null)
            {
                var picker = new SaveFileDialog
                {
                    Title = "Create DOSDis research project", DefaultExt = ".ddproj",
                    Filter = "DOSDis project (*.ddproj)|*.ddproj", AddExtension = true
                };
                if (picker.ShowDialog(this) != true) return;
                _project = ProjectStore.Open(picker.FileName);
            }
            _project.EnsureArtifact(_artifact);
            if (_graph != null) _project.SaveAnalysis(_graph);
            StatusText.Text = $"Project saved: {_project.FilePath}. No complete original executable was embedded; instruction bytes remain in cached findings.";
            SaveNoteButton.IsEnabled = AssemblyGrid.SelectedItem is AssemblyRow;
        }
        catch (Exception ex) { Error("Project save failed", ex); }
    }
    private void OpenProject_Click(object sender, RoutedEventArgs e)
    {
        if (_artifact is null)
        {
            MessageBox.Show(this,"First open the original binary so its SHA-256 can be checked against the project.",
                "Original binary required",MessageBoxButton.OK,MessageBoxImage.Information);
            return;
        }
        var picker = new OpenFileDialog
        {
            Title = "Open DOSDis project", Filter = "DOSDis project (*.ddproj)|*.ddproj", CheckFileExists = true
        };
        if (picker.ShowDialog(this) != true) return;
        try
        {
            var next = ProjectStore.Open(picker.FileName);
            try
            {
                next.VerifyIntegrity();
                if (!next.ListArtifacts().Any(a => a.Sha256 == _artifact.Sha256 && a.Length == _artifact.Length))
                    throw new InvalidDataException("This project does not contain the matching original binary hash.");
                FlowAnalysisResult? saved = next.LoadLatestAnalysis(_artifact.Sha256);
                _project?.Dispose();
                _project = next;
                _graph = saved;
                if (saved != null) ShowAnalysis(saved);
                StatusText.Text = saved is null ? "Project opened; no analysis snapshot saved yet."
                    : $"Project restored: {saved.Instructions.Count:N0} candidate instructions.";
            }
            catch { next.Dispose(); throw; }
        }
        catch (Exception ex) { Error("Project open failed", ex); }
    }
    private void SaveNote_Click(object sender, RoutedEventArgs e)
    {
        if (_working || _artifact is null || _project is null || AssemblyGrid.SelectedItem is not AssemblyRow row) return;
        try
        {
            var note = _project.SaveAnnotation(_artifact.Sha256, row.Object, row.Offset,
                "comment", NoteEditor.Text, _commentRevision);
            _commentRevision = note.Revision;
            StatusText.Text = $"Comment saved for {row.AddressText}, revision {note.Revision}. Original file untouched.";
        }
        catch (Exception ex) { Error("Annotation was not saved", ex); }
    }
    private void Window_Closing(object? sender, CancelEventArgs e) => _project?.Dispose();
}
