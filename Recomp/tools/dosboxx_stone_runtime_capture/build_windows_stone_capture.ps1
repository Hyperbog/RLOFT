# Requires VS 2022 Developer PowerShell, Python 3 and Git.
[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RloftRoot,
      [Parameter(Mandatory=$true)][string]$DosboxSourceRoot)
$ErrorActionPreference='Stop'
$commit='b6abbd5980a885f5f310a4088c59a8688d1b116c'
$blob='e582cc06f38809f2ee8ee8d6f1922f3eba5b6ab5'
$tools=Join-Path $RloftRoot 'Recomp\tools\dosboxx_stone_runtime_capture'
if (!(Get-Command git -ErrorAction SilentlyContinue) -or
    !(Get-Command python -ErrorAction SilentlyContinue)) {
 throw 'Python 3 and Git required'
}
if (!(Test-Path -LiteralPath $DosboxSourceRoot)) {
 & git -c core.autocrlf=false clone --depth 1 --branch 'dosbox-x-v2026.10.01' 'https://github.com/joncampbell123/dosbox-x.git' $DosboxSourceRoot
 if ($LASTEXITCODE -ne 0) {throw 'Pinned DOSBox-X clone failed'}
}
$actual=(& git -C $DosboxSourceRoot rev-parse HEAD).Trim()
if ($actual -ne $commit) {throw "DOSBox-X commit mismatch: $actual"}
$core=Join-Path $DosboxSourceRoot 'src\cpu\core_normal.cpp'
$actualBlob=(& git -C $DosboxSourceRoot hash-object $core).Trim()
if ($actualBlob -ne $blob) {throw "Original core differs: $actualBlob"}
& python (Join-Path $tools 'apply_stone_runtime_capture.py') --dosbox-root $DosboxSourceRoot --header (Join-Path $tools 'rloft_stone_runtime_capture.hpp')
if ($LASTEXITCODE -ne 0) {throw 'Original source hook failed'}
if (!(Get-Command msbuild -ErrorAction SilentlyContinue)) {
 throw 'Use Visual Studio 2022 Developer PowerShell'
}
$solution=Join-Path $DosboxSourceRoot 'vs\dosbox-x.sln'
& msbuild $solution '/t:dosbox-x:Rebuild' '/p:Configuration=Release SDL2' '/p:Platform=x64' '/p:PlatformToolset=v143' '/m'
if ($LASTEXITCODE -ne 0) {throw 'DOSBox-X build failed'}
$exe=Join-Path $DosboxSourceRoot 'bin\x64\Release SDL2\dosbox-x.exe'
if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {throw "Missing $exe"}
Write-Host "INSTRUMENTED_DOSBOXX_EXE=$exe"
Write-Host 'Original DOS game was not executed by this build.'
