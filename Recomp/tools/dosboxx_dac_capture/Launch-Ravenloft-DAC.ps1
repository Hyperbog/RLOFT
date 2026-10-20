# Ravenloft: Strahd's Possession -- exact GOG installation, instrumented DOSBox-X.
# This script NEVER modifies the original game or the original game.gog image.
[CmdletBinding()]
param([switch]$CompatibilityMode)

$ErrorActionPreference = 'Stop'

# Owner-confirmed installation paths. Keep both values quoted.
$gameDir = "D:\GOG Games\Ravenloft - Strahd's Possession\RLOFT"
$cdImage = "D:\GOG Games\Ravenloft - Strahd's Possession\game.gog"
$dosbox = Join-Path $PSScriptRoot 'dosbox-x.exe'

if (-not (Test-Path -LiteralPath $dosbox -PathType Leaf)) {
    throw "Instrumented dosbox-x.exe not found beside the launcher: $dosbox"
}
if (-not (Test-Path -LiteralPath $gameDir -PathType Container)) {
    throw "Ravenloft game directory not found: $gameDir"
}
if (-not (Test-Path -LiteralPath $cdImage -PathType Leaf)) {
    throw "Ravenloft GOG CD image not found: $cdImage"
}
foreach ($required in @('R.EXE', 'RLOFT.BAT', 'CODE.1', 'CODE.2', 'INFO.DAT', 'COLORS')) {
    if (-not (Test-Path -LiteralPath (Join-Path $gameDir $required) -PathType Leaf)) {
        throw "Missing $required in $gameDir; this must be the innermost RLOFT folder."
    }
}

$documents = [Environment]::GetFolderPath('MyDocuments')
if ([string]::IsNullOrWhiteSpace($documents)) {
    throw 'Windows Documents folder could not be resolved.'
}
$privateDir = Join-Path $documents 'RLOFT-DAC-private'
New-Item -ItemType Directory -Path $privateDir -Force | Out-Null
$utc = [DateTime]::UtcNow.ToString('yyyyMMddTHHmmssfffZ')
$log = Join-Path $privateDir "active-dac-$utc-$PID.jsonl"
if (Test-Path -LiteralPath $log) {
    throw "Refusing to overwrite an existing DAC capture: $log"
}

# Put startup commands in a real DOSBox-X [autoexec] configuration file.
# DO NOT use nested quotes through PowerShell's -c argument; Windows native
# argument marshalling can silently strip quotes in paths containing spaces.
# Requiring an explicit -conf avoids ambiguity in long GOG installation paths.
$conf = Join-Path $privateDir "RLOFT-DAC-AutoLaunch-$utc-$PID.conf"
if (Test-Path -LiteralPath $conf) {
    throw "Refusing to overwrite a DOSBox-X launch configuration: $conf"
}
$cfg = @(
    '[dosbox]',
    'saveremark=false',
    '',
    '[autoexec]',
    'z:',
    'mount -u c',
    'mount -u d',
    ('mount c "' + $gameDir + '"'),
    ('imgmount d "' + $cdImage + '" -t iso -fs iso'),
    'c:',
    'r.exe'
)
Set-Content -LiteralPath $conf -Value $cfg -Encoding ASCII

# The entire launch config is one file path argument to DOSBox-X.
# No nested quoting or command splitting is required.
$commands = @('-conf', $conf)
if ($CompatibilityMode) {
    $commands = @('-set', 'cpu core=normal', '-set', 'cpu cycles=5000',
                  '-set', 'dos ems=false') + $commands
}

Write-Host ''
Write-Host 'ORIGINAL RAVENLOFT LAUNCH' -ForegroundColor Cyan
Write-Host "C: Game:  $gameDir"
Write-Host "D: CD:    $cdImage"
Write-Host "DAC log:  $log"
Write-Host "Config:   $conf"
Write-Host 'Game:     R.EXE (not RLOFT.BAT, which ends with EXIT)'
Write-Host ''

$prior = $env:RLOFT_DAC_CAPTURE_JSONL
try {
    $env:RLOFT_DAC_CAPTURE_JSONL = $log
    & $dosbox @commands
    $code = $LASTEXITCODE
    Write-Host "DOSBox-X exited with code: $code"
    Write-Host "Capture path: $log"
    if ($code -ne 0) {
        Write-Warning "DOSBox-X reported exit code $code"
        exit $code
    }
}
finally {
    $env:RLOFT_DAC_CAPTURE_JSONL = $prior
}
