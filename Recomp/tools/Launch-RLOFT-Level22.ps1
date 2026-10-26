# Native CODE.2 Level22 experimental source host. Owner-local game media only.
[CmdletBinding()]
param(
 [string]$GameFolder = "",
 [switch]$Smoke,
 [switch]$ExperimentalPortals,
 [string]$SavedLevel23Folder = "",
 [switch]$SavedPortalSmoke,
 [string]$CdRoot = ""
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
# An explicit installation path wins. Otherwise auto-discover common GOG paths
# and let the owner select a directory when their library is elsewhere.
if([string]::IsNullOrWhiteSpace($GameFolder)) {
  $paths=New-Object System.Collections.Generic.List[string]
  if(-not [string]::IsNullOrWhiteSpace($env:RLOFT_GAME_FOLDER)) {
    $paths.Add($env:RLOFT_GAME_FOLDER)
  }
  foreach($base in @('C:\GOG Games','D:\GOG Games','E:\GOG Games',
                      "$env:ProgramFiles", "C:\Program Files (x86)")) {
    if([string]::IsNullOrWhiteSpace($base)) { continue }
    $paths.Add((Join-Path (Join-Path $base "Ravenloft - Strahd's Possession") 'RLOFT'))
    $paths.Add((Join-Path (Join-Path $base "GOG Galaxy\Games\Ravenloft - Strahd's Possession") 'RLOFT'))
  }
  foreach($candidate in $paths) {
    if((Test-Path -LiteralPath (Join-Path $candidate 'CODE.2') -PathType Leaf)) {
      $GameFolder=$candidate
      break
    }
  }
}
if([string]::IsNullOrWhiteSpace($GameFolder)) {
  Write-Host 'Select the original Ravenloft RLOFT folder containing CODE.2.'
  Add-Type -AssemblyName System.Windows.Forms
  $dialog=New-Object System.Windows.Forms.FolderBrowserDialog
  $dialog.Description='Choose the installed Ravenloft RLOFT folder (contains CODE.2)'
  if($dialog.ShowDialog() -ne [System.Windows.Forms.DialogResult]::OK) {
    throw 'Original RLOFT installation selection cancelled.'
  }
  $GameFolder=$dialog.SelectedPath
}
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
  if($ExperimentalPortals) {
    # Host process environment is inherited by the Windows executable.
    # The original guest uses _putenv/getenv to read the opt-in gate.
    $env:RLOFT_RECOMP_EXPERIMENTAL_LIVE_LEVEL_TRANSITIONS='1'
    Write-Warning 'Live portal traversal is EXPERIMENTAL. Back up your saves; this host never writes the retail WORK directory.'
  }
  if(-not [string]::IsNullOrWhiteSpace($SavedLevel23Folder)) {
    $slot=[IO.Path]::GetFullPath($SavedLevel23Folder)
    if(-not (Test-Path -LiteralPath (Join-Path $slot 'SAVE.DAT') -PathType Leaf) -or
       -not (Test-Path -LiteralPath (Join-Path $slot 'LEV023.DAT') -PathType Leaf)) {
      throw 'Saved Level23 folder must contain untouched SAVE.DAT and LEV023.DAT.'
    }
    if($Smoke) { throw '-Smoke is a Level22-only fixture. Use -SavedPortalSmoke for Level23.' }
    $sourceMode=if($SavedPortalSmoke){'--smoke-save23'}else{'--load-save23'}
    Write-Host "Authentic original Level23 save restore: $slot (source 07938)."
    & $exe --root $root --cd-root $sourceCd $sourceMode $slot
  } elseif($SavedPortalSmoke) {
    throw '-SavedPortalSmoke requires -SavedLevel23Folder.'
  } elseif($Smoke) {
    & $exe --root $root --cd-root $sourceCd --smoke
  } else {
    & $exe --root $root --cd-root $sourceCd
  }
  if($LASTEXITCODE -ne 0) { throw "Level22 guest exited with code $LASTEXITCODE." }
} finally {
  if($null -ne $private -and (Test-Path -LiteralPath $private -PathType Container)) {
    Remove-Item -LiteralPath $private -Recurse -Force
  }
}
