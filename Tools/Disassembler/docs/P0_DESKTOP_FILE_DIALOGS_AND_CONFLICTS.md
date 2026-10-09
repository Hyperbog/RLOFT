# Desktop file dialog and same-name file handling

**Checkpoint:** source committed to `disassembler-foundation`. **No
Windows compilation, regression execution or new distribution in this change.**

## Open Binary remembers the last folder

`FileDialogPreferences` persists only the directory of the most recently
successfully loaded source file, in the current user's local
`%LOCALAPPDATA%/DOSDis/desktop-preferences.json`. Subsequent Open Binary
dialogs start there, including after restarting DOSDis. Missing folders,
inaccessible/corrupt settings and preference-write failures safely fall
back to the operating system's default directory. This setting is local
and separate from project files and privacy-constrained diagnostic exports.
It does not store the original executable or a list of recently opened
file names. Game binaries remain read-only.

## Existing destination: Windows-style prompt, not fatal exception

The Save project, first-comment project creation, Backup project,
analysis JSON/text export, and diagnostic JSON export flows use the
same **Yes/No/Cancel** collision prompt, defaulting to **No**:

- **Yes — Replace:** only after an explicit user choice; the new project,
  backup or report is prepared in a unique temporary sibling file and
  published with atomic file replacement after success. For SQLite
  databases, an existing `-wal` or `-shm` sidecar blocks replacement
  to protect a potentially active research session. Windows may also
  refuse replacing a file that is open elsewhere; the previous file is
  preserved.
- **No — Keep both:** automatically chooses `name (1).ext`, then
  `name (2).ext`, and so on, skipping existing files/directories.
  Publishing with non-overwrite semantics also catches races with
  another process.
- **Cancel:** no new content is saved and an unsaved analyst comment
  remains in the editor.

A filename matching the currently opened original game binary is
**never** overwritten, even if the user navigates to it manually.
A backup may not overwrite its own open source project. These cases
are redirected to a numbered filename with an informational dialog.

Pre-existing research projects are not implicitly opened or mixed
with the current source; the analyst must explicitly select Open project
to resume existing notes. The project constructor validates source
identity and verified SQLite data before publishing a new file.

## Export atomicity

`ProjectFilePaths.PublishNewOrReplace` retains the JSON/TXT extension
of staging paths because `AnalysisReportExport.WriteNew` verifies
extensions. Export failures preserve prior complete report contents.
`ProjectStore.CreateBackup` preserves its safe default refusal to
overwrite; only a separately confirmed call can set `overwrite:true`.

Existing diagnostics remain source-path-free; local folder preferences
are not exported by the diagnostics subsystem.

## Regression tests added (source-only)

`DosDis.ProjectSmoke` covers saved/reloaded preferences and malformed
settings, suffix progression and preservation of earlier files,
staged new project publication, rejection of implicit overwrite,
explicit project replacement, verified backup replacement,
SQLite journal protection, and atomic report failure recovery.

These source tests have **not** run in the current checkpoint.
Manual UI acceptance still needs the Save As interactions, nested
modal dismissal, and whether the app stays open after Cancel/Keep both
and after a failed write. The previously observed error-dialog exit
has not been reproduced independently; removing normal filename
collisions from the error path is a targeted repair, not proof of
the absence of other application crashes.
