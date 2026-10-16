# Continuous native PRE-ALPHA startup (NOT original DOS execution).
# Reuses normal CODE.2/1 cold path; no preview warp, trace, UI chooser,
# or overwriting the user's original installed game.
$ErrorActionPreference = 'Stop'

function Fail-ContinuousAlpha([string] $Reason) {
    Write-Host ('CONTINUOUS ALPHA: ' + $Reason) -ForegroundColor Red
    exit 3
}

$package = Split-Path -Parent $MyInvocation.MyCommand.Path
$engine = Join-Path $package 'Ravenloft.exe'
$hashFile = Join-Path $package 'Ravenloft.exe.sha256'
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
# Positive match always provides capture group 1 on Windows PowerShell.
if ($hashLine -cmatch '^([0-9a-fA-F]{64})  ravenloft_demo_v1\.exe$') {
    $expected = $Matches[1]
} else {
    Fail-ContinuousAlpha 'Executable hash manifest is malformed.'
}
$actual = (Get-FileHash -LiteralPath $engine -Algorithm SHA256).Hash
if (-not $actual.Equals($expected, [StringComparison]::OrdinalIgnoreCase)) {
    Fail-ContinuousAlpha 'Released native executable SHA-256 mismatch.'
}

# No folder chooser or manual path selection. Prefer a complete owned
# Game/ installation; otherwise try common GOG/Steam install locations.
# Work ONLY on a fresh temp copy -- original saves are never modified.
function Test-CompleteLooseGame([string] $folder) {
    if (-not (Test-Path -LiteralPath $folder -PathType Container)) { return $false }
    # The intro requires CINE/RES10 media, not just the partial DOS
    # installed RLOFT folder (which may have RES0/INIT/T but no movies).
    foreach ($name in @('RES0','INIT','T','CODE.1','CODE.2',
                        'CINE00.ANM','CINE12.ANM','RES10')) {
        if (-not (Test-Path -LiteralPath (Join-Path $folder $name) -PathType Leaf)) {
            return $false
        }
    }
    return $true
}

function Find-OwnedGameMedia([string] $root) {
    if ([string]::IsNullOrWhiteSpace($root) -or
        -not (Test-Path -LiteralPath $root -PathType Container)) {
        return $null
    }
    $base = [System.IO.Path]::GetFullPath($root)
    # Only bounded, known layouts: do not recursively search whole drives.
    $layouts = @(
        '.',
        'RLOFT',
        'RLOFT\RLOFT',
        'CD_Extracted\RLOFT',
        'Game',
        'Game\RLOFT',
        'Game\CD_Extracted\RLOFT',
        "Ravenloft - Strahd's Possession\RLOFT",
        "Ravenloft - Strahd's Possession\CD_Extracted\RLOFT",
        "MASTER\Ravenloft - Strahd's Possession\CD_Extracted\RLOFT"
    )
    foreach ($relative in $layouts) {
        $candidate = Join-Path $base $relative
        if (-not (Test-Path -LiteralPath $candidate -PathType Container)) { continue }
        $entry = Get-Item -LiteralPath $candidate -Force -ErrorAction Stop
        if (($entry.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
            continue
        }
        if ((Test-CompleteLooseGame $candidate) -or
            (Test-Path -LiteralPath (Join-Path $candidate 'game.gog') -PathType Leaf)) {
            return [System.IO.Path]::GetFullPath($candidate)
        }
    }
    return $null
}

$original = Join-Path $package 'Game'
$found = Find-OwnedGameMedia $original
if (-not $found) {
    $candidates = New-Object 'System.Collections.Generic.List[string]'
    if ($env:RLOFT_GAME_DIR) { $candidates.Add($env:RLOFT_GAME_DIR) }
    if ($env:GOG_GAMES) { $candidates.Add($env:GOG_GAMES) }

    # GOG product 1432905053: read both registry views without changing them.
    foreach ($hive in @('HKEY_CURRENT_USER','HKEY_LOCAL_MACHINE')) {
        foreach ($view in @('SOFTWARE\GOG.com\Games\1432905053',
                             'SOFTWARE\WOW6432Node\GOG.com\Games\1432905053')) {
            try {
                $key = Get-ItemProperty -LiteralPath ('Registry::' + $hive + '\' + $view) -ErrorAction Stop
                if ($key.path) { $candidates.Add([string]$key.path) }
            } catch { }
        }
    }

    $parents = New-Object 'System.Collections.Generic.List[string]'
    $parents.Add('C:\GOG Games')
    if ($env:USERPROFILE) { $parents.Add((Join-Path $env:USERPROFILE 'GOG Games')) }
    foreach ($pf in @([Environment]::GetEnvironmentVariable('ProgramFiles'),
                       [Environment]::GetEnvironmentVariable('ProgramFiles(x86)'))) {
        if (-not [string]::IsNullOrWhiteSpace($pf)) {
            $parents.Add((Join-Path $pf 'GOG Galaxy\Games'))
            $parents.Add((Join-Path $pf 'Steam\steamapps\common'))
        }
    }
    foreach ($parent in $parents) {
        foreach ($name in @("Ravenloft - Strahd's Possession",
                            "Ravenloft Strahd's Possession")) {
            $candidates.Add((Join-Path $parent $name))
        }
    }
    foreach ($candidate in $candidates) {
        $found = Find-OwnedGameMedia $candidate
        if ($found) { break }
    }
}

if (-not $found) {
    Write-Host ('Checked: ' + $original)
    Write-Host 'For a FULL cinematic start, Game must contain game.gog OR the'
    Write-Host 'files from MASTER\Ravenloft - Strahd''s Possession\CD_Extracted\RLOFT.'
    Write-Host 'Those include RES0, INIT, T, CODE.1, CODE.2, CINE00.ANM,'
    Write-Host 'CINE12.ANM and RES10. A partial installed RLOFT folder is insufficient.'
    Write-Host 'Alternatively set RLOFT_GAME_DIR to your owned GOG game install.'
    Fail-ContinuousAlpha 'Complete original movie/game resources were not found.'
}
$original = $found
Write-Host ('Using owned game resources: ' + $original)

$originalEntry = Get-Item -LiteralPath $original -Force -ErrorAction Stop
if (($originalEntry.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
    Fail-ContinuousAlpha 'Original media must not be a filesystem link or junction.'
}
$hasRootMedia = Test-CompleteLooseGame $original
$nested = Join-Path $original 'RLOFT'
$hasNestedMedia = Test-CompleteLooseGame $nested
$hasGogRoot = Test-Path -LiteralPath (Join-Path $original 'game.gog') -PathType Leaf
$hasGogNested = Test-Path -LiteralPath (Join-Path $nested 'game.gog') -PathType Leaf
if (-not $hasRootMedia -and -not $hasNestedMedia -and
    -not $hasGogRoot -and -not $hasGogNested) {
    Fail-ContinuousAlpha 'No complete loose RES/CINE installation or game.gog found.'
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
