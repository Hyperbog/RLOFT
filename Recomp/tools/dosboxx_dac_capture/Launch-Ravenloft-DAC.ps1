# Launch the original Ravenloft game inside instrumented DOSBox-X.
# Original game files must be owner-supplied; never downloads/copies game files.
[CmdletBinding()]
param([string]$GameFolder, [switch]$CompatibilityMode)
$ErrorActionPreference = 'Stop'
$emulator = Join-Path $PSScriptRoot 'dosbox-x.exe'
if (-not (Test-Path -LiteralPath $emulator -PathType Leaf)) {
    throw "Cannot find dosbox-x.exe alongside this launcher: $emulator"
}
if ([string]::IsNullOrWhiteSpace($GameFolder)) {
    Add-Type -AssemblyName System.Windows.Forms
    $picker = New-Object System.Windows.Forms.FolderBrowserDialog
    $picker.Description = 'Select the folder containing original Ravenloft R.EXE and RLOFT.BAT'
    $picker.ShowNewFolderButton = $false
    if ($picker.ShowDialog() -ne [System.Windows.Forms.DialogResult]::OK) {
        Write-Host 'No game folder selected. No emulator launched.'
        return
    }
    $GameFolder = $picker.SelectedPath
}
$GameFolder = (Resolve-Path -LiteralPath $GameFolder).Path
$mustExist = @('R.EXE','RLOFT.BAT','CODE.1','CODE.2','COLORS','RES0','RES3','RES4')
$missing = @($mustExist | Where-Object { -not (Test-Path -LiteralPath (Join-Path $GameFolder $_) -PathType Leaf) })
if ($missing.Count -gt 0) {
    throw "Wrong game folder: $GameFolder`nMissing original game file(s): $($missing -join ', ')`nSelect the RLOFT folder itself, not its parent."
}
# GOG puts the original disc image alongside the RLOFT game directory.
# Mounting the installed files alone cannot satisfy INFO.DAT's D: resource paths.
$imagePath = Join-Path (Split-Path -Path $GameFolder -Parent) 'game.gog'
if (-not (Test-Path -LiteralPath $imagePath -PathType Leaf)) {
    throw "Missing GOG CD image at: $imagePath`nThe original game needs both its installed files and the virtual CD."
}
$private = Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'RLOFT-DAC-private'
New-Item -ItemType Directory -Path $private -Force | Out-Null
$stamp = [DateTime]::UtcNow.ToString('yyyyMMddTHHmmssfffZ')
$log = Join-Path $private "active-dac-$stamp.jsonl"
if (Test-Path -LiteralPath $log) { throw "Capture file already exists: $log" }
$old = $env:RLOFT_DAC_CAPTURE_JSONL
try {
    $env:RLOFT_DAC_CAPTURE_JSONL = $log
    Write-Host 'Ravenloft original game folder:' $GameFolder
    Write-Host 'Instrumented DOSBox-X:' $emulator
    Write-Host 'Private DAC capture:' $log
    Write-Host 'Launching R.EXE directly so original RLOFT.BAT cannot issue EXIT and hide startup errors.'
    # DOSBox-X IMGMOUNT needs -t iso because GOG uses a .gog extension.
    # The resource loader resolves RES9/RES10 on CD drive D:.
    $commands = @(
        '-c', ('mount c "' + $GameFolder + '"'),
        '-c', ('imgmount d "' + $imagePath + '" -t iso -fs iso'),
        '-c', 'c:',
        '-c', 'r.exe'
    )
    Write-Host 'GOG CD image:' $imagePath
    if ($CompatibilityMode) {
        # Controlled DOS/4GW diagnostic: defaults + interpreted CPU + no EMS/VCPI.
        # Keep XMS enabled. Do not alter game files.
        $commands = @('-defaultconf', '-set', 'cpu core=normal', '-set',
                      'cpu cycles=5000', '-set', 'dos ems=false') + $commands
        Write-Host 'Compatibility test: core=normal, cycles=5000, ems=false'
    }
    & $emulator @commands
    $exitCode = $LASTEXITCODE
    Write-Host "DOSBox-X host process ended with exit code $exitCode"
    if ($exitCode -ne 0) { Write-Warning "DOSBox-X returned a nonzero code $exitCode" }
    if ($CompatibilityMode) {
        Read-Host 'Press Enter to close this diagnostic window'
    }
}
finally {
    $env:RLOFT_DAC_CAPTURE_JSONL = $old
}
