# DOSDis desktop — discoverable crash reports in a Logs folder

**Requested:** Store crash files in a `Logs` folder instead of requiring
the owner to navigate to a hidden Windows application-data `Incidents`
directory. Keep original DOS game binaries untouched and out of public CI.

## Behavior (new Windows desktop source)

1. By default, original-free JSON incident reports are written into
   **`Logs/` beside `DosDis.Desktop.exe`** (for example, an extracted
   portable build's `DosDis.Desktop.exe` and adjacent `Logs` directory).
   A report name is `incident-YYYYMMddTHHmmssfff-GUID.json`; each report
   uses a newly generated name, atomic creation, and never overwrites
   an older report.
2. If Windows denies writes beside the executable (for example, under
   `Program Files`), the report automatically falls back to
   **`%LOCALAPPDATA%\DOSDis\Logs`**.
3. A new **Logs** button next to **Diagnostics…** opens the actual
   writable/current-session Logs folder in Windows Explorer.
   When no report has been written yet, the button creates/probes
   the preferred folder without overwriting any existing file;
   it falls back if that folder cannot be written.
4. Both existing managed unhandled-exception handlers and the
   recoverable file-open/selection exception handlers use the same
   local crash report path policy. The app's on-demand
   **Diagnostics…** export remains separate: the user can still
   choose its destination.
5. A crash report includes a bounded history of operation stages,
   original-file SHA-256 identity, optional numeric addresses/counts,
   and **exception type**. It does not include source paths,
   original executable bytes, exception messages, stack traces or
   analyst annotations, and is never automatically uploaded.
   Managed exception handlers cannot guarantee a report for hard
   native/process-termination failures.
6. Previously saved reports under
   `%LOCALAPPDATA%\DOSDis\Incidents` remain where they are;
   the new behavior applies to newly generated reports and does
   not silently relocate/delete older incident files.

## Automatic original-free regression tests

The new `CrashLogsSmoke` covers:

- Portable `Logs` selection, JSON report creation and exact file location.
- Two separate exceptions produce distinct report files; neither
  overwrites the other.
- Source paths or exception messages are not serialized.
- An unwritable portable installation directory triggers the
  per-user `DOSDis/Logs` fallback without altering blocking files.
- Both unavailable paths return failure without throwing another exception.
- The actual Windows WPF smoke harness checks that the
  owner-facing **Logs** toolbar action exists.

No proprietary game files are included in tests or public builds.

## Release verification

This section records the exact Windows run and artifact once CI
compilation, pure synthetic tests, real WPF smoke and
desktop-only publish are complete. Actual owner reproduction of a
CODE.1 or CODE.2 managed error remains a separate GUI acceptance test.

**Note:** Creating a `Logs` folder improves incident accessibility,
but does not by itself resolve source-file opening or decoding bugs.
