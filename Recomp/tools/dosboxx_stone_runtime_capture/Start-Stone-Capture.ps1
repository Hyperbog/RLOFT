# Run original Stone with isolated C: and opt-in CPU/actor/event telemetry.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$GameDir,
      [Parameter(Mandatory=$true)][string]$CdDir,
      [Parameter(Mandatory=$true)][string]$DosboxExe,
      [switch]$EveryInstruction)
$ErrorActionPreference='Stop'
foreach ($f in @('SP.EXE','CODE.1','CODE.2')) {
 if (!(Test-Path -LiteralPath (Join-Path $GameDir $f) -PathType Leaf)) {
  throw "Missing original installed Stone file $f"
 }
}
foreach ($f in @('LEVELS.DAT','NPCS.DAT')) {
 if (!(Test-Path -LiteralPath (Join-Path $CdDir $f) -PathType Leaf)) {
  throw "Missing extracted Stone CD file $f"
 }
}
if (!(Test-Path -LiteralPath $DosboxExe -PathType Leaf)) {throw 'DOSBox-X missing'}
$hash=(Get-FileHash -LiteralPath (Join-Path $GameDir 'CODE.2') -Algorithm SHA256).Hash.ToLowerInvariant()
if ($hash -ne '00e225ec8f141a8fdb72ff0547fe93b2ea3aabdb4ec3b59d9261a5268de9203e') {
 throw "Original CODE.2 SHA-256 mismatch: $hash"
}
$stamp=[DateTime]::UtcNow.ToString('yyyyMMddTHHmmssfffZ') + '-' + $PID
$session=Join-Path (Join-Path $env:LOCALAPPDATA 'RLOFT-Stone-Original-Trace') $stamp
if (Test-Path -LiteralPath $session) {throw 'Refusing overwrite'}
$copy=Join-Path $session 'PROPHET'
New-Item -ItemType Directory -Path $copy -Force | Out-Null
Copy-Item -Path (Join-Path $GameDir '*') -Destination $copy -Recurse -Force
$trace=Join-Path $session 'stone-original.jsonl'
$report=Join-Path $session 'stone-summary.json'
$conf=Join-Path $session 'stone-original.conf'
$log=Join-Path $session 'launcher.log'
$cfg=@('[cpu]','core=normal','cycles=5000','',
       '[autoexec]','z:','mount -u c','mount -u d',
       ('mount c "'+$copy+'"'),
       ('mount d "'+$CdDir+'" -t cdrom'),
       'c:','sp.exe',
       'echo.',
       'echo STONE CAPTURE: SP.EXE has returned to the DOS prompt.',
       'echo Copy any error above before closing this window.',
       'echo Type EXIT only after recording the message.')
Set-Content -LiteralPath $conf -Value $cfg -Encoding ASCII
$prior=$env:RLOFT_STONE_ORIGINAL_TRACE_JSONL
$priorStrict=$env:RLOFT_STONE_TRACE_EVERY_INSTRUCTION
Start-Transcript -LiteralPath $log -ErrorAction Stop | Out-Null
try {
 $env:RLOFT_STONE_ORIGINAL_TRACE_JSONL=$trace
 $env:RLOFT_STONE_TRACE_EVERY_INSTRUCTION=if($EveryInstruction){'1'}else{'0'}
 Write-Host "Private isolated Stone C: and trace directory: $session"
 Write-Host "Launcher transcript: $log"
 Write-Host 'If SP.EXE fails, its DOS screen stays OPEN. Type EXIT to close it after recording the error.'
 & $DosboxExe -console -conf $conf
 $emulatorStatus=$LASTEXITCODE
 Write-Host "DOSBox-X process exited with status $emulatorStatus"
 if($emulatorStatus -ne 0) {Write-Warning "DOSBox-X startup/runtime error code: $emulatorStatus. Look for details in DOSBox-X console and launcher.log."}
} finally {
 $env:RLOFT_STONE_ORIGINAL_TRACE_JSONL=$prior
 $env:RLOFT_STONE_TRACE_EVERY_INSTRUCTION=$priorStrict
 Stop-Transcript | Out-Null
}
if (!(Test-Path -LiteralPath $trace -PathType Leaf)) {
 throw "No trace file created in $session"
}
& python (Join-Path $PSScriptRoot 'summarize_stone_runtime_trace.py') --trace $trace --report $report
if ($LASTEXITCODE -ne 0) {
 throw "No authenticated original CODE.2 event evidence; preserve $session"
}
Write-Host "VALIDATED_SUMMARY=$report"
Write-Host "OWNER_PRIVATE_TRACE=$trace"
Write-Host 'No campaign or DOS crash conclusion without runtime evidence.'
