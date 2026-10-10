using System.Windows;
using DosDis.Core;

namespace DosDis.Desktop;

public partial class App : Application
{
    // One bounded log for this process. Each exported record contains only
    // controlled event types, numeric addresses/counts and source SHA-256.
    internal static DiagnosticSession Diagnostics { get; } = new();

    protected override void OnStartup(StartupEventArgs e)
    {
        Diagnostics.Record(DiagnosticStage.AppStarted);
        DispatcherUnhandledException += (_, args) =>
        {
            Diagnostics.Record(DiagnosticStage.UnhandledUiException, error: args.Exception);
            Diagnostics.TryWriteLocalIncident();
            // Do NOT mark the exception handled or resume a corrupted UI session.
        };
        AppDomain.CurrentDomain.UnhandledException += (_, args) =>
        {
            Diagnostics.Record(DiagnosticStage.UnhandledProcessException,
                error: args.ExceptionObject as Exception);
            Diagnostics.TryWriteLocalIncident();
        };
        TaskScheduler.UnobservedTaskException += (_, args) =>
        {
            Diagnostics.Record(DiagnosticStage.UnobservedTaskException, error: args.Exception);
            Diagnostics.TryWriteLocalIncident();
            // No SetObserved(): preserve default runtime failure policy.
        };
        base.OnStartup(e);
    }
}
