# Pre-Alpha field runner — preserve preferred original installation

**Branch:** `demo-v1`. **Scope:** Win32 diagnostic host/bookkeeping,
not original DOS gameplay or a release gate advancement.

## Failure found during startup artifact audit

The intentionally isolated PowerShell field launcher copies the
user-owned game to a new
`%TEMP%\\RLOFT-Alpha-Field\\<GUID>` directory and invokes
`ravenloft_demo_v1.exe --dev-trace "<scratch>"`.
Prior `run()` unconditionally called
`save_preferred_install(source.location().install_root)` for
**any explicit source path**, including the temporary test copy.
As a result, a successful diagnostic launch silently replaced
the normal saved `HKCU\\Software\\Hyperbog\\RLOFT Demo V1`
installation preference with a disposable directory. Later
normal startup could therefore prefer a deleted scratch folder.

## Bounded repair

Only an **explicit diagnostic source** leaves the saved
preferred-install record untouched. Ordinary non-diagnostic
explicit launch still saves the selected game folder unchanged,
and normal remembered-folder/auto-location/chooser paths are
unaltered. No original game assets or saves are touched.

The existing `alpha_sandbox_normal_launch_contract_tests`
CI source guard now checks this boundary and detects a sixth
intentional bad mutation that re-enables writing the temporary
scratch path to the persistent registry.

This is host-only correctness; the actual cold-start Windows
session and DOS behavior/pixels remain unaccepted.
