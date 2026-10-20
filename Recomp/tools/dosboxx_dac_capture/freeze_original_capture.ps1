# Pair a frozen original DOSBox-X VGA state and independently instrumented
# byte-level DAC write history. Invoke WHILE guest emulation is paused.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)] [string]$RloftRoot,
  [Parameter(Mandatory=$true)] [string]$PrivateFolder,
  [Parameter(Mandatory=$true)] [string]$SavedState,
  [Parameter(Mandatory=$true)] [string]$OriginalCode2,
  [Parameter(Mandatory=$true)] [string]$CaptureId
)
$ErrorActionPreference = 'Stop'
if ($CaptureId -cnotmatch '^[A-Za-z0-9_-]{1,64}$') {
  throw 'CaptureId must contain only letters, digits, hyphens, underscores.'
}
$folder = [IO.Path]::GetFullPath($PrivateFolder)
$raw = Join-Path $folder 'active-dac.jsonl'
$state = (Resolve-Path -LiteralPath $SavedState).Path
$game = (Resolve-Path -LiteralPath $OriginalCode2).Path
$convert = Join-Path $RloftRoot 'Recomp\tools\dosboxx_dac_capture\convert_capture.py'
if (!(Test-Path -LiteralPath $raw -PathType Leaf)) {
  throw "Missing instrumented DOSBox-X raw event history: $raw"
}
if (!(Test-Path -LiteralPath $convert -PathType Leaf)) {
  throw "Missing converter at $convert"
}
if (!(Test-Path -LiteralPath $game -PathType Leaf)) {
  throw 'Original CODE.2 binary is missing.'
}
if ([IO.Path]::GetExtension($state).ToLowerInvariant() -ne '.sav') {
  throw 'Expected a DOSBox-X .sav file, not an in-game SAVE.DAT.'
}
$frozen = Join-Path $folder "$CaptureId-frozen-events.jsonl"
$copyState = Join-Path $folder "$CaptureId-original-emulator.sav"
$out = Join-Path $folder "$CaptureId-verified-events.json"
foreach ($name in @($frozen,$copyState,$out)) {
  if (Test-Path -LiteralPath $name) {
    throw "Refusing to overwrite an earlier evidence file: $name"
  }
}
Write-Host 'IMPORTANT: source emulator MUST already be paused without interrupts.'
Write-Host 'This script does not prove that the emulator was paused.'
Copy-Item -LiteralPath $raw -Destination $frozen -ErrorAction Stop
Copy-Item -LiteralPath $state -Destination $copyState -ErrorAction Stop
$before = (Get-FileHash -LiteralPath $state -Algorithm SHA256).Hash
$after = (Get-FileHash -LiteralPath $copyState -Algorithm SHA256).Hash
if ($before -ne $after) {
  throw 'DOSBox-X save state changed during copy; capture cannot be certified.'
}
& python $convert --raw-jsonl $frozen --state $copyState --code2 $game --capture-id $CaptureId --owner-confirms-paused-copy --output $out
if ($LASTEXITCODE -ne 0) {
  throw 'Independent DAC write replay did not match the original saved palette.'
}
Write-Host "RLOFT_FROZEN_PORT_HISTORY=$frozen"
Write-Host "RLOFT_FROZEN_ORIGINAL_SAV=$copyState"
Write-Host "RLOFT_ORIGINAL_DAC_RECEIPT=$out"
Write-Host 'RETAIL_DOS_CPU_CALL_ANCESTRY=NOT_ATTESTED'
