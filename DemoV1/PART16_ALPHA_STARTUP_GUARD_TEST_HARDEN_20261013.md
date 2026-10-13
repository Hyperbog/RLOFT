# Follow-up CI guard correction

Prior source-only first-logo regression had two independent defects:

1. A too-broad `rfind(if (!skipped))` could accept CINE12's
   preceding guard after the actual first-logo guard was removed,
   invalidating the negative mutation.
2. The negative-test main referenced `intro12`, a local variable
   belonging to `check()`, which raises `NameError` when executed.

The corrected check matches the **first-logo block's own source
guard, optional stage marker and original 120-tick deadline
in order**, and the negative-test main resolves its own
CINE12 search index. The change is limited to source-only test
logic; no CODE.2 game implementation or Win32 startup logic
was altered.

CI results for this exact source commit are required before
claiming the startup test set passes.
