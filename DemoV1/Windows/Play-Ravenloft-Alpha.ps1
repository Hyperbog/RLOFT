# Continuous native PRE-ALPHA startup (NOT original DOS execution).
# Reuses normal CODE.2/1 cold path; no preview warp, trace, UI chooser,
# or overwriting the user's original installed game.
$ErrorActionPreference = 'Stop'

function Fail-ContinuousAlpha([string] $Reason) {
    Write-Host ('CONTINUOUS ALPHA: ' + $Reason) -ForegroundColor Red
    exit 3
}

$package = Split-Path -Parent $MyInvocation.MyCommand.Path
$engine = Join-Path $package 'ravenloft_demo_v1.exe'
$hashFile = Join-Path $package 'ravenloft_demo_v1.exe.sha256'
$sourceFile = Join-Path $package 'build-source-sha.txt'

# Verify exact released exe bytes before opening the owner's game data.
if (-not (Test-Path -LiteralPath $engine -PathType Leaf)) {
    Fail-ContinuousAlpha 'Executable missing. Download the full Alpha artifact.'
}
if (-not (Test-Path -LiteralPath $hashFile -PathType Leaf)) {
    Fail-ContinuousAlpha 'Executable SHA-256 manifest missing.'
}
if (-not (Test-Path -LiteralPath $sourceFile -PathType Leaf)) {
    Fail-ContinuousAlpha 'Compiled source revision file missing.'
}
$sourceSha = (Get-Content -LiteralPath $sourceFile -Raw).Trim()
if ($sourceSha -cnotmatch '^[0-9a-fA-F]{40}$') {
    Fail-ContinuousAlpha 'Compiled source revision is malformed.'
}
$hashLine = (Get-Content -LiteralPath $hashFile -Raw).Trim()
if ($hashLine -cnotmatch '^([0-9a-fA-F]{64})  ravenloft_demo_v1\.exe$') {
    Fail-ContinuousAlpha 'Executable hash manifest is malformed.'
}
$expected = $Matches[1]
$actual = (Get-FileHash -LiteralPath $engine -Algorithm SHA256).Hash
if (-not $actual.Equals($expected, [StringComparison]::OrdinalIgnoreCase)) {
    Fail-ContinuousAlpha 'Released native executable SHA-256 mismatch.'
}

# The ONE initial setup: copy owned DOS installation into Game/ ONCE.
# All later launches are single-click, no folder/browser or mode prompt.
$original = Join-Path $package 'Game'
if (-not (Test-Path -LiteralPath $original -PathType Container)) {
    Fail-ContinuousAlpha 'Copy original installed game files into the Game folder beside the launcher.'
}
$original = [System.IO.Path]::GetFullPath($original)
$originalEntry = Get-Item -LiteralPath $original -Force -ErrorAction Stop
if (($originalEntry.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
    Fail-ContinuousAlpha 'Game folder must not be a filesystem link or junction.'
}
$hasRootMedia = (Test-Path -LiteralPath (Join-Path $original 'RES0') -PathType Leaf) -and
    (Test-Path -LiteralPath (Join-Path $original 'INIT') -PathType Leaf) -and
    (Test-Path -LiteralPath (Join-Path $original 'T') -PathType Leaf)
$nested = Join-Path $original 'RLOFT'
$hasNestedMedia = (Test-Path -LiteralPath (Join-Path $nested 'RES0') -PathType Leaf) -and
    (Test-Path -LiteralPath (Join-Path $nested 'INIT') -PathType Leaf) -and
    (Test-Path -LiteralPath (Join-Path $nested 'T') -PathType Leaf)
$hasGogRoot = Test-Path -LiteralPath (Join-Path $original 'game.gog') -PathType Leaf
$hasGogNested = Test-Path -LiteralPath (Join-Path $nested 'game.gog') -PathType Leaf
if (-not $hasRootMedia -and -not $hasNestedMedia -and
    -not $hasGogRoot -and -not $hasGogNested) {
    Fail-ContinuousAlpha 'Original loose RES0/INIT/T or game.gog must be inside Game or Game\RLOFT.'
}
$links = @(Get-ChildItem -LiteralPath $original -Force -Recurse -Attributes ReparsePoint -ErrorAction Stop | Select-Object -First 1)
if ($links.Count -ne 0) {
    Fail-ContinuousAlpha 'Original media tree contains symlinks or junctions. Use real files.'
}

$workspaceRoot = Join-Path $env:TEMP 'RLOFT-Continuous-Alpha'
$sep = [System.IO.Path]::DirectorySeparatorChar
$sourcePrefix = $original.TrimEnd($sep) + $sep
$workspaceFull = [System.IO.Path]::GetFullPath($workspaceRoot).TrimEnd($sep)
if ($workspaceFull.StartsWith($sourcePrefix, [StringComparison]::OrdinalIgnoreCase) -or
    $sourcePrefix.StartsWith($workspaceFull + $sep, [StringComparison]::OrdinalIgnoreCase)) {
    Fail-ContinuousAlpha 'Original Game folder overlaps disposable workspace.'
}
$runId = [Guid]::NewGuid().ToString('N')
$scratch = Join-Path $workspaceRoot $runId
[void](New-Item -Path $scratch -ItemType Directory -Force)
Write-Host ('Copying original game data to temporary play area: ' + $scratch)
& robocopy.exe $original $scratch /E /XJ /R:1 /W:1 /COPY:DAT /DCOPY:DAT /NFL /NDL /NP /NJH /NJS | Out-Null
$copyStatus = $LASTEXITCODE
if ($copyStatus -ge 8) {
    Fail-ContinuousAlpha "Temporary game-data copy failed (robocopy $copyStatus)."
}

# Original CODE.2 tests CHARS before intro. Force cold start only
# in the temporary copy so the full established audiovisual front-end
# always runs. Real installation, saved parties, and saves are untouched.
# GameSource::runtime_file("CHARS") probes only these two roots.
# Do NOT remove same-named files from SAVE## snapshots or other folders.
$charsCandidates = @(
    (Join-Path $scratch 'CHARS'),
    (Join-Path (Join-Path $scratch 'RLOFT') 'CHARS')
)
foreach ($candidate in $charsCandidates) {
    if (Test-Path -LiteralPath $candidate -PathType Leaf) {
        Remove-Item -LiteralPath $candidate -Force -ErrorAction Stop
    }
}
Write-Host ('Native build source: ' + $sourceSha.Substring(0,12))
Write-Host 'Launching original intro, logos, title, character creation, then Level 22.'
# No --front-end-only, --explore-level22, --dev-trace or other dev flags.
# Loose media in Game\RLOFT must use THAT actual directory:
# GameSource::open() accepts loose media at the passed path only.
# It can discover nested game.gog images automatically.
$launchMediaRoot = $scratch
if (-not $hasRootMedia -and $hasNestedMedia) {
    $launchMediaRoot = Join-Path $scratch 'RLOFT'
}
# This host-only flag prevents the temporary path from replacing the
# user's preferred original media location in the Windows registry.
$arguments = '--no-remember-install "' + $launchMediaRoot + '"'
$process = Start-Process -FilePath $engine -ArgumentList $arguments -WorkingDirectory $package -Wait -PassThru
Write-Host ('Native exit code: ' + [string]$process.ExitCode)
Write-Host ('Temporary created-party data is at: ' + $scratch)
Write-Host 'Original Game data was not modified.'
exit [int]$process.ExitCode
