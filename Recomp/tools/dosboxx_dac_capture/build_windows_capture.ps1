# Build an opt-in DAC-instrumented DOSBox-X SDL2 executable on Windows.
# Run in "Developer PowerShell for VS 2022" with Python and Git available.
# No Ravenloft media is copied.
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)] [string]$RloftRoot,
    [Parameter(Mandatory=$true)] [string]$DosboxSourceRoot
)
$ErrorActionPreference = 'Stop'
$tag = 'dosbox-x-v2026.10.01'
$commit = 'b6abbd5980a885f5f310a4088c59a8688d1b116c'
$tools = Join-Path $RloftRoot 'Recomp\tools\dosboxx_dac_capture'
$patcher = Join-Path $tools 'apply_dosboxx_dac_capture.py'
$header = Join-Path $tools 'rloft_dac_port_capture.hpp'
if (!(Test-Path -LiteralPath $patcher -PathType Leaf) -or
    !(Test-Path -LiteralPath $header -PathType Leaf)) {
    throw 'Expected the Recomp capture tools in your local RLOFT checkout.'
}
if (!(Get-Command python -ErrorAction SilentlyContinue)) {
    throw 'Python 3 is needed to authenticate and apply the hook.'
}
if (!(Get-Command git -ErrorAction SilentlyContinue)) {
    throw 'Git is needed to authenticate the exact DOSBox-X source release.'
}
if (!(Test-Path -LiteralPath $DosboxSourceRoot)) {
    & git clone --depth 1 --branch $tag 'https://github.com/joncampbell123/dosbox-x.git' $DosboxSourceRoot
    if ($LASTEXITCODE -ne 0) {throw 'Failed to clone upstream DOSBox-X.'}
}
$actual = (& git -C $DosboxSourceRoot rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0 -or $actual -ne $commit) {
    throw "Expected exact DOSBox-X source commit $commit, obtained $actual"
}
& python $patcher --dosbox-root $DosboxSourceRoot --header $header
if ($LASTEXITCODE -ne 0) {throw 'Source authenticity or patching failed.'}
$msbuild = Get-Command msbuild -ErrorAction SilentlyContinue
if (!$msbuild) {
    throw 'MSBuild not on PATH. Run this in Developer PowerShell for VS 2022.'
}
$solution = Join-Path $DosboxSourceRoot 'vs\dosbox-x.sln'
& msbuild $solution '/t:dosbox-x:Rebuild' '/p:Configuration=Release SDL2' '/p:Platform=x64' '/p:PlatformToolset=v143' '/m'
if ($LASTEXITCODE -ne 0) {
    throw 'DOSBox-X instrumented Windows build failed; see original compiler log.'
}
$exe = Join-Path $DosboxSourceRoot 'bin\x64\Release SDL2\dosbox-x.exe'
if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    throw "Build returned success but expected DOSBox-X executable was missing: $exe"
}
Write-Host 'RLOFT_DOSBOXX_DAC_HOOK_WINDOWS_BUILT=YES'
Write-Host "Emulator executable: $exe"
Write-Host 'Live Ravenloft DAC parity: NOT_CAPTURED'
