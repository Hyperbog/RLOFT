# One-click native RLOFT source-boot and saved-world VISUAL PREVIEW.
# The staged startup is not a playable DOS boot. Only user-owned original
# files are opened; no original content is embedded in the downloaded build.
[CmdletBinding()]
param(
    [string]$GameFolder = "D:\GOG Games\Ravenloft - Strahd's Possession\RLOFT",
    [string]$SaveFolder = "",
    [switch]$SkipPreview
)
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$game = [IO.Path]::GetFullPath($GameFolder)
$gogParent = Split-Path -Path $game -Parent
$cdImage = Join-Path $gogParent 'game.gog'
$bin = $PSScriptRoot

$requiredExe = @(
    'rloft_code1_startup_gate.exe',
    'rloft_code2_startup_gate.exe',
    'rloft_code1_code2_staged_boot.exe',
    'rloft_gog_cd_extract.exe',
    'rloft_original_code2_world_frame.exe'
)
foreach ($name in $requiredExe) {
    $path=Join-Path $bin $name
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing compiled component: $path. Extract the COMPLETE fresh build."
    }
}
if (-not (Test-Path -LiteralPath $game -PathType Container)) {
    throw "Cannot find the original GOG RLOFT installation: $game"
}
foreach ($name in @('CODE.1','CODE.2','COLORS','INIT','INFO.DAT',
                    'RES0','RES1','RES3','RES4','SINTAB',
                    'NPCS.DAT','OBJECTS.DAT')) {
    if (-not (Test-Path -LiteralPath (Join-Path $game $name) -PathType Leaf)) {
        throw "Missing original $name in $game."
    }
}
$documents = [Environment]::GetFolderPath('MyDocuments')
if ([string]::IsNullOrWhiteSpace($documents)) {
    throw 'Windows Documents directory is unavailable.'
}
$session = Join-Path (Join-Path $documents 'RLOFT-Recomp-Preview') ([DateTime]::UtcNow.ToString('yyyyMMddTHHmmssfffZ') + "-$PID")
New-Item -ItemType Directory -Force -Path $session | Out-Null
$cdRoot = Join-Path $session 'private-cd-resources'
$results = New-Object System.Collections.Generic.List[string]
$results.Add("RLOFT source-build experiment - NOT a playable boot.")
$results.Add("Original installation: $game")
$results.Add("Owner CD image: $cdImage")
$results.Add("Output: $session")
$results.Add("CODE1_TO_CODE2_GUEST_TRANSFER=NOT_IMPLEMENTED")
$results.Add("CONTINUOUS_GAMEPLAY_LOOP=NOT_IMPLEMENTED")

function Invoke-Logged([string]$exe, [string[]]$argv, [string]$log) {
    # PowerShell 5.1 may promote native stderr to NativeCommandError.
    # Retain the real native process exit code in every situation.
    $old = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    try {
        & $exe @argv 2>&1 | Out-File -LiteralPath $log -Encoding utf8
        $nativeCode = $LASTEXITCODE
    }
    finally {
        $ErrorActionPreference = $old
    }
    return [int]$nativeCode
}

Write-Host ""
Write-Host "RAVENLOFT NATIVE SOURCE BUILD - BOOT CHECK + VISUAL PREVIEW"
Write-Host "Game:    $game"
Write-Host "Output:  $session"
Write-Host "This is an engineering preview, not an interactive game."
Write-Host ""

# Always run at least CODE.1 and CODE.2 reconstructed-source startup
# probes, even if the CD image is unavailable/unreadable.
$code1Log=Join-Path $session 'code1_source_startup.log'
$code1Exe=Join-Path $bin 'rloft_code1_startup_gate.exe'
$code1Result=Invoke-Logged $code1Exe @(
    '--extender-host-resize-to-cd97-constructors',
    (Join-Path $game 'CODE.1')) $code1Log
$results.Add("CODE1_ORIGINAL_CONSTRUCTORS_EXIT=$code1Result")
Write-Host "Original CODE.1 constructor checkpoint: exit $code1Result"
if($code1Result -ne 0) {
    Write-Warning "CODE.1 diagnostic stopped. See $code1Log"
}

$code2Log=Join-Path $session 'code2_source_startup.log'
$code2Exe=Join-Path $bin 'rloft_code2_startup_gate.exe'
$code2Result=Invoke-Logged $code2Exe @(
    '--enter-game',(Join-Path $game 'CODE.2')) $code2Log
$results.Add("CODE2_BOUNDED_SOURCE_ENTRY_EXIT=$code2Result")
$results.Add("CODE2_DOS_ENVIRONMENT=EXPLICIT_TEST_FIXTURE")
Write-Host "Original CODE.2 bounded entry checkpoint: exit $code2Result"

if(Test-Path -LiteralPath $cdImage -PathType Leaf) {
    $extractLog=Join-Path $session 'gog_disc_extraction.log'
    $extractExe=Join-Path $bin 'rloft_gog_cd_extract.exe'
    Write-Host "Reading original game.gog CD resources locally..."
    $cdResult=Invoke-Logged $extractExe @('--image',$cdImage,'--out',$cdRoot) $extractLog
    $results.Add("CD_RESOURCE_EXTRACT_EXIT=$cdResult")
    if($cdResult -eq 0) {
        $stageLog=Join-Path $session 'code1_code2_source_staged_boot.log'
        $stageExe=Join-Path $bin 'rloft_code1_code2_staged_boot.exe'
        Write-Host "Executing staged source boot with original INIT/INFO/RES/COLORS..."
        $stageResult=Invoke-Logged $stageExe @(
            '--root',$game,'--cd-root',$cdRoot) $stageLog
        $results.Add("CODE1_CODE2_STAGED_BOOT_EXIT=$stageResult")
        Write-Host "Staged startup/palette checkpoint: exit $stageResult"
        if($stageResult -ne 0){Write-Warning "Staged source boot failed; see $stageLog"}
    } else {
        $results.Add("CODE1_CODE2_STAGED_BOOT=SKIPPED_UNSUPPORTED_OR_CORRUPT_DISC_LAYOUT")
        Write-Warning "CD extraction unavailable. Check $extractLog. The renderer can still run."
    }
} else {
    $results.Add("CODE1_CODE2_STAGED_BOOT=SKIPPED_GAME_GOG_MISSING")
    Write-Warning "game.gog not found: $cdImage. CODE.1/2 startup probes ran separately."
}
# Clean up 54 MB or more of copied commercial media, even on failure.
if(Test-Path -LiteralPath $cdRoot -PathType Container) {
    Remove-Item -LiteralPath $cdRoot -Recurse -Force
}

if(-not $SkipPreview) {
    # Prefer the exact Level-5 SAVE09 known to pass the original source
    # 36CEC skeleton world renderer. An arbitrary DOSBox .sav is NOT an
    # equivalent input to the game's SAVE.DAT.
    $saveCandidates = New-Object System.Collections.Generic.List[string]
    if($SaveFolder) { $saveCandidates.Add($SaveFolder) }
    else {
        foreach($path in @(
            (Join-Path $gogParent 'cloud_saves\RLOFT\SAVE09'),
            (Join-Path $gogParent 'cloud_saves\RLOFT\SAVE10'),
            (Join-Path $gogParent 'cloud_saves\RLOFT\SAVE03'),
            (Join-Path $gogParent 'cloud_saves\RLOFT\SAVE04'),
            (Join-Path $gogParent 'cloud_saves\RLOFT\SAVE08'),
            (Join-Path $game 'SAVE09'),
            (Join-Path $game 'SAVE10'),
            $game
        )) { $saveCandidates.Add($path) }
    }
    $foundSave=$null
    foreach($candidate in $saveCandidates) {
        if((Test-Path -LiteralPath (Join-Path $candidate 'SAVE.DAT') -PathType Leaf) -and
           (Test-Path -LiteralPath (Join-Path $candidate 'LEV005.DAT') -PathType Leaf)) {
            $foundSave=$candidate
            break
        }
    }
    if(-not $foundSave) {
        Write-Host ""
        Write-Host "No Level 5 original SAVE.DAT + LEV005.DAT pair found automatically."
        Write-Host "Select the folder containing BOTH original save files."
        Add-Type -AssemblyName System.Windows.Forms
        $folderPicker=New-Object System.Windows.Forms.FolderBrowserDialog
        $folderPicker.Description='Choose original Ravenloft SAVE.DAT + LEV005.DAT folder (not a DOSBox .sav)'
        if($folderPicker.ShowDialog() -eq [System.Windows.Forms.DialogResult]::OK) {
            $candidate=$folderPicker.SelectedPath
            if((Test-Path -LiteralPath (Join-Path $candidate 'SAVE.DAT') -PathType Leaf) -and
               (Test-Path -LiteralPath (Join-Path $candidate 'LEV005.DAT') -PathType Leaf)) {
                $foundSave=$candidate
            }
        }
    }
    if($foundSave) {
        $outStem=Join-Path $session 'ravenloft_native_level5'
        $worldExe=Join-Path $bin 'rloft_original_code2_world_frame.exe'
        $worldLog=Join-Path $session 'native_scene_renderer.log'
        Write-Host "Rendering original-save Level 5 scene (may take several seconds)..."
        $renderResult=Invoke-Logged $worldExe @(
            '--save-complete-frame',
            (Join-Path $foundSave 'SAVE.DAT'),
            (Join-Path $foundSave 'LEV005.DAT'),
            (Join-Path $game 'SINTAB'),
            (Join-Path $game 'RES4'),
            (Join-Path $game 'COLORS'),
            (Join-Path $game 'CODE.2'),
            (Join-Path $game 'INIT'),
            (Join-Path $game 'RES0'),
            (Join-Path $game 'NPCS.DAT'),
            (Join-Path $game 'OBJECTS.DAT'),
            (Join-Path $game 'RES3'),
            $outStem,
            (Join-Path $game 'RES1')
        ) $worldLog
        $results.Add("NATIVE_SAVED_SCENE_EXIT=$renderResult")
        $picture=$outStem+'.bmp'
        if($renderResult -eq 0 -and (Test-Path -LiteralPath $picture -PathType Leaf) -and
           (Test-Path -LiteralPath ($outStem+'.idx') -PathType Leaf) -and
           (Test-Path -LiteralPath ($outStem+'.pal') -PathType Leaf)) {
            $results.Add("NATIVE_RENDERED_SCENE_BMP=$picture")
            $results.Add("DISPLAYED_SCENE_IS_SAVED_GAME_RENDER_NOT_LIVE_GAMEPLAY=YES")
            Write-Host "Native source scene ready: $picture"
            Start-Process -FilePath $picture
        }else {
            $results.Add("NATIVE_SCENE=FAILED_SEE_RENDER_LOG")
            Write-Warning "Native scene could not be rendered with this save. Read $worldLog"
        }
    } else {
        $results.Add("NATIVE_SCENE=SKIPPED_NO_SAVE_DAT_PLUS_LEV005_DAT")
        Write-Warning "No suitable original Level 5 save selected; source boot diagnostics still completed."
    }
}

$summary=Join-Path $session 'STARTUP_RESULT.txt'
$results | Set-Content -LiteralPath $summary -Encoding UTF8
Write-Host ""
Write-Host "All results: $summary"
Write-Host "Original-source startup is bounded. An interactive CODE.1->CODE.2 game boot is NOT implemented."
