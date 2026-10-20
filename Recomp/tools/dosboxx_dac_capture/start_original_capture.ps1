# Start the instrumented DOSBox-X executable with an independent raw
# VGA 3C8/3C9 port event log. Does not run or copy retail game media.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)] [string]$DosboxExe,
  [Parameter(Mandatory=$true)] [string]$PrivateFolder
)
$ErrorActionPreference = 'Stop'
$exe = (Resolve-Path -LiteralPath $DosboxExe).Path
if ([IO.Path]::GetFileName($exe) -ne 'dosbox-x.exe') {
  throw 'Specify the instrumented dosbox-x.exe from the GitHub build.'
}
$folder = [IO.Path]::GetFullPath($PrivateFolder)
New-Item -ItemType Directory -Path $folder -Force | Out-Null
$log = Join-Path $folder 'active-dac.jsonl'
if (Test-Path -LiteralPath $log) {
  throw "Previous raw capture exists; relocate it first: $log"
}
$prior = $env:RLOFT_DAC_CAPTURE_JSONL
try {
  $env:RLOFT_DAC_CAPTURE_JSONL = $log
  $proc = Start-Process -FilePath $exe -WorkingDirectory ([IO.Path]::GetDirectoryName($exe)) -PassThru
}
finally {
  $env:RLOFT_DAC_CAPTURE_JSONL = $prior
}
if (!$proc) { throw 'DOSBox-X process did not start.' }
Write-Host "DOSBOXX_CAPTURE_PROCESS_ID=$($proc.Id)"
Write-Host "DOSBOXX_RAW_CAPTURE=$log"
Write-Host 'Run original Ravenloft and load the target scene.'
Write-Host 'Pause emulation WITHOUT interrupt processing, then save a DOSBox-X .sav'
Write-Host 'Use freeze_original_capture.ps1 WHILE THE EMULATOR REMAINS PAUSED.'
