# Native CODE.2 Level22 experimental source host. Owner-local game media only.
[CmdletBinding()]
param(
 [string]$GameFolder = "D:\GOG Games\Ravenloft - Strahd's Possession\RLOFT",
 [string]$CdRoot = ""
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($GameFolder)
$exe=Join-Path $PSScriptRoot 'rloft_code2_level22_window.exe'
if(-not (Test-Path -LiteralPath $exe -PathType Leaf)) {
  throw "Missing original Level22 window executable. Extract the complete Windows milestone ZIP."
}
if(-not (Test-Path -LiteralPath (Join-Path $root 'CODE.2') -PathType Leaf)) {
  throw "Original CODE.2 not found at $root. Use -GameFolder to specify the original installation."
}
$private=$null
try {
  if([string]::IsNullOrWhiteSpace($CdRoot)) {
    $parent=Split-Path -Parent $root
    $disc=Join-Path $parent 'game.gog'
    $extractor=Join-Path $PSScriptRoot 'rloft_gog_cd_extract.exe'
    if(-not (Test-Path -LiteralPath $disc -PathType Leaf)) {
      throw "Original game.gog not found at $disc. Supply -CdRoot for an already-extracted CD RLOFT folder."
    }
    if(-not (Test-Path -LiteralPath $extractor -PathType Leaf)) {
      throw "GOG CD extractor missing from complete package."
    }
    $private=Join-Path ([IO.Path]::GetTempPath()) ("RLOFT-Level22-" + [Guid]::NewGuid().ToString('N'))
    New-Item -Path $private -ItemType Directory -Force | Out-Null
    Write-Host "Extracting original CD resources into private local scratch..."
    & $extractor --image $disc --out $private
    if($LASTEXITCODE -ne 0) { throw "CD extraction failed with exit code $LASTEXITCODE." }
    $sourceCd=$private
  } else {
    $sourceCd=[IO.Path]::GetFullPath($CdRoot)
  }
  Write-Host "Native original Level22: W/S forward/back, A/D strafe, J/L or arrows turn, Esc quit."
  Write-Host "EXPERIMENTAL original-source window; unsupported game events stop execution."
  & $exe --root $root --cd-root $sourceCd
  if($LASTEXITCODE -ne 0) { throw "Level22 guest exited with code $LASTEXITCODE." }
} finally {
  if($null -ne $private -and (Test-Path -LiteralPath $private -PathType Container)) {
    Remove-Item -LiteralPath $private -Recurse -Force
  }
}
