using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Threading;
using DosDis.Desktop;

internal static class Program
{
    // Synthetic, no original DOS executable or bytes. Exercise actual WPF
    // MainWindow markup in an STA dispatcher, not only the core/LE loader.
    private sealed class ReadOnlyInstructionRow
    {
        public string AddressText { get; init; } = "1:0x0000CC1C";
        public string BytesBefore { get; init; } = "90";
        public string BytesMatched { get; init; } = "CD";
        public string BytesAfter { get; init; } = "21";
        public string Assembly { get; init; } = "synthetic-only";
        public string Evidence { get; init; } = "no-original-bytes";
    }

    private static void Require(bool condition, string description)
    {
        if (!condition)
            throw new InvalidOperationException("WPF desktop binding regression: " + description);
    }

    [STAThread]
    private static int Main()
    {
        try
        {
            return Run();
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine("FAIL: WPF read-only byte highlight smoke: " +
                ex.GetType().FullName + ": " + ex.Message);
            return 1;
        }
    }

    private static int Run()
    {
        var app = new App();
        app.InitializeComponent();
        app.StartupUri = null;
        app.ShutdownMode = ShutdownMode.OnMainWindowClose;
        Exception? failure = null;
        app.DispatcherUnhandledException += (_, args) =>
        {
            failure = args.Exception;
            args.Handled = true; // Isolated CI smoke only; never silently resume production.
            app.Shutdown(1);
        };

        var window = new MainWindow { ShowInTaskbar = false, Title = "Original-free WPF binding smoke" };
        var grid = window.FindName("AssemblyGrid") as DataGrid
            ?? throw new InvalidOperationException("Real MainWindow disassembly grid not found.");
        var bytesColumn = grid.Columns.OfType<DataGridTemplateColumn>().Single();
        var textBlock = bytesColumn.CellTemplate?.LoadContent() as TextBlock
            ?? throw new InvalidOperationException("Real original-byte template is absent.");
        var runs = textBlock.Inlines.OfType<Run>().ToArray();
        Require(runs.Length == 3, "original-byte template must have 3 source-backed runs");
        var bindingNames = new[] { "BytesBefore", "BytesMatched", "BytesAfter" };
        for (int i = 0; i < runs.Length; i++)
        {
            var binding = BindingOperations.GetBinding(runs[i], Run.TextProperty);
            Require(binding is not null && binding.Path?.Path == bindingNames[i],
                "template Run " + i + " must preserve bound original-byte partition");
            Require(binding.Mode == BindingMode.OneWay,
                "Run.Text is TwoWay by default; read-only assembly row cannot be a TwoWay source.");
        }

        // Give WPF actual bound rows. The old bug surfaced asynchronously
        // ~100ms after file open, during dispatcher-driven row realization.
        grid.ItemsSource = Enumerable.Range(0, 128).Select(i =>
            new ReadOnlyInstructionRow { AddressText = $"1:0x{i:X8}" }).ToArray();
        int observedTicks = 0;
        var timer = new DispatcherTimer(DispatcherPriority.Background)
        {
            Interval = TimeSpan.FromMilliseconds(200)
        };
        timer.Tick += (_, _) =>
        {
            try
            {
                observedTicks++;
                window.UpdateLayout();
                grid.UpdateLayout();
                if (observedTicks < 4) return; // >600ms of deferred binding/layout.
                var row = grid.ItemContainerGenerator.ContainerFromIndex(0);
                Require(row is not null, "DataGrid failed to materialize first instruction row");
                Require(grid.Items.Count == 128, "synthetic instruction listing was replaced");
                timer.Stop();
                app.Shutdown(0);
            }
            catch (Exception ex)
            {
                failure = ex;
                timer.Stop();
                app.Shutdown(1);
            }
        };
        timer.Start();
        int result = app.Run(window);
        if (failure is not null)
            throw new InvalidOperationException("Deferred WPF row realization threw: " +
                failure.GetType().FullName + " / " + failure.Message, failure);
        Require(result == 0 && observedTicks >= 4, "WPF rendering did not complete safely");
        Console.WriteLine("PASS: real Windows WPF MainWindow, 128 read-only instruction rows, " +
            "one-way Run.Text bindings and deferred DataGrid layout (no originals)");
        return 0;
    }
}
