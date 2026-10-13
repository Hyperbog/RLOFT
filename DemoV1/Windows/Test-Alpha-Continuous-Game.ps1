# Alpha normal-game diagnostic only. Never execute the incomplete
# native engine against the user's selected original installation.
# This is NOT the DOS game and is NOT an Alpha acceptance certificate.
$ErrorActionPreference = 'Stop'

function Fail-Alpha([string] $Reason) {
    Write-Host "ALPHA FIELD TEST FAILED: $Reason" -ForegroundColor Red
    exit 3
}

$package = Split-Path -Parent $MyInvocation.MyCommand.Path
$engine = Join-Path $package 'ravenloft_demo_v1.exe'
if (-not (Test-Path -LiteralPath $engine -PathType Leaf)) {
    Fail-Alpha 'The native executable is missing from this package.'
}
$identity = Join-Path $package 'build-source-sha.txt'
if (-not (Test-Path -LiteralPath $identity -PathType Leaf)) {
    Fail-Alpha 'Exact Windows build source provenance is missing.'
}
$sha = (Get-Content -LiteralPath $identity -Raw).Trim()
if ($sha -notmatch '^[0-9a-fA-F]{40}$') {
    Fail-Alpha 'Build source SHA is not a complete Git commit identifier.'
}

Add-Type -AssemblyName System.Windows.Forms
$folder = New-Object System.Windows.Forms.FolderBrowserDialog
$folder.Description = 'Choose your OWN original Ravenloft game installation. Only a temporary COPY will be used.'
$folder.ShowNewFolderButton = $false
if ($folder.ShowDialog() -ne [System.Windows.Forms.DialogResult]::OK) {
    Write-Host 'Alpha test cancelled before copying or running.'
    exit 2
}
$original = [System.IO.Path]::GetFullPath($folder.SelectedPath)
if (-not (Test-Path -LiteralPath $original -PathType Container)) {
    Fail-Alpha 'Selected original installation is not a directory.'
}

$choice = [System.Windows.Forms.MessageBox]::Show(
    'YES: Cold startup, hide CHARS ONLY IN THE TEMPORARY COPY.' +
    [Environment]::NewLine +
    'NO: Existing-party route, preserve CHARS in the copied folder.' +
    [Environment]::NewLine +
    'CANCEL: Do not start.',
    'Ravenloft Alpha - select original startup route',
    [System.Windows.Forms.MessageBoxButtons]::YesNoCancel,
    [System.Windows.Forms.MessageBoxIcon]::Information)
if ($choice -eq [System.Windows.Forms.DialogResult]::Cancel) {
    Write-Host 'Alpha test cancelled before copying or running.'
    exit 2
}

# Each test gets an isolated root; never reuse, overwrite or delete
# original installation paths. /XJ prevents directory junction traversal.
$workspaceRoot = Join-Path $env:TEMP 'RLOFT-Alpha-Field'
# Reject selecting the scratch root or any of its ancestors, including
# an entire drive; otherwise copying source->child can recursively
# copy the new temporary folder into itself or copy huge private data.
$sourcePrefix = [System.IO.Path]::GetFullPath($original).TrimEnd('\\') + '\\'
$workspacePath = [System.IO.Path]::GetFullPath($workspaceRoot).TrimEnd('\\')
if ($workspacePath.StartsWith($sourcePrefix, [StringComparison]::OrdinalIgnoreCase) -or
    $sourcePrefix.StartsWith($workspacePath + '\\', [StringComparison]::OrdinalIgnoreCase)) {
    Fail-Alpha 'The selected folder overlaps the Alpha temporary workspace.'
}
# Filesystem links/junctions could make the original media escape the
# selected tree. Do not follow any such links into the sandbox.
$linked = @(Get-ChildItem -LiteralPath $original -Force -Recurse -Attributes ReparsePoint -ErrorAction Stop | Select-Object -First 1)
if ($linked.Count -ne 0) {
    Fail-Alpha 'The original installation contains a reparse point; choose a regular copied directory.'
}
$runId = [Guid]::NewGuid().ToString('N')
$scratch = Join-Path $workspaceRoot $runId
[void](New-Item -ItemType Directory -Path $scratch -Force)
Write-Host "Copying original game files into isolated workspace: $scratch"
& robocopy.exe $original $scratch /E /XJ /R:1 /W:1 /COPY:DAT /DCOPY:DAT /NFL /NDL /NP /NJH /NJS | Out-Null
$copyStatus = $LASTEXITCODE
if ($copyStatus -ge 8) {
    Fail-Alpha "Isolated game-file copy failed (robocopy $copyStatus)."
}
if (-not (Test-Path -LiteralPath $scratch -PathType Container)) {
    Fail-Alpha 'Temporary working copy does not exist.'
}
# PowerShell can recurse copied directories; all deletion targets use
# the scratch root, never the selected original folder.
if ($choice -eq [System.Windows.Forms.DialogResult]::Yes) {
    $scratchCharacters = @(Get-ChildItem -LiteralPath $scratch -Filter 'CHARS' -File -Recurse -ErrorAction Stop)
    foreach ($entry in $scratchCharacters) {
        Remove-Item -LiteralPath $entry.FullName -Force -ErrorAction Stop
    }
    Write-Host "Cold-start test: removed $($scratchCharacters.Count) CHARS file(s) from COPY only."
} else {
    Write-Host 'Existing-party test: copied CHARS preserved when present.'
}

# DevSessionTrace is opt-in and writes diagnostics under TEMP. Record the
# previous session pointer so an earlier trace cannot be called success.
$traceRoot = Join-Path $env:TEMP 'RLOFT-Demo-Diagnostics'
$latestPointer = Join-Path $traceRoot 'latest-session.txt'
$previous = ''
if (Test-Path -LiteralPath $latestPointer -PathType Leaf) {
    $previous = (Get-Content -LiteralPath $latestPointer -Raw).Trim()
}
Write-Host "Native source SHA: $sha"
Write-Host "Temporary test installation: $scratch"
Write-Host 'Normal intro/party -> CODE.2 game; no developer warp.'
Write-Host 'F12: mark a frame after the first rendered gameplay scene.'
Write-Host 'The test may fail before creating a trace; preserve the first error.'
# No --choose-install: an explicit scratch path is passed to the engine.
# --dev-trace observes the NORMAL route, not --explore-level22.
$argv = '--dev-trace "' + $scratch + '"'
$process = Start-Process -FilePath $engine -ArgumentList $argv -WorkingDirectory $package -Wait -PassThru
$gameStatus = [int]$process.ExitCode

$session = ''
if (Test-Path -LiteralPath $latestPointer -PathType Leaf) {
    $session = (Get-Content -LiteralPath $latestPointer -Raw).Trim()
}
if ([string]::IsNullOrWhiteSpace($session) -or $session -eq $previous) {
    Write-Host 'No NEW diagnostic session. Record the exact startup blocker.'
} else {
    $rootPrefix = [System.IO.Path]::GetFullPath($traceRoot).TrimEnd('\') + '\'
    $sessionPath = [System.IO.Path]::GetFullPath($session)
    if (-not $sessionPath.StartsWith($rootPrefix, [StringComparison]::OrdinalIgnoreCase)) {
        Fail-Alpha 'Trace pointer escapes the local diagnostics root.'
    }
    $manifest = Join-Path $sessionPath 'manifest.json'
    if (-not (Test-Path -LiteralPath $manifest -PathType Leaf)) {
        Write-Host 'Trace session has no manifest; it is not field acceptance.'
    } else {
        $zip = $sessionPath + '.zip'
        Compress-Archive -LiteralPath $sessionPath -DestinationPath $zip -Force -ErrorAction Stop
        Write-Host "Diagnostic session ZIP (not uploaded): $zip"
        Write-Host 'Trace can include character names. Review before sharing.'
    }
}
Write-Host "Normal native game exit code: $gameStatus"
Write-Host "Isolated workspace (delete when finished): $scratch"
exit $gameStatus
