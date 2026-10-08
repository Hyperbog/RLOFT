using System.ComponentModel;
using System.Globalization;
using System.IO;
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
    private IReadOnlyDictionary<FlowAddress, IReadOnlyList<ReferenceRow>> _referenceIndex =
        new Dictionary<FlowAddress, IReadOnlyList<ReferenceRow>>();
    private bool _working;
    private long _commentRevision;
    private CancellationTokenSource? _analysisCancellation;

    public MainWindow()
    {
        InitializeComponent();
        AnalyzeButton.IsEnabled = false;
        SaveProjectButton.IsEnabled = false;
        BackupProjectButton.IsEnabled = false;
        SaveNoteButton.IsEnabled = false;
        CancelAnalysisButton.IsEnabled = false;
        ExportAnalysisButton.IsEnabled = false;
    }

    private sealed record AssemblyRow(int Object, uint Offset, string Bytes,
        string Assembly, string Evidence)
    {
        public string AddressText => $"{Object}:0x{Offset:X8}";
    }
    private sealed record FunctionRow(FlowAddress Address, string DisplayName);
    private sealed record ReferenceRow(string Direction, string AddressText, string Kind,
        string Evidence, FlowAddress? NavigationTarget);
    private sealed record IntelligenceRow(string Name, string DisplayValue, string Evidence);

    private void SetWorking(bool value, string? status = null)
    {
        _working = value;
        OpenBinaryButton.IsEnabled = !value;
        OpenProjectButton.IsEnabled = !value;
        AnalyzeButton.IsEnabled = !value && _image != null;
        SaveProjectButton.IsEnabled = !value && _artifact != null;
        BackupProjectButton.IsEnabled = !value && _project != null;
        SaveNoteButton.IsEnabled = !value && _artifact != null && _project != null && AssemblyGrid.SelectedItem is AssemblyRow;
        AssemblyGrid.IsEnabled = !value;
        FunctionsList.IsEnabled = !value;
        CancelAnalysisButton.IsEnabled = value && _analysisCancellation is not null;
        ExportAnalysisButton.IsEnabled = !value && _graph is not null;
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
            ExportAnalysisButton.IsEnabled = false;
            _referenceIndex = new Dictionary<FlowAddress, IReadOnlyList<ReferenceRow>>();
            if (_project != null && !_project.ListArtifacts().Any(a => a.Sha256 == _artifact.Sha256))
            {
                _project.Dispose();
                _project = null;
                BackupProjectButton.IsEnabled = false;
            }
            BinaryName.Text = _artifact.Name;
            ArtifactHash.Text = $"SHA-256 {_artifact.Sha256}";
            FileSubtitle.Text = _image is null
                ? $"{_artifact.Length:N0} bytes · original raw file · code analysis unavailable"
                : $"{_artifact.Length:N0} bytes · LE · {_image.Objects.Count} objects · {_image.Relocations.Count:N0} fixups";
            ExplorerSectionTitle.Text = _image is null ? "RAW FILE — NO FUNCTION ANALYSIS" : "FUNCTION CANDIDATES";
            IntelligenceList.ItemsSource = result.intelligence.Select(x => new IntelligenceRow(x.Key,
                $"{x.Value.Value} ({x.Value.Status})", x.Value.Evidence)).ToArray();
            FunctionsList.ItemsSource = Array.Empty<FunctionRow>();
            AddressBox.Text = _image is null ? "1:0x0" : $"{_image.EntryObject}:0x{_image.EntryOffset:X}";
            SetRows(result.preview.Select(x => new AssemblyRow(_image is null ? 1 : (int)_image.EntryObject,
                x.ObjectOffset, x.Bytes, x.Assembly, "linear-preview-unverified")).ToArray());
            if (_image is null)
            {
                // Data/resource files are not code. Show exact physical bytes
                // immediately, without inventing a function candidate.
                HexView.Text = RawHexPreview.Render(_artifact);
                AddressBox.Text = "file:0x0";
                MainTabs.SelectedIndex = 1;
            }
            if (_project != null && _image != null)
            {
                _graph = _project.LoadLatestAnalysis(_artifact.Sha256, _image);
                if (_graph != null) ShowAnalysis(_graph);
            }
            StatusText.Text = _image is null
                ? "RAW HEX VIEW ONLY: original file bytes at physical offsets. No LE object mapping or instructions inferred."
                : _graph is not null ? "Restored saved candidate analysis. Original file hash verified."
                : "Showing an UNVERIFIED linear preview. Run Analyze for evidence-tagged reachable code.";
        }
        catch (Exception ex) { Error("Unable to open file", ex); }
        finally { SetWorking(false); }
    }

    private async void Analyze_Click(object sender, RoutedEventArgs e)
    {
        if (_image is null || _artifact is null || _working) return;
        _analysisCancellation = new CancellationTokenSource();
        CancellationTokenSource cancellation = _analysisCancellation;
        SetWorking(true, "Analyzing direct and relocation-backed control flow…");
        try
        {
            LeImage image = _image;
            var graph = await Task.Run(() => ControlFlowAnalysis.Analyze(image, maxInstructions: 100_000,
                cancellationToken: cancellation.Token));
            cancellation.Token.ThrowIfCancellationRequested();
            // Analysis has completed; do not advertise cancellation while saving
            // a finished snapshot to the project database.
            _analysisCancellation = null;
            CancelAnalysisButton.IsEnabled = false;
            // Persist before changing the visible analysis. If storage fails,
            // the previous graph and analyst selection remain visible rather
            // than displaying a new graph that the project never saved.
            if (_project != null)
            {
                ProjectStore project = _project;
                await Task.Run(() => project.SaveAnalysis(graph));
            }
            _graph = graph;
            ShowAnalysis(graph);
            ExportAnalysisButton.IsEnabled = true;
            StatusText.Text = $"{graph.Instructions.Count:N0} candidate instructions; {graph.FunctionCandidates.Count:N0} function candidates; " +
                $"{graph.JumpTables.Count:N0} table candidates. " +
                (graph.Truncated ? "TRUNCATED by instruction budget." : "Remaining bytes are not automatically code.");
        }
        catch (OperationCanceledException)
        {
            StatusText.Text = "Analysis cancelled; previous project findings preserved.";
        }
        catch (Exception ex) { Error("Analysis failed", ex); }
        finally
        {
            _analysisCancellation = null;
            cancellation.Dispose();
            SetWorking(false);
        }
    }

    private void CancelAnalysis_Click(object sender, RoutedEventArgs e)
    {
        if (_analysisCancellation is null) return;
        _analysisCancellation.Cancel();
        CancelAnalysisButton.IsEnabled = false;
        StatusText.Text = "Cancellation requested; retaining previous findings…";
    }

    private static IReadOnlyDictionary<FlowAddress, IReadOnlyList<ReferenceRow>> IndexReferences(
        FlowAnalysisResult graph)
    {
        var index = new Dictionary<FlowAddress, List<ReferenceRow>>();
        void Add(FlowAddress source, ReferenceRow reference)
        {
            if (!index.TryGetValue(source, out var refs))
            {
                refs = new List<ReferenceRow>();
                index.Add(source, refs);
            }
            refs.Add(reference);
        }
        foreach (var edge in graph.Edges)
        {
            Add(edge.Source, new ReferenceRow("Outgoing", Format(edge.Destination),
                edge.Kind, edge.Evidence, edge.Destination));
            if (edge.Destination is { } destination)
                Add(destination, new ReferenceRow("Incoming", Format(edge.Source),
                    edge.Kind, edge.Evidence, edge.Source));
        }
        foreach (var reference in graph.DataReferences)
            Add(reference.Source, new ReferenceRow("Data", Format(reference.Target),
                reference.Kind, reference.Evidence, reference.Target));
        return index.ToDictionary(x => x.Key,
            x => (IReadOnlyList<ReferenceRow>)x.Value.ToArray());
    }

    private void ShowAnalysis(FlowAnalysisResult graph)
    {
        // Build reverse references once. Scanning every graph edge when a row
        // changes selection can make the GUI unresponsive on large DOS images.
        _referenceIndex = IndexReferences(graph);
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
        _commentRevision = 0;
        SaveNoteButton.IsEnabled = false;
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
        ReferencesGrid.ItemsSource = _referenceIndex.TryGetValue(address, out var refs)
            ? refs : Array.Empty<ReferenceRow>();
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
    private void ReferencesGrid_MouseDoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
    {
        if (_working) return;
        if (ReferencesGrid.SelectedItem is ReferenceRow { NavigationTarget: { } target })
            Navigate(target);
    }

    private static string Format(FlowAddress? at) =>
        at is { } a ? $"{a.Object}:0x{a.Offset:X8}" : "unresolved";

    private void RenderHex(FlowAddress address)
    {
        if (_image is null)
        {
            if (_artifact is not null) HexView.Text = RawHexPreview.Render(_artifact);
            return;
        }
        if (address.Object < 1 || address.Object > _image.Objects.Count) return;
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
        if (_artifact is null) return;
        try
        {
            string[] parts = AddressBox.Text.Trim().Split(':', 2);
            if (parts.Length != 2)
                throw new FormatException("Use object:0xOFFSET for LE code or file:0xOFFSET for physical file bytes.");
            string value = parts[1].StartsWith("0x", StringComparison.OrdinalIgnoreCase)
                ? parts[1][2..] : parts[1];

            if (parts[0].Equals("file", StringComparison.OrdinalIgnoreCase))
            {
                int fileOffset = int.Parse(value, NumberStyles.HexNumber, CultureInfo.InvariantCulture);
                HexView.Text = RawHexPreview.Render(_artifact, fileOffset);
                AddressBox.Text = $"file:0x{fileOffset:X8}";
                MainTabs.SelectedIndex = 1;
                StatusText.Text = "Physical source file view (unmodified original bytes); this is not LE object-relative disassembly.";
                return;
            }

            if (_image is null)
                throw new FormatException("This file has no supported executable mapping. Use file:0xOFFSET to inspect original bytes.");
            int number = int.Parse(parts[0], CultureInfo.InvariantCulture);
            uint offset = uint.Parse(value, NumberStyles.HexNumber, CultureInfo.InvariantCulture);
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
                // A navigation preview must not discard the graph or the browsable
                // analysis listing. Merge only previously absent instruction starts;
                // preserve the existing candidate evidence for matching addresses.
                var byAddress = _rows.ToDictionary(x => (x.Object, x.Offset));
                foreach (var line in lines)
                {
                    var preview = new AssemblyRow(address.Object, line.ObjectOffset, line.Bytes,
                        line.Assembly, "linear-preview-unverified");
                    byAddress.TryAdd((preview.Object, preview.Offset), preview);
                }
                _rows = byAddress.Values.OrderBy(x => x.Object).ThenBy(x => x.Offset).ToArray();
                AssemblyGrid.ItemsSource = _rows;
                StatusText.Text = "Supplemental UNVERIFIED linear preview; original analyzed listing retained.";
                found = _rows.FirstOrDefault(x => x.Object == address.Object && x.Offset == address.Offset);
            }
            catch (Exception ex) when (ex is InvalidDataException or ArgumentException or NotSupportedException)
            { Error("Cannot navigate to address", ex); return; }
        }
        if (found is null) return;
        // Navigation updates the selected address and context without
        // unexpectedly switching the user's Hexadecimal or References tab.
        AssemblyGrid.SelectedItem = found;
        if (MainTabs.SelectedIndex == 0) AssemblyGrid.ScrollIntoView(found);
    }

    private void IntelligenceList_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        IntelligenceEvidenceText.Text = IntelligenceList.SelectedItem is IntelligenceRow row
            ? row.Evidence : "Select a field to see supporting evidence.";
    }

    private async void ExportAnalysis_Click(object sender, RoutedEventArgs e)
    {
        if (_working || _graph is null) return;
        var picker = new SaveFileDialog
        {
            Title = "Export candidate analysis (no executable or source paths)",
            Filter = "JSON analysis report (*.json)|*.json|Text analysis report (*.txt)|*.txt",
            DefaultExt = ".json", AddExtension = true, OverwritePrompt = false,
            FileName = "dosdis-candidate-analysis.json"
        };
        if (picker.ShowDialog(this) != true) return;
        bool json = picker.FilterIndex == 1;
        var bytesChoice = MessageBox.Show(this,
            "Include ORIGINAL machine-byte excerpts for individual candidate instructions?\n\n" +
            "No (recommended) exports addresses, assembly and evidence only. " +
            "Neither choice embeds a complete original executable or project notes.",
            "Export privacy options", MessageBoxButton.YesNoCancel,
            MessageBoxImage.Question, MessageBoxResult.No);
        if (bytesChoice == MessageBoxResult.Cancel) return;
        FlowAnalysisResult graph = _graph ?? throw new InvalidOperationException("No analysis to export.");
        SetWorking(true, "Exporting evidence-tagged candidate analysis…");
        try
        {
            await Task.Run(() => AnalysisReportExport.WriteNew(picker.FileName, graph,
                json, bytesChoice == MessageBoxResult.Yes));
            StatusText.Text = $"Analysis report saved: {picker.FileName}. Original executable untouched.";
        }
        catch (Exception ex) { Error("Analysis report export failed", ex); }
        finally { SetWorking(false); }
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
            BackupProjectButton.IsEnabled = true;
            SaveNoteButton.IsEnabled = AssemblyGrid.SelectedItem is AssemblyRow;
        }
        catch (Exception ex) { Error("Project save failed", ex); }
    }
    private void BackupProject_Click(object sender, RoutedEventArgs e)
    {
        if (_project is null || _working) return;
        var picker = new SaveFileDialog
        {
            Title = "Create consistent DOSDis project backup",
            DefaultExt = ".ddproj", Filter = "DOSDis project (*.ddproj)|*.ddproj",
            AddExtension = true, OverwritePrompt = false,
            FileName = Path.GetFileNameWithoutExtension(_project.FilePath) + "-backup.ddproj"
        };
        if (picker.ShowDialog(this) != true) return;
        try
        {
            _project.CreateBackup(picker.FileName);
            StatusText.Text = "Project backup saved (previously committed analysis/notes only): " + picker.FileName;
        }
        catch (Exception ex) { Error("Project backup failed", ex); }
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
                FlowAnalysisResult? saved = _image is null
                    ? null  // No executable mapping: do not revive stale interpreted instructions.
                    : next.LoadLatestAnalysis(_artifact.Sha256, _image);
                _project?.Dispose();
                _project = next;
                BackupProjectButton.IsEnabled = true;
                _graph = saved;
                ExportAnalysisButton.IsEnabled = saved != null;
                _referenceIndex = new Dictionary<FlowAddress, IReadOnlyList<ReferenceRow>>();
                if (saved != null)
                {
                    ShowAnalysis(saved);
                }
                else
                {
                    // The new project has no saved graph. Do not retain rows,
                    // functions, or references from the previously open project.
                    FunctionsList.ItemsSource = Array.Empty<FunctionRow>();
                    IReadOnlyList<DecodedLine> preview = Array.Empty<DecodedLine>();
                    if (_image is { } currentImage)
                    {
                        try { preview = Disassembly.Decode(currentImage, (int)currentImage.EntryObject,
                            currentImage.EntryOffset, 128); }
                        catch (InvalidDataException) { /* No stored bytes at the entry. */ }
                    }
                    int entryObject = _image is { } loaded ? (int)loaded.EntryObject : 1;
                    SetRows(preview.Select(x => new AssemblyRow(entryObject,
                        x.ObjectOffset, x.Bytes, x.Assembly, "linear-preview-unverified")).ToArray());
                    if (_image is null && _artifact is not null)
                    {
                        HexView.Text = RawHexPreview.Render(_artifact);
                        AddressBox.Text = "file:0x0";
                        MainTabs.SelectedIndex = 1;
                    }
                }
                StatusText.Text = saved is not null
                    ? $"Project restored: {saved.Instructions.Count:N0} candidate instructions."
                    : _image is null
                        ? "Project opened for raw file metadata. Hexadecimal view shows only original bytes, not executable code."
                        : "Project opened; no analysis snapshot saved. Showing UNVERIFIED entry preview.";
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
    private void Window_Closing(object? sender, CancelEventArgs e)
    {
        if (_working)
        {
            // A background import or project save may still hold the project
            // connection. Never dispose it until the operation finishes.
            _analysisCancellation?.Cancel();
            e.Cancel = true;
            StatusText.Text = _analysisCancellation is null
                ? "Finish the current file/project operation before closing."
                : "Cancelling analysis before closing. Close again when it finishes.";
            return;
        }
        _project?.Dispose();
    }
}
