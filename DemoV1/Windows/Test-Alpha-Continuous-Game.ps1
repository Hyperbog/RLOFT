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

# Keep the source revision ID separate from binary-byte integrity.
# Refuse a repackaged/stale EXE before asking for any owned media.
$hashFile = Join-Path $package 'ravenloft_demo_v1.exe.sha256'
if (-not (Test-Path -LiteralPath $hashFile -PathType Leaf)) {
    Fail-Alpha 'Exact packaged executable SHA-256 is missing.'
}
$hashRecord = (Get-Content -LiteralPath $hashFile -Raw).Trim()
if ($hashRecord -cnotmatch '^([0-9a-fA-F]{64})  ravenloft_demo_v1\.exe$') {
    Fail-Alpha 'Executable checksum manifest is malformed.'
}
$expectedExeHash = $Matches[1]
$actualExeHash = (Get-FileHash -LiteralPath $engine -Algorithm SHA256).Hash
if (-not $actualExeHash.Equals($expectedExeHash, [StringComparison]::OrdinalIgnoreCase)) {
    Fail-Alpha 'The packaged native executable does not match its SHA-256 manifest.'
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
# A selected root can itself be a junction or symlink. Checking only
# descendant reparse points would not detect that escape path.
$rootEntry = Get-Item -LiteralPath $original -Force -ErrorAction Stop
if (($rootEntry.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
    Fail-Alpha 'Selected original installation root is a filesystem link; choose its real directory.'
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
$sep = [System.IO.Path]::DirectorySeparatorChar
$sourcePrefix = [System.IO.Path]::GetFullPath($original).TrimEnd($sep) + $sep
$workspacePath = [System.IO.Path]::GetFullPath($workspaceRoot).TrimEnd($sep)
if ($workspacePath.StartsWith($sourcePrefix, [StringComparison]::OrdinalIgnoreCase) -or
    $sourcePrefix.StartsWith($workspacePath + $sep, [StringComparison]::OrdinalIgnoreCase)) {
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
    # GameSource::runtime_file("CHARS") only checks install_root and its
    # RLOFT child. A CHARS elsewhere cannot enter the existing-party path.
    # Fail before launch rather than silently mislabelling cold startup.
    $partyRoot = Join-Path $scratch 'CHARS'
    $partyChild = Join-Path (Join-Path $scratch 'RLOFT') 'CHARS'
    $partyFile = $null
    foreach ($candidate in @($partyRoot, $partyChild)) {
        if (Test-Path -LiteralPath $candidate -PathType Leaf) {
            $partyFile = $candidate
            break
        }
    }
    if ($null -eq $partyFile) {
        Fail-Alpha 'Existing-party route requires CHARS in the copied game root or RLOFT child. Use cold startup otherwise.'
    }
    # Original CODE.2 16CD0 reads exactly 0x920 bytes. Do not print
    # character contents or modify the copied save.
    $partySize = (Get-Item -LiteralPath $partyFile -ErrorAction Stop).Length
    if ($partySize -ne 0x920) {
        Fail-Alpha 'Copied existing-party CHARS has an invalid size (expected 0x920 bytes).'
    }
    Write-Host 'Existing-party test: valid-size copied CHARS preserved.'
}

# DevSessionTrace is opt-in and writes diagnostics under TEMP. Record the
# previous session pointer so an earlier trace cannot be called success.
$traceRoot = Join-Path $env:TEMP 'RLOFT-Demo-Diagnostics'
$latestPointer = Join-Path $traceRoot 'latest-session.txt'
$latestBootPointer = Join-Path $traceRoot 'latest-boot-session.txt'
$previous = ''
$previousBoot = ''
if (Test-Path -LiteralPath $latestPointer -PathType Leaf) {
    $previous = (Get-Content -LiteralPath $latestPointer -Raw).Trim()
}
if (Test-Path -LiteralPath $latestBootPointer -PathType Leaf) {
    $previousBoot = (Get-Content -LiteralPath $latestBootPointer -Raw).Trim()
}
Write-Host "Native source SHA: $sha"
Write-Host "Temporary test installation: $scratch"
Write-Host 'Normal intro/party -> CODE.2 game; no developer warp.'
Write-Host 'F12: mark a frame after the first rendered gameplay scene.'
Write-Host 'Startup boot trace begins before CINE00/title/character creation.'
Write-Host 'Early installation/window errors may still precede trace creation.'
# No --choose-install: an explicit scratch path is passed to the engine.
# --dev-trace observes the NORMAL route, not --explore-level22.
$argv = '--dev-trace "' + $scratch + '"'
$process = Start-Process -FilePath $engine -ArgumentList $argv -WorkingDirectory $package -Wait -PassThru
$gameStatus = [int]$process.ExitCode

$session = ''
if (Test-Path -LiteralPath $latestPointer -PathType Leaf) {
    $session = (Get-Content -LiteralPath $latestPointer -Raw).Trim()
}
# Collect the opening sequence independently of gameplay. Previously
# only the exploration session existed, so a CINE/title/CODE.1 failure
# produced no packageable evidence at all.
$bootSession = ''
if (Test-Path -LiteralPath $latestBootPointer -PathType Leaf) {
    $bootSession = (Get-Content -LiteralPath $latestBootPointer -Raw).Trim()
}
if ([string]::IsNullOrWhiteSpace($bootSession) -or $bootSession -eq $previousBoot) {
    Write-Host 'No NEW startup trace; check pre-window installation/host errors.'
} else {
    $rootPrefix = [System.IO.Path]::GetFullPath($traceRoot).TrimEnd($sep) + $sep
    $bootPath = [System.IO.Path]::GetFullPath($bootSession)
    # Shared TEMP pointers can be replaced by another test process.
    # Refuse to label an unrelated process's trace as this field run.
    $expectedBootSuffix = '-boot-' + [string]$process.Id
    if (-not [System.IO.Path]::GetFileName($bootPath).EndsWith($expectedBootSuffix, [StringComparison]::Ordinal)) {
        Fail-Alpha 'Startup trace belongs to a different native process.'
    }
    if (-not $bootPath.StartsWith($rootPrefix, [StringComparison]::OrdinalIgnoreCase)) {
        Fail-Alpha 'Startup trace pointer escapes the local diagnostics root.'
    }
    $bootManifest = Join-Path $bootPath 'manifest.json'
    if (-not (Test-Path -LiteralPath $bootManifest -PathType Leaf)) {
        Write-Host 'Startup trace has no manifest; it is not field acceptance.'
    } else {
        $bootIdentity = Get-Content -LiteralPath $bootManifest -Raw | ConvertFrom-Json -ErrorAction Stop
        if (-not ([string]$bootIdentity.source_revision).Equals($sha, [StringComparison]::OrdinalIgnoreCase)) {
            Fail-Alpha 'Startup trace revision does not match this packaged native build.'
        }
        $bootZip = $bootPath + '.zip'
        Compress-Archive -LiteralPath $bootPath -DestinationPath $bootZip -Force -ErrorAction Stop
        Write-Host "Startup-stage diagnostic ZIP (not uploaded): $bootZip"
        Write-Host 'This contains stage IDs and input metadata, not original game files.'
    }
}
if ([string]::IsNullOrWhiteSpace($session) -or $session -eq $previous) {
    Write-Host 'No NEW exploration trace. The startup ZIP can still show the first blocker.'
} else {
    $rootPrefix = [System.IO.Path]::GetFullPath($traceRoot).TrimEnd($sep) + $sep
    $sessionPath = [System.IO.Path]::GetFullPath($session)
    $expectedSessionSuffix = '-' + [string]$process.Id
    if (-not [System.IO.Path]::GetFileName($sessionPath).EndsWith($expectedSessionSuffix, [StringComparison]::Ordinal)) {
        Fail-Alpha 'Exploration trace belongs to a different native process.'
    }
    if (-not $sessionPath.StartsWith($rootPrefix, [StringComparison]::OrdinalIgnoreCase)) {
        Fail-Alpha 'Trace pointer escapes the local diagnostics root.'
    }
    $manifest = Join-Path $sessionPath 'manifest.json'
    if (-not (Test-Path -LiteralPath $manifest -PathType Leaf)) {
        Write-Host 'Trace session has no manifest; it is not field acceptance.'
    } else {
        $sessionIdentity = Get-Content -LiteralPath $manifest -Raw | ConvertFrom-Json -ErrorAction Stop
        if (-not ([string]$sessionIdentity.source_revision).Equals($sha, [StringComparison]::OrdinalIgnoreCase)) {
            Fail-Alpha 'Exploration trace revision does not match this packaged native build.'
        }
        # Report only whitelisted symbolic stage IDs, never raw event
        # JSON or keystrokes. A ready presenter is NOT a rendered frame.
        $preloadStages = @(
            'exploration.load.begin',
            'exploration.load.failed',
            'exploration.load.ready',
            'exploration.world.failed',
            'exploration.world.ready',
            'exploration.presenters.ready',
            'exploration.frame.build.begin',
            'exploration.frame.build.failed',
            'exploration.frame.build.ready',
            'exploration.frame.raster.failed',
            'exploration.frame.raster.ready',
            'exploration.frame.present.submitted'
        )
        $events = Join-Path $sessionPath 'events.jsonl'
        if (Test-Path -LiteralPath $events -PathType Leaf) {
            $lastPreload = Get-Content -LiteralPath $events | ForEach-Object {
                try {
                    $entry = $_ | ConvertFrom-Json -ErrorAction Stop
                    if ($preloadStages -contains [string]$entry.event) {
                        [string]$entry.event
                    }
                } catch {
                    # An incomplete final JSON line is not a valid stage.
                }
            } | Select-Object -Last 1
            if ($lastPreload) {
                Write-Host "Last exploration initialization checkpoint: $lastPreload"
                Write-Host 'Host submission is not visual verification or original DOS frame parity.'
            }
        }
        $zip = $sessionPath + '.zip'
        Compress-Archive -LiteralPath $sessionPath -DestinationPath $zip -Force -ErrorAction Stop
        Write-Host "Diagnostic session ZIP (not uploaded): $zip"
        Write-Host 'Trace can include character names. Review before sharing.'
    }
}
Write-Host "Normal native game exit code: $gameStatus"
Write-Host "Isolated workspace (delete when finished): $scratch"
exit $gameStatus
