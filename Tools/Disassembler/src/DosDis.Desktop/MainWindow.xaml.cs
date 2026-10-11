using System.ComponentModel;
using System.Globalization;
using System.IO;
using System.Text;
using System.Windows.Input;
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
    private DosMzImage? _mzImage;
    private FlowAnalysisResult? _graph;
    private Dos16FlowResult? _mzGraph;
    private ProjectStore? _project;
    private IReadOnlyList<AssemblyRow> _rows = Array.Empty<AssemblyRow>();
    // Presentation indexing only: MZ CS:IP and LE object:offset keep their
    // distinct typed identities at every user-facing navigation boundary.
    private IReadOnlyDictionary<(int Number, uint Offset), IReadOnlyList<ReferenceRow>> _referenceIndex =
        new Dictionary<(int Number, uint Offset), IReadOnlyList<ReferenceRow>>();
    private IReadOnlyList<ReferenceRow> _allReferences = Array.Empty<ReferenceRow>();
    private bool _working;
    private long _commentRevision;
    private CancellationTokenSource? _analysisCancellation;
    private readonly FileDialogPreferences _filePreferences = new();
    private string? _openedBinaryPath;
    private IReadOnlyList<HexByteToken> _hexByteTokens = Array.Empty<HexByteToken>();
    private int? _highlightedFileOffset;
    private int _highlightedByteLength = 1;
    private string? _lastByteSearch;
    private int _lastByteMatchOffset = -1;
    private System.Windows.Point? _hexMouseDownPosition;
    private bool _suppressAutomaticHexRefresh;

    private void Trace(DiagnosticStage stage, int? objectNumber = null,
        uint? objectOffset = null, int? instructionCount = null)
    {
        App.Diagnostics.Record(stage, _artifact?.Sha256, objectNumber,
            objectOffset, instructionCount);
    }

    public MainWindow()
    {
        InitializeComponent();
        AnalyzeButton.IsEnabled = false;
        SaveProjectButton.IsEnabled = false;
        BackupProjectButton.IsEnabled = false;
        SaveNoteButton.IsEnabled = false;
        CancelAnalysisButton.IsEnabled = false;
        ExportAnalysisButton.IsEnabled = false;
        ExportDiagnosticButton.IsEnabled = true;
        // The read-only TextBox consumes mouse-up while implementing drag
        // selection. Listen even when handled so a normal click can select
        // an individual byte without breaking drag-to-copy behavior.
        HexView.AddHandler(System.Windows.UIElement.MouseLeftButtonUpEvent,
            new MouseButtonEventHandler(HexView_MouseLeftButtonUp), true);
    }

    private sealed record AssemblyRow(int Object, uint Offset, string Bytes,
        string Assembly, string Evidence, string? DisplayAddress = null)
    {
        public string AddressText => DisplayAddress ?? $"{Object}:0x{Offset:X8}";
    }
    private sealed record FunctionRow(FlowAddress? LeAddress, string DisplayName,
        Dos16Address? MzAddress = null);
    private sealed record ReferenceRow(string Direction, string AddressText, string Kind,
        string Evidence, FlowAddress? LeNavigationTarget,
        Dos16Address? MzNavigationTarget = null, string SourceText = "");
    private sealed record IntelligenceRow(string Name, string DisplayValue, string Evidence);

    private void SetWorking(bool value, string? status = null)
    {
        _working = value;
        OpenBinaryButton.IsEnabled = !value;
        AnalyzeButton.IsEnabled = !value && (_image != null || _mzImage != null);
        // Current SQLite graphs/comments use LE object addresses. Disable
        // MZ project mutation rather than silently saving an empty LE graph.
        OpenProjectButton.IsEnabled = !value && _mzImage is null;
        SaveProjectButton.IsEnabled = !value && _artifact != null && _mzImage is null;
        // Both workflows can create a project on demand. A missing project
        // must not strand an entered analyst comment behind a disabled button.
        BackupProjectButton.IsEnabled = !value && _artifact != null && _mzImage is null;
        SaveNoteButton.IsEnabled = !value && _mzImage is null && _artifact != null &&
            AssemblyGrid.SelectedItem is AssemblyRow;
        NoteEditor.IsEnabled = !value && _mzImage is null && _artifact != null &&
            AssemblyGrid.SelectedItem is AssemblyRow;
        AssemblyGrid.IsEnabled = !value;
        FunctionsList.IsEnabled = !value;
        CancelAnalysisButton.IsEnabled = value && _analysisCancellation is not null;
        ExportAnalysisButton.IsEnabled = !value && (_graph is not null || _mzGraph is not null);
        ExportDiagnosticButton.IsEnabled = !value;
        if (status != null) StatusText.Text = status;
    }
    private void Error(string context, Exception error)
    {
        App.Diagnostics.Record(DiagnosticStage.HandledError, _artifact?.Sha256, error: error);
        StatusText.Text = $"{context}: {error.Message}";
        MessageBox.Show(this, $"{context}\n\n{error.Message}", "DOSDis", MessageBoxButton.OK, MessageBoxImage.Error);
    }

    private async void OpenBinary_Click(object sender, RoutedEventArgs e)
    {
        var picker = new OpenFileDialog
        {
            Title = "Open original DOS executable (read-only)",
            Filter = "All files (*.*)|*.*|Executables (*.exe;*.com)|*.exe;*.com",
            InitialDirectory = _filePreferences.LastBinaryDirectory ?? string.Empty,
            CheckFileExists = true
        };
        if (picker.ShowDialog(this) != true) return;
        App.Diagnostics.Record(DiagnosticStage.FileOpenStarted);
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
                DosMzImage? mz = null;
                // Only standalone, source-backed MZ receives automatic
                // 16-bit decoding. Extended MZ stubs need explicit handling;
                // never reinterpret an unsupported LE/LX/PE as standalone MZ.
                if (image is null && BinaryFormatDetector.Inspect(artifact).FormatId == "mz")
                {
                    try { mz = DosMzImage.Load(artifact); }
                    catch (InvalidDataException) { /* Malformed MZ remains raw. */ }
                    catch (NotSupportedException) { /* Unsupported container remains raw. */ }
                }
                var intelligence = FileIntelligence.Inspect(artifact, image, mz: mz);
                IReadOnlyList<DecodedLine> preview = Array.Empty<DecodedLine>();
                IReadOnlyList<Dos16Instruction> mzPreview = Array.Empty<Dos16Instruction>();
                if (image != null)
                {
                    try { preview = Disassembly.Decode(image, (int)image.EntryObject, image.EntryOffset, 128); }
                    catch (InvalidDataException) { /* Entry not stored in the original file. */ }
                }
                else if (mz != null)
                {
                    try { mzPreview = mz.Decode(mz.InitialCs, mz.InitialIp, 128); }
                    catch (ArgumentException) { /* Keep backed image available for explicit navigation. */ }
                    catch (InvalidDataException) { /* Invalid initial instruction is not executable proof. */ }
                }
                return (artifact, image, mz, intelligence, preview, mzPreview);
            });
            _artifact = result.artifact;
            _openedBinaryPath = Path.GetFullPath(picker.FileName);
            _filePreferences.RememberBinary(_openedBinaryPath);
            _image = result.image;
            _mzImage = result.mz;
            Trace(_image is not null ? DiagnosticStage.FileOpenedLe
                : _mzImage is not null ? DiagnosticStage.FileOpenedMz : DiagnosticStage.FileOpenedRaw);
            _graph = null;
            _mzGraph = null;
            _highlightedFileOffset = null;
            _highlightedByteLength = 1;
            _lastByteSearch = null;
            _lastByteMatchOffset = -1;
            _hexByteTokens = Array.Empty<HexByteToken>();
            ExportAnalysisButton.IsEnabled = false;
            _referenceIndex = new Dictionary<(int Number, uint Offset), IReadOnlyList<ReferenceRow>>();
            _allReferences = Array.Empty<ReferenceRow>();
            RefreshReferences();
            if (_project != null && !_project.ListArtifacts().Any(a => a.Sha256 == _artifact.Sha256))
            {
                _project.Dispose();
                _project = null;
                BackupProjectButton.IsEnabled = false;
            }
            BinaryName.Text = _artifact.Name;
            ArtifactHash.Text = $"SHA-256 {_artifact.Sha256}";
            FileSubtitle.Text = _image is not null
                ? $"{_artifact.Length:N0} bytes · LE · {_image.Objects.Count} objects · {_image.Relocations.Count:N0} fixups"
                : _mzImage is not null
                    ? $"{_artifact.Length:N0} bytes · DOS MZ · x86-16 entry {_mzImage.InitialCs:X4}:{_mzImage.InitialIp:X4} · {_mzImage.Relocations.Count} stored fixups"
                    : $"{_artifact.Length:N0} bytes · original raw file · code analysis unavailable";
            ExplorerSectionTitle.Text = _image is not null ? "FUNCTION CANDIDATES"
                : _mzImage is not null ? "16-BIT ENTRY — CANDIDATE FLOW"
                : "RAW FILE — NO FUNCTION ANALYSIS";
            IntelligenceList.ItemsSource = result.intelligence.Select(x => new IntelligenceRow(x.Key,
                $"{x.Value.Value} ({x.Value.Status})", x.Value.Evidence)).ToArray();
            FunctionsList.ItemsSource = Array.Empty<FunctionRow>();
            AddressBox.Text = _image is not null ? $"{_image.EntryObject}:0x{_image.EntryOffset:X}"
                : _mzImage is not null ? $"{_mzImage.InitialCs:X4}:{_mzImage.InitialIp:X4}" : "file:0x0";
            if (_mzImage is not null)
            {
                SetRows(result.mzPreview.Select(x => new AssemblyRow(x.Segment, x.Offset,
                    x.OriginalBytes, x.Assembly, x.Valid ? "linear-x86-16-preview-unverified" : "invalid-decoding",
                    $"{x.Segment:X4}:{x.Offset:X4}")).ToArray());
                FunctionsList.ItemsSource = new[] { new FunctionRow(null,
                    $"{_mzImage.InitialCs:X4}:{_mzImage.InitialIp:X4}  [entry hypothesis]",
                    new Dos16Address(_mzImage.InitialCs, _mzImage.InitialIp)) };
                int mzOffset = checked((int)(_mzImage.ToFileOffset(_mzImage.InitialCs, _mzImage.InitialIp)
                    ?? throw new InvalidDataException("MZ entry is not file backed.")));
                RenderMzHex(new Dos16Address(_mzImage.InitialCs, _mzImage.InitialIp));
                MainTabs.SelectedIndex = 0;
            }
            else
                SetRows(result.preview.Select(x => new AssemblyRow(_image is null ? 1 : (int)_image.EntryObject,
                    x.ObjectOffset, x.Bytes, x.Assembly, "linear-preview-unverified")).ToArray());
            if (_image is null && _mzImage is null)
            {
                // Data/resource files are not code. Show exact physical bytes
                // immediately, without inventing a function candidate.
                SetHexWindow(RawHexPreview.Render(_artifact),
                    "Original raw file bytes, starting file+0x00000000");
                AddressBox.Text = "file:0x0";
                MainTabs.SelectedIndex = 1;
            }
            if (_project != null && _image != null)
            {
                _graph = _project.LoadLatestAnalysis(_artifact.Sha256, _image);
                if (_graph != null) ShowAnalysis(_graph);
            }
            StatusText.Text = _image is not null
                ? _graph is not null ? "Restored saved candidate analysis. Original file hash verified."
                  : "Showing an UNVERIFIED linear preview. Run Analyze for evidence-tagged reachable code."
                : _mzImage is not null
                  ? "Standalone DOS MZ loaded. Initial CS:IP is module-relative. Run Analyze for bounded x86-16 candidate control flow; not verified executed code."
                  : "RAW HEX VIEW ONLY: original file bytes at physical offsets. No executable mapping or instructions inferred.";
        }
        catch (Exception ex) { Error("Unable to open file", ex); }
        finally { SetWorking(false); }
    }

    private async void Analyze_Click(object sender, RoutedEventArgs e)
    {
        if (_artifact is null || _working) return;
        if (_mzImage is not null)
        {
            await AnalyzeMzAsync();
            return;
        }
        if (_image is null) return;
        _analysisCancellation = new CancellationTokenSource();
        CancellationTokenSource cancellation = _analysisCancellation;
        Trace(DiagnosticStage.AnalysisStarted);
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
            Trace(DiagnosticStage.AnalysisCompleted, instructionCount: graph.Instructions.Count);
            ExportAnalysisButton.IsEnabled = true;
            StatusText.Text = $"{graph.Instructions.Count:N0} candidate instructions; {graph.FunctionCandidates.Count:N0} function candidates; " +
                $"{graph.JumpTables.Count:N0} table candidates. " +
                (graph.Truncated ? "TRUNCATED by instruction budget." : "Remaining bytes are not automatically code.");
        }
        catch (OperationCanceledException)
        {
            Trace(DiagnosticStage.AnalysisCancelled);
            StatusText.Text = "Analysis cancelled; previous project findings preserved.";
        }
        catch (Exception ex)
        {
            App.Diagnostics.Record(DiagnosticStage.AnalysisFailed, _artifact?.Sha256, error: ex);
            Error("Analysis failed", ex);
        }
        finally
        {
            _analysisCancellation = null;
            cancellation.Dispose();
            SetWorking(false);
        }
    }

    private async Task AnalyzeMzAsync()
    {
        DosMzImage mz = _mzImage ?? throw new InvalidOperationException("MZ is not open.");
        Trace(DiagnosticStage.AnalysisStarted);
        SetWorking(true, "Analyzing original-backed DOS MZ entry segment as x86-16 candidate code…");
        try
        {
            // This core analyzer has an instruction budget but currently no
            // cancellation token. Do not enable Cancel for this operation.
            var graph = await Task.Run(() => Dos16ControlFlowAnalysis.Analyze(
                mz.CreateEntryCodeRegion(), mz.InitialIp, maxInstructions: 20_000));
            if (!ReferenceEquals(_mzImage, mz)) return;
            _mzGraph = graph;
            ShowMzAnalysis(graph);
            Trace(DiagnosticStage.AnalysisCompleted, instructionCount: graph.Instructions.Count);
            StatusText.Text = $"{graph.Instructions.Count:N0} unverified x86-16 candidates; " +
                $"{graph.Edges.Count:N0} control-flow edges; {graph.Diagnostics.Count:N0} diagnostics; " +
                $"{graph.UnknownFileBackedBytes:N0} source-backed bytes not classified as code. " +
                (graph.Truncated ? "Budget reached; analysis TRUNCATED." :
                 "No confirmed functions, live DOS load address or dynamic far targets.");
        }
        catch (Exception ex)
        {
            App.Diagnostics.Record(DiagnosticStage.AnalysisFailed, _artifact?.Sha256, error: ex);
            Error("MZ analysis failed", ex);
        }
        finally { SetWorking(false); }
    }

    private void CancelAnalysis_Click(object sender, RoutedEventArgs e)
    {
        if (_analysisCancellation is null) return;
        _analysisCancellation.Cancel();
        CancelAnalysisButton.IsEnabled = false;
        StatusText.Text = "Cancellation requested; retaining previous findings…";
    }

    private static IReadOnlyDictionary<(int Number, uint Offset), IReadOnlyList<ReferenceRow>> IndexReferences(
        FlowAnalysisResult graph)
    {
        var index = new Dictionary<(int Number, uint Offset), List<ReferenceRow>>();
        void Add(FlowAddress source, ReferenceRow reference)
        {
            var key = (Number: source.Object, Offset: source.Offset);
            if (!index.TryGetValue(key, out var refs))
            {
                refs = new List<ReferenceRow>();
                index.Add(key, refs);
            }
            refs.Add(reference);
        }
        foreach (var edge in graph.Edges)
        {
            Add(edge.Source, new ReferenceRow("Outgoing", Format(edge.Destination),
                edge.Kind, edge.Evidence, edge.Destination, SourceText: Format(edge.Source)));
            if (edge.Destination is { } destination)
                Add(destination, new ReferenceRow("Incoming", Format(edge.Source),
                    edge.Kind, edge.Evidence, edge.Source, SourceText: Format(edge.Source)));
        }
        foreach (var reference in graph.DataReferences)
            Add(reference.Source, new ReferenceRow("Data", Format(reference.Target),
                reference.Kind, reference.Evidence, reference.Target, SourceText: Format(reference.Source)));
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
        _allReferences = graph.Edges.Select(edge => new ReferenceRow("Edge",
            Format(edge.Destination), edge.Kind, edge.Evidence,
            edge.Destination, SourceText: Format(edge.Source)))
            .Concat(graph.DataReferences.Select(reference => new ReferenceRow("Data",
                Format(reference.Target), reference.Kind, reference.Evidence,
                reference.Target, SourceText: Format(reference.Source))))
            .ToArray();
        RefreshReferences();
    }
    private void ShowMzAnalysis(Dos16FlowResult graph)
    {
        // CS:IP remains Dos16Address and is never retyped as FlowAddress.
        var index = new Dictionary<(int Number, uint Offset), List<ReferenceRow>>();
        void Add(Dos16Address at, ReferenceRow row)
        {
            var key = (Number: (int)at.Segment, Offset: (uint)at.Offset);
            if (!index.TryGetValue(key, out var list))
                index.Add(key, list = new List<ReferenceRow>());
            list.Add(row);
        }
        static string Display(Dos16Address at) => $"{at.Segment:X4}:{at.Offset:X4}";
        foreach (var edge in graph.Edges)
        {
            Add(edge.Source, new ReferenceRow("Outgoing",
                edge.Destination is { } target ? Display(target) : "unresolved",
                edge.Kind, edge.Evidence, null, edge.Destination,
                Display(edge.Source)));
            if (edge.Destination is { } incoming)
                Add(incoming, new ReferenceRow("Incoming", Display(edge.Source),
                    edge.Kind, edge.Evidence, null, edge.Source,
                    Display(edge.Source)));
        }
        _referenceIndex = index.ToDictionary(x => x.Key,
            x => (IReadOnlyList<ReferenceRow>)x.Value.ToArray());
        SetRows(graph.Instructions.Select(x => new AssemblyRow(
            x.Address.Segment, x.Address.Offset, x.OriginalBytes, x.Assembly,
            $"x86-16-candidate;{x.FlowControl};runtime-unverified",
            Display(x.Address))).ToArray());
        FunctionsList.ItemsSource = new[] { new FunctionRow(null,
            $"{Display(graph.Entry)}  [entry hypothesis; no function proof]",
            graph.Entry) };
        _allReferences = graph.Edges.Select(edge => new ReferenceRow("Edge",
            edge.Destination is { } target ? Display(target) : "unresolved",
            edge.Kind, edge.Evidence, null, edge.Destination, Display(edge.Source))).ToArray();
        RefreshReferences();
        ExportAnalysisButton.IsEnabled = true;
    }

    private void SetRows(IReadOnlyList<AssemblyRow> items)
    {
        _rows = items;
        AssemblyGrid.ItemsSource = items;
        ReferencesGrid.ItemsSource = null;
        ReferencesSummary.Text = "No analyzed references";
        _commentRevision = 0;
        SaveNoteButton.IsEnabled = false;
        NoteEditor.IsEnabled = false;
        NoteEditor.Text = string.Empty;
        SelectedAddressText.Text = "Select an instruction";
        HexView.Text = string.Empty;
        _hexByteTokens = Array.Empty<HexByteToken>();
        HexLocationText.Text = "No physical file position selected";
    }

    private void FunctionsList_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (FunctionsList.SelectedItem is not FunctionRow row) return;
        if (_mzImage is not null && row.MzAddress is { } mzAddress)
            NavigateMz(mzAddress);
        else if (_image is not null && row.LeAddress is { } leAddress)
            Navigate(leAddress);
    }
    private void AssemblyGrid_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (AssemblyGrid.SelectedItem is not AssemblyRow row || _artifact is null)
        {
            SaveNoteButton.IsEnabled = false;
            NoteEditor.IsEnabled = false;
            return;
        }
        NoteEditor.IsEnabled = !_working && _mzImage is null;
        SelectedAddressText.Text = row.AddressText;
        AddressBox.Text = _mzImage is not null ? row.AddressText : $"{row.Object}:0x{row.Offset:X}";
        RefreshReferences();
        SelectMatchingReference(row);
        if (!_suppressAutomaticHexRefresh)
        {
            long? physical = _mzImage is not null
                ? row.Object is >= 0 and <= ushort.MaxValue && row.Offset <= ushort.MaxValue
                    ? _mzImage.ToFileOffset((ushort)row.Object, (ushort)row.Offset)
                    : null
                : _image?.ToFileOffset(row.Object, row.Offset);
            _highlightedFileOffset = physical is >= 0 and <= int.MaxValue
                ? checked((int)physical.Value) : null;
            _highlightedByteLength = 1;
            if (_mzImage is not null)
                RenderMzHex(new Dos16Address(checked((ushort)row.Object), checked((ushort)row.Offset)));
            else
                RenderHex(new FlowAddress(row.Object, row.Offset));
        }
        _commentRevision = 0;
        NoteEditor.Text = string.Empty;
        if (_project != null && _mzImage is null)
        {
            var note = _project.GetAnnotation(_artifact.Sha256, row.Object, row.Offset, "comment");
            if (note != null) { NoteEditor.Text = note.Body; _commentRevision = note.Revision; }
        }
        SaveNoteButton.IsEnabled = !_working && _mzImage is null && _artifact != null;
    }
    private void SelectMatchingReference(AssemblyRow row)
    {
        if (ReferencesGrid.ItemsSource is not IEnumerable<ReferenceRow> available)
            return;
        string selectedAddress = row.AddressText;
        ReferenceRow? reference = available.FirstOrDefault(x =>
            x.SourceText == selectedAddress || x.AddressText == selectedAddress);
        ReferencesGrid.SelectedItem = reference;
        if (reference is not null && MainTabs.SelectedIndex == 2)
            ReferencesGrid.ScrollIntoView(reference);
    }

    private void SelectedReferencesOnly_Changed(object sender, RoutedEventArgs e)
    {
        RefreshReferences();
        if (AssemblyGrid.SelectedItem is AssemblyRow selected)
            SelectMatchingReference(selected);
    }

    private void RefreshReferences()
    {
        // An analyzed graph can have hundreds of edges. Default to a
        // complete edge list; narrowing to the selected instruction is opt-in.
        if (ReferencesGrid is null || ReferencesSummary is null ||
            SelectedReferencesOnlyCheckBox is null) return;
        bool selectedOnly = SelectedReferencesOnlyCheckBox.IsChecked == true;
        IReadOnlyList<ReferenceRow> shown = _allReferences;
        if (selectedOnly)
        {
            shown = AssemblyGrid.SelectedItem is AssemblyRow row &&
                _referenceIndex.TryGetValue((row.Object, row.Offset), out var at)
                ? at : Array.Empty<ReferenceRow>();
        }
        // Global data does not change when the selected disassembly row
        // changes. Preserve scroll position and virtualization state.
        if (!ReferenceEquals(ReferencesGrid.ItemsSource, shown))
            ReferencesGrid.ItemsSource = shown;
        ReferencesSummary.Text = selectedOnly
            ? $"{shown.Count:N0} references at selected instruction"
            : $"{shown.Count:N0} graph references (including unresolved)";
    }

    private void AssemblyGrid_MouseDoubleClick(object sender,
        System.Windows.Input.MouseButtonEventArgs e)
    {
        if (_working || AssemblyGrid.SelectedItem is not AssemblyRow row) return;
        // Ignore headers and empty-grid clicks; only the selected
        // disassembly row can request a candidate control-flow jump.
        System.Windows.DependencyObject? visual = e.OriginalSource as System.Windows.DependencyObject;
        while (visual is not null && visual is not DataGridRow)
            visual = visual is System.Windows.Media.Visual
                ? System.Windows.Media.VisualTreeHelper.GetParent(visual)
                : System.Windows.LogicalTreeHelper.GetParent(visual);
        if (visual is not DataGridRow clicked ||
            !ReferenceEquals(clicked.Item, row)) return;
        if (_mzGraph is null || _mzImage is null)
        {
            StatusText.Text = "Run Analyze before following a candidate 16-bit direct call or jump.";
            return;
        }
        var source = new Dos16Address(checked((ushort)row.Object),
            checked((ushort)row.Offset));
        Dos16Address? target = Dos16Navigation.DirectTarget(_mzGraph.Edges, source);
        if (target is not { } resolved)
        {
            StatusText.Text = "No unique source-backed direct branch/call destination. " +
                "Indirect, far and runtime-dependent targets are not guessed.";
            return;
        }
        NavigateMz(resolved, activateDisassembly: true);
        e.Handled = true;
    }

    private void ReferencesGrid_MouseDoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
    {
        if (_working) return;
        if (ReferencesGrid.SelectedItem is not ReferenceRow row) return;
        if (_mzImage is not null && row.MzNavigationTarget is { } mzTarget)
            NavigateMz(mzTarget);
        else if (_image is not null && row.LeNavigationTarget is { } leTarget)
            Navigate(leTarget);
    }

    private static string Format(FlowAddress? at) =>
        at is { } a ? $"{a.Object}:0x{a.Offset:X8}" : "unresolved";

    private void AddressBox_KeyDown(object sender, KeyEventArgs e)
    {
        if (e.Key != Key.Enter || _working) return;
        e.Handled = true;
        GoTo_Click(sender, new RoutedEventArgs());
    }

    private void MainTabs_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (!ReferenceEquals(e.Source, MainTabs) || MainTabs.SelectedIndex != 1) return;
        // The TextBox may have been collapsed while its selection changed.
        Dispatcher.BeginInvoke(new Action(HighlightVisibleHexByte),
            System.Windows.Threading.DispatcherPriority.Loaded);
    }

    private void HexView_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e) =>
        _hexMouseDownPosition = e.GetPosition(HexView);

    private void HexView_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
        if (_working || _artifact is null || _hexMouseDownPosition is not { } down)
            return;
        _hexMouseDownPosition = null;
        System.Windows.Point end = e.GetPosition(HexView);
        if ((end - down).Length > 4)
            return; // preserve normal drag-to-select/copy behavior.
        int index = HexView.GetCharacterIndexFromPoint(end, true);
        HexByteToken? token = HexByteNavigation.HitTest(_hexByteTokens, index);
        if (token is null) return; // ASCII, address, gap, or LE ??.
        SelectPhysicalBytes(token.PhysicalFileOffset, 1, preserveGoQuery: false);
        StatusText.Text = $"Selected exact original file+0x{token.PhysicalFileOffset:X8}, " +
            $"byte {_artifact.Data[token.PhysicalFileOffset]:X2}; source remains read-only.";
    }

    private void HighlightVisibleHexByte()
    {
        if (_highlightedFileOffset is not { } offset) return;
        HexByteToken? start = HexByteNavigation.FindPhysical(_hexByteTokens, offset);
        if (start is null) return;
        int selectionLength = 2; // always at least the clicked/search first byte.
        if (_highlightedByteLength > 1)
        {
            int lastPhysical = checked(offset + _highlightedByteLength - 1);
            HexByteToken? end = HexByteNavigation.FindPhysical(_hexByteTokens, lastPhysical);
            if (end is not null)
            {
                int firstLine = HexView.Text.LastIndexOf('\n', start.CharacterStart);
                int lastLine = HexView.Text.LastIndexOf('\n', end.CharacterStart);
                // For same-row sequences include intervening byte separators.
                // Never select ASCII/prefix columns on a multi-row pattern.
                if (firstLine == lastLine)
                    selectionLength = end.CharacterStart + 2 - start.CharacterStart;
            }
        }
        HexView.Select(start.CharacterStart, selectionLength);
        if (MainTabs.SelectedIndex == 1)
            HexView.ScrollToLine(HexView.GetLineIndexFromCharacterIndex(start.CharacterStart));
    }

    private AssemblyRow? FindInstructionAtFileOffset(int fileOffset)
    {
        foreach (AssemblyRow row in _rows)
        {
            int count = row.Bytes.Length / 2;
            if (count is < 1 or > 15) continue;
            if (_mzImage is not null)
            {
                if (row.Object is < 0 or > ushort.MaxValue ||
                    row.Offset > ushort.MaxValue) continue;
                long? first = _mzImage.ToFileOffset((ushort)row.Object, (ushort)row.Offset);
                if (first is { } start && fileOffset >= start &&
                    fileOffset - start < count) return row;
            }
            else if (_image is not null)
            {
                for (int i = 0; i < count; i++)
                {
                    if (row.Offset > uint.MaxValue - (uint)i) break;
                    if (_image.ToFileOffset(row.Object, row.Offset + (uint)i) == fileOffset)
                        return row;
                }
            }
        }
        return null; // Do not invent a disassembly row for undecoded data.
    }

    private void SelectPhysicalBytes(int fileOffset, int length,
        bool preserveGoQuery = true)
    {
        BinaryArtifact source = _artifact ?? throw new InvalidOperationException("No file loaded.");
        if (fileOffset < 0 || length < 1 ||
            (long)fileOffset + length > source.Length)
            throw new ArgumentOutOfRangeException(nameof(fileOffset));

        _highlightedFileOffset = fileOffset;
        _highlightedByteLength = length;
        if (HexByteNavigation.FindPhysical(_hexByteTokens, fileOffset) is null)
        {
            // A search might find header/data bytes outside the current MZ
            // entry region, or bytes without a decoded instruction.
            SetHexWindow(RawHexPreview.Render(source, fileOffset),
                $"Original file+0x{fileOffset:X8} " +
                $"(first displayed row file+0x{(fileOffset & ~15):X8})");
        }
        else HighlightVisibleHexByte();

        AssemblyRow? matchedInstruction = FindInstructionAtFileOffset(fileOffset);
        if (matchedInstruction is not null)
        {
            _suppressAutomaticHexRefresh = true;
            try { AssemblyGrid.SelectedItem = matchedInstruction; }
            finally { _suppressAutomaticHexRefresh = false; }
            if (MainTabs.SelectedIndex == 0)
                AssemblyGrid.ScrollIntoView(matchedInstruction);

            // Highlight a real graph reference only when its source or
            // destination corresponds to this mapped candidate instruction.
            string sourceAddress = matchedInstruction.AddressText;
            SelectMatchingReference(matchedInstruction);
        }
        else
        {
            ReferencesGrid.SelectedItem = null;
            // Undecoded source bytes have no honest disassembly or
            // reference highlight. Show the exact hex cell instead.
            if (MainTabs.SelectedIndex != 1) MainTabs.SelectedIndex = 1;
        }
        HighlightVisibleHexByte();
        if (!preserveGoQuery)
            AddressBox.Text = $"file:0x{fileOffset:X8}";
    }

    private void SearchOriginalBytes(string query, byte[] pattern)
    {
        BinaryArtifact source = _artifact ?? throw new InvalidOperationException("No file loaded.");
        string normalized = Convert.ToHexString(pattern);
        int next = string.Equals(_lastByteSearch, normalized, StringComparison.Ordinal)
            ? _lastByteMatchOffset + 1
            : (_highlightedFileOffset ?? 0);
        int found = HexByteNavigation.FindNext(source.Data, pattern,
            Math.Clamp(next, 0, source.Length));
        if (found < 0)
        {
            StatusText.Text = $"No original-file match for hex bytes {normalized}.";
            return;
        }
        _lastByteSearch = normalized;
        _lastByteMatchOffset = found;
        SelectPhysicalBytes(found, pattern.Length);
        // Keep the original query so pressing Enter again finds the NEXT hit.
        AddressBox.Text = query;
        StatusText.Text = $"Original bytes {normalized} found at file+0x{found:X8} " +
            $"(match length {pattern.Length}); Enter again finds next. " +
            "Candidate code/reference rows are selected only if source-backed.";
    }

    private void SetHexWindow(string preview, string location)
    {
        HexLocationText.Text = location;
        HexView.Text = preview;
        _hexByteTokens = HexByteNavigation.MapRenderedBytes(preview, (number, offset) =>
        {
            long? mapped = _image?.ToFileOffset(number, offset);
            return mapped is >= 0 and <= int.MaxValue ? checked((int)mapped.Value) : null;
        });
        // A WPF TextBox can retain its old vertical position when
        // text is replaced; ensure Go shows the requested first row.
        HexView.CaretIndex = 0;
        HexView.ScrollToHome();
        HighlightVisibleHexByte();
    }

    private void RenderMzHex(Dos16Address address)
    {
        if (_mzImage is null || _artifact is null) return;
        long? physical = _mzImage.ToFileOffset(address.Segment, address.Offset);
        if (physical.HasValue)
            SetHexWindow(RawHexPreview.Render(_artifact, checked((int)physical.Value)),
                $"MZ module CS:IP {address.Segment:X4}:{address.Offset:X4} → " +
                $"original file+0x{physical.Value:X8} " +
                $"(first displayed row file+0x{(physical.Value & ~15L):X8})");
    }

    private void RenderHex(FlowAddress address)
    {
        if (_image is null)
        {
            if (_artifact is not null)
                SetHexWindow(RawHexPreview.Render(_artifact),
                    "Original raw file bytes, starting file+0x00000000");
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
        SetHexWindow(output.ToString(),
            $"LE object {address.Object}, virtual offset 0x{address.Offset:X8} " +
            "(original-file mapping may contain unbacked ?? bytes)");
    }

    private void GoTo_Click(object sender, RoutedEventArgs e)
    {
        if (_artifact is null) return;
        try
        {
            string query = AddressBox.Text.Trim();
            if (HexByteNavigation.TryParseBytePattern(query, out byte[] pattern))
            {
                SearchOriginalBytes(query, pattern);
                return;
            }
            if (query.StartsWith("hex:", StringComparison.OrdinalIgnoreCase))
                throw new FormatException("Hex byte patterns require 1–64 complete bytes (for example hex:CD 21).");
            string[] parts = query.Split(':', 2);
            if (parts.Length != 2)
                throw new FormatException(
                    "Enter MZ CS:IP, LE object:0xOFFSET, file:0xOFFSET, or hex bytes such as CD 21.");
            string value = parts[1].StartsWith("0x", StringComparison.OrdinalIgnoreCase)
                ? parts[1][2..] : parts[1];

            if (parts[0].Equals("file", StringComparison.OrdinalIgnoreCase))
            {
                int fileOffset = int.Parse(value, NumberStyles.HexNumber, CultureInfo.InvariantCulture);
                MainTabs.SelectedIndex = 1;  // reveal Hex before scrolling
                _highlightedFileOffset = fileOffset;
                _highlightedByteLength = 1;
                SetHexWindow(RawHexPreview.Render(_artifact, fileOffset),
                    $"Original file+0x{fileOffset:X8} " +
                    $"(first displayed row file+0x{(fileOffset & ~15):X8})");
                SelectPhysicalBytes(fileOffset, 1, preserveGoQuery: false);
                StatusText.Text = "Physical source file view (unmodified original bytes); this is not LE object-relative disassembly.";
                return;
            }

            if (_mzImage is not null)
            {
                // MZ addresses are hexadecimal MODULE-RELATIVE CS:IP, not
                // runtime selectors and never 32-bit LE object numbers.
                string segment = parts[0].StartsWith("0x", StringComparison.OrdinalIgnoreCase)
                    ? parts[0][2..] : parts[0];
                ushort cs = ushort.Parse(segment, NumberStyles.HexNumber, CultureInfo.InvariantCulture);
                ushort ip = ushort.Parse(value, NumberStyles.HexNumber, CultureInfo.InvariantCulture);
                NavigateMz(new Dos16Address(cs, ip));
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
        long? original = _image.ToFileOffset(address.Object, address.Offset);
        _highlightedFileOffset = original is >= 0 and <= int.MaxValue
            ? checked((int)original.Value) : null;
        _highlightedByteLength = 1;
        Trace(DiagnosticStage.Navigation, address.Object, address.Offset);
        // Navigation updates the selected address and context without
        // unexpectedly switching the user's Hexadecimal or References tab.
        AssemblyGrid.SelectedItem = found;
        // Selecting the same row twice does not raise SelectionChanged.
        if (MainTabs.SelectedIndex == 1) RenderHex(address);
        SelectMatchingReference(found);
        if (MainTabs.SelectedIndex == 0) AssemblyGrid.ScrollIntoView(found);
    }

    private void NavigateMz(Dos16Address address, bool activateDisassembly = false)
    {
        DosMzImage mz = _mzImage ?? throw new InvalidOperationException("MZ is not open.");
        ushort cs = address.Segment;
        ushort ip = address.Offset;
        if (mz.ToFileOffset(cs, ip) is null)
        {
            StatusText.Text = "Selected CS:IP is outside original-file-backed MZ load bytes.";
            return;
        }
        AddressBox.Text = $"{cs:X4}:{ip:X4}";
        long? original = mz.ToFileOffset(cs, ip);
        _highlightedFileOffset = original is >= 0 and <= int.MaxValue
            ? checked((int)original.Value) : null;
        _highlightedByteLength = 1;
        if (activateDisassembly) MainTabs.SelectedIndex = 0;
        if (MainTabs.SelectedIndex == 1)
        {
            // Hex Go can inspect ANY originally backed byte without
            // inventing a code boundary or requiring a graph row.
            RenderMzHex(address);
            Trace(DiagnosticStage.Navigation);
            StatusText.Text = $"MZ CS:IP {cs:X4}:{ip:X4} mapped to physical original-file bytes.";
            return;
        }
        AssemblyRow? found = _rows.FirstOrDefault(x => x.Object == cs && x.Offset == ip);
        if (found is null)
        {
            try
            {
                var preview = mz.Decode(cs, ip, 64);
                var byAddress = _rows.ToDictionary(x => (x.Object, x.Offset));
                foreach (var ins in preview)
                {
                    var item = new AssemblyRow(ins.Segment, ins.Offset, ins.OriginalBytes,
                        ins.Assembly, ins.Valid ? "supplemental-x86-16-unverified" : "invalid-decoding",
                        $"{ins.Segment:X4}:{ins.Offset:X4}");
                    byAddress.TryAdd((item.Object, item.Offset), item);
                }
                _rows = byAddress.Values.OrderBy(x => x.Object).ThenBy(x => x.Offset).ToArray();
                AssemblyGrid.ItemsSource = _rows;
                found = _rows.FirstOrDefault(x => x.Object == cs && x.Offset == ip);
                StatusText.Text = "Supplemental source-backed x86-16 preview; no verified code/function claims.";
            }
            catch (Exception ex) when (ex is InvalidDataException or ArgumentException or NotSupportedException)
            {
                Error("Cannot navigate to MZ address", ex);
                return;
            }
        }
        if (found is null) return;
        AddressBox.Text = $"{cs:X4}:{ip:X4}";
        // Diagnostic events accept positive LE object IDs only. Do not
        // mislabel MZ CS=0000 as an LE object index.
        Trace(DiagnosticStage.Navigation);
        AssemblyGrid.SelectedItem = found;
        // Always reposition Hex, even if selecting an unchanged row.
        RenderMzHex(address);
        SelectMatchingReference(found);
        if (MainTabs.SelectedIndex == 0) AssemblyGrid.ScrollIntoView(found);
    }

    private void IntelligenceList_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        IntelligenceEvidenceText.Text = IntelligenceList.SelectedItem is IntelligenceRow row
            ? row.Evidence : "Select a field to see supporting evidence.";
    }

    private async void ExportAnalysis_Click(object sender, RoutedEventArgs e)
    {
        if (_working || (_graph is null && _mzGraph is null)) return;
        bool mzMode = _mzGraph is not null && _mzImage is not null;
        var picker = new SaveFileDialog
        {
            Title = "Export candidate analysis (no executable or source paths)",
            Filter = "JSON analysis report (*.json)|*.json|Text analysis report (*.txt)|*.txt",
            DefaultExt = ".json", AddExtension = true, OverwritePrompt = false,
            FileName = mzMode ? "dosdis-mz16-candidate-analysis.json"
                : "dosdis-candidate-analysis.json"
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
        var selected = ChooseSaveDestination(picker.FileName, "analysis export", json ? ".json" : ".txt");
        if (selected is null) return;
        string destination = selected.Value.Path;
        bool overwrite = selected.Value.Overwrite;
        FlowAnalysisResult? graph = _graph;
        Dos16FlowResult? mzGraph = _mzGraph;
        BinaryArtifact artifact = _artifact ?? throw new InvalidOperationException("Original source is not open.");
        LeImage? image = _image;
        DosMzImage? mz = _mzImage;
        SetWorking(true, "Exporting evidence-tagged candidate analysis…");
        try
        {
            await Task.Run(() =>
            {
                // Both exporters preserve the exact source SHA and never
                // publish the entire proprietary source or its private path.
                // MZ remains module-relative x86-16, not a fabricated LE
                // object graph. Keep the existing LE export unchanged.
                if (mz is not null && mzGraph is not null)
                {
                    ProjectFilePaths.PublishNewOrReplace(destination, overwrite, temporary =>
                        Dos16DesktopReportExport.WriteNew(temporary, artifact, mz, mzGraph,
                            json, bytesChoice == MessageBoxResult.Yes));
                }
                else if (image is not null && graph is not null)
                {
                    var provenance = SourceProvenance.ForArtifact(artifact, image);
                    var gaps = ExecutableGapAnalysis.Analyze(graph, image);
                    ProjectFilePaths.PublishNewOrReplace(destination, overwrite, temporary =>
                        AnalysisReportExport.WriteNew(temporary, graph,
                            json, bytesChoice == MessageBoxResult.Yes, provenance, gaps));
                }
                else throw new InvalidOperationException("No matching source-backed analysis to export.");
            });
            Trace(DiagnosticStage.AnalysisReportExported);
            StatusText.Text = $"Analysis report saved: {destination}. Original executable untouched.";
        }
        catch (Exception ex) { Error("Analysis report export failed", ex); }
        finally { SetWorking(false); }
    }

    private async void ExportDiagnostic_Click(object sender, RoutedEventArgs e)
    {
        if (_working) return;
        var consent = MessageBox.Show(this,
            "Export this session's recent operation events and source SHA-256 identities?\n\n" +
            "The report excludes original executable bytes, local paths, analyst notes, " +
            "exception messages and stack traces. No data is uploaded.\n\n" +
            "The report is a bounded event history, not a replay or instruction-level debugger.",
            "Diagnostic report — privacy review",
            MessageBoxButton.YesNo, MessageBoxImage.Information, MessageBoxResult.No);
        if (consent != MessageBoxResult.Yes) return;
        var picker = new SaveFileDialog
        {
            Title = "Save private DOSDis diagnostic session (JSON)",
            Filter = "DOSDis diagnostic session (*.json)|*.json",
            DefaultExt = ".json", AddExtension = true, OverwritePrompt = false,
            FileName = "dosdis-diagnostic-session.json"
        };
        if (picker.ShowDialog(this) != true) return;
        var selected = ChooseSaveDestination(picker.FileName, "diagnostic export", ".json");
        if (selected is null) return;
        string destination = selected.Value.Path;
        bool overwrite = selected.Value.Overwrite;
        SetWorking(true, "Exporting bounded diagnostic events…");
        try
        {
            await Task.Run(() =>
                ProjectFilePaths.PublishNewOrReplace(destination, overwrite,
                    temporary => App.Diagnostics.WriteReportNew(temporary)));
            Trace(DiagnosticStage.DiagnosticReportExported);
            StatusText.Text = "Diagnostic session exported to " + destination +
                " (no binary bytes, comments or source paths in the report).";
        }
        catch (Exception ex) { Error("Diagnostic export failed", ex); }
        finally { SetWorking(false); }
    }

    /// <summary>
    /// A regular filename conflict is not an error. Offer Replace, Keep both,
    /// or Cancel. The default is Keep both, which chooses a numbered filename.
    /// Never allow the open original game binary to be overwritten.
    /// </summary>
    private (string Path, bool Overwrite)? ChooseSaveDestination(
        string selectedPath, string purpose, string requiredExtension)
    {
        string destination = Path.GetFullPath(selectedPath);
        if (!string.Equals(Path.GetExtension(destination), requiredExtension,
            StringComparison.OrdinalIgnoreCase))
        {
            MessageBox.Show(this,
                "This output must use " + requiredExtension +
                ". Choose a different filename.",
                "Unsupported destination extension", MessageBoxButton.OK,
                MessageBoxImage.Information);
            return null;
        }
        if (!File.Exists(destination) && !Directory.Exists(destination))
            return (destination, false);
        string numbered = ProjectFilePaths.NextAvailable(destination);
        if (Directory.Exists(destination))
        {
            MessageBox.Show(this,
                "A folder already uses that name. The new file will be saved as:\n\n" +
                Path.GetFileName(numbered),
                "Keep both", MessageBoxButton.OK, MessageBoxImage.Information);
            return (numbered, false);
        }
        bool original = _openedBinaryPath is not null &&
            string.Equals(_openedBinaryPath, destination,
                StringComparison.OrdinalIgnoreCase);
        bool currentProject = _project is not null &&
            string.Equals(_project.FilePath, destination,
                StringComparison.OrdinalIgnoreCase);
        if (original || (currentProject && purpose == "project backup"))
        {
            MessageBox.Show(this,
                (original
                    ? "Original executable files are always read-only. "
                    : "The active research project cannot be replaced by its own backup. ") +
                "The new file will be saved as:\n\n" + Path.GetFileName(numbered),
                "Keep both", MessageBoxButton.OK, MessageBoxImage.Information);
            return (numbered, false);
        }
        var answer = MessageBox.Show(this,
            "A file named " + Path.GetFileName(destination) + " already exists.\n\n" +
            "YES — Replace the existing file (previous contents are lost).\n" +
            "NO — Keep both by saving as " + Path.GetFileName(numbered) + ".\n" +
            "CANCEL — Do not save.\n\n" +
            "Replacing a project requires that no other application has it open.",
            "File already exists — " + purpose,
            MessageBoxButton.YesNoCancel, MessageBoxImage.Question,
            MessageBoxResult.No);
        return answer switch
        {
            MessageBoxResult.Yes => (destination, true),
            MessageBoxResult.No => (numbered, false),
            _ => null
        };
    }

    /// <summary>
    /// A cancelled Save As dialog leaves all typed comments intact. New
    /// projects are staged and verified before publishing the database file.
    /// </summary>
    private bool TryCreateProject(string action)
    {
        if (_project != null && _mzImage is null) return true;
        if (_mzImage is not null)
            throw new NotSupportedException(
                "MZ analysis can be exported as JSON or text. LE-only SQLite project snapshots and notes are not yet supported for MZ addresses.");
        if (_artifact is null) return false;
        var picker = new SaveFileDialog
        {
            Title = "Create DOSDis research project to " + action,
            DefaultExt = ".ddproj",
            Filter = "DOSDis project (*.ddproj)|*.ddproj",
            AddExtension = true,
            OverwritePrompt = false,
            FileName = ProjectFilePaths.DefaultProjectName(_artifact.Name)
        };
        if (picker.ShowDialog(this) != true) return false;
        var destination = ChooseSaveDestination(picker.FileName, "new project", ".ddproj");
        if (destination is null) return false;
        _project = ProjectStore.CreateNew(destination.Value.Path, _artifact,
            _graph, overwrite: destination.Value.Overwrite);
        Trace(DiagnosticStage.ProjectSaved);
        BackupProjectButton.IsEnabled = true;
        return true;
    }

    private void SaveProject_Click(object sender, RoutedEventArgs e)
    {
        if (_artifact is null || _working || _mzImage is not null) return;
        try
        {
            bool alreadyOpen = _project is not null;
            if (!TryCreateProject("save this binary's analysis")) return;
            var project = _project ?? throw new InvalidOperationException("No research project.");
            if (alreadyOpen)
            {
                project.EnsureArtifact(_artifact);
                if (_graph is not null) project.SaveAnalysis(_graph);
                Trace(DiagnosticStage.ProjectSaved);
            }
            StatusText.Text = $"Project saved: {project.FilePath}. " +
                "Original executable bytes are not embedded.";
            SaveNoteButton.IsEnabled = AssemblyGrid.SelectedItem is AssemblyRow;
        }
        catch (Exception ex) { Error("Project save failed", ex); }
    }

    private void BackupProject_Click(object sender, RoutedEventArgs e)
    {
        if (_artifact is null || _working || _mzImage is not null) return;
        try
        {
            if (!TryCreateProject("create a project backup")) return;
            var project = _project ?? throw new InvalidOperationException("No research project.");
            var picker = new SaveFileDialog
            {
                Title = "Create consistent DOSDis project backup",
                DefaultExt = ".ddproj", Filter = "DOSDis project (*.ddproj)|*.ddproj",
                AddExtension = true, OverwritePrompt = false,
                FileName = Path.GetFileNameWithoutExtension(project.FilePath) + "-backup.ddproj"
            };
            if (picker.ShowDialog(this) != true) return;
            var destination = ChooseSaveDestination(picker.FileName, "project backup", ".ddproj");
            if (destination is null) return;
            project.CreateBackup(destination.Value.Path,
                overwrite: destination.Value.Overwrite);
            Trace(DiagnosticStage.ProjectBackupSaved);
            StatusText.Text =
                "Project backup saved (previously committed analysis/notes only): " +
                destination.Value.Path;
        }
        catch (Exception ex) { Error("Project backup failed", ex); }
    }

    private void OpenProject_Click(object sender, RoutedEventArgs e)
    {
        if (_mzImage is not null)
        {
            MessageBox.Show(this,
                "16-bit MZ candidate graphs use CS:IP addresses and are not yet stored in the LE-only project database. Use Export to save a source-bound JSON/text report.",
                "MZ project support pending", MessageBoxButton.OK, MessageBoxImage.Information);
            return;
        }
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
                _referenceIndex = new Dictionary<(int Number, uint Offset), IReadOnlyList<ReferenceRow>>();
                _allReferences = Array.Empty<ReferenceRow>();
                RefreshReferences();
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
                        SetHexWindow(RawHexPreview.Render(_artifact),
                            "Original raw file bytes, starting file+0x00000000");
                        AddressBox.Text = "file:0x0";
                        MainTabs.SelectedIndex = 1;
                    }
                }
                Trace(DiagnosticStage.ProjectOpened);
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
        if (_working || _mzImage is not null || _artifact is null || AssemblyGrid.SelectedItem is not AssemblyRow row) return;
        try
        {
            // Keep the unsaved text in the editor while asking for a project;
            // cancelling the dialog does not discard the draft.
            if (!TryCreateProject("save this comment")) return;
            var project = _project ?? throw new InvalidOperationException("No research project.");
            var note = project.SaveAnnotation(_artifact.Sha256, row.Object, row.Offset,
                "comment", NoteEditor.Text, _commentRevision);
            _commentRevision = note.Revision;
            Trace(DiagnosticStage.AnalystCommentSaved, row.Object, row.Offset);
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
        Trace(DiagnosticStage.AppClosed);
        _project?.Dispose();
    }
}
