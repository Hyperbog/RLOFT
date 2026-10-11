# Windows x64 distribution acceptance: verify only the asset-free bootstrap zip.
# This is not a test of actual original DOS behavior or game playability.
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$Archive,
    [Parameter(Mandatory=$true)][string]$ExpectedSourceSha
)
$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$sourceSha = $ExpectedSourceSha.ToLowerInvariant()
if ($sourceSha -notmatch '^[0-9a-f]{40}$') { throw "ExpectedSourceSha must be a 40-hex Git SHA" }
$archiveFile = (Resolve-Path -LiteralPath $Archive).Path
$zipName = "RLOFT-NativeEngine-Windows-x64-BOOTSTRAP.zip"
if ([IO.Path]::GetFileName($archiveFile) -cne $zipName) {
    throw "Unexpected package name: $archiveFile"
}
$checksum = "$archiveFile.sha256"
if (-not (Test-Path -LiteralPath $checksum)) { throw "Missing ZIP SHA256 sidecar" }
$line = (Get-Content -LiteralPath $checksum -Raw).Trim()
if ($line -cnotmatch '^([0-9a-f]{64})  RLOFT-NativeEngine-Windows-x64-BOOTSTRAP\.zip$') {
    throw "Invalid ZIP SHA256 sidecar format"
}
$actualZipHash = (Get-FileHash -LiteralPath $archiveFile -Algorithm SHA256).Hash.ToLowerInvariant()
if ($actualZipHash -cne $Matches[1]) { throw "ZIP SHA256 does not match sidecar" }

$temp = Join-Path ([IO.Path]::GetTempPath()) ("rloft-native-package-check-" + [Guid]::NewGuid().ToString("N"))
New-Item -ItemType Directory -Force -Path $temp | Out-Null
try {
    Expand-Archive -LiteralPath $archiveFile -DestinationPath $temp -Force
    $allowed = @(
        "README-WINDOWS.txt",
        "SHA256SUMS.txt",
        "build-provenance.json",
        "rloft_install_probe.exe",
        "rloft_native_bootstrap.exe"
    )
    $found = @(Get-ChildItem -LiteralPath $temp -File -Recurse |
        ForEach-Object { $_.FullName.Substring($temp.Length + 1).Replace('\', '/') } |
        Sort-Object)
    $expected = @($allowed | Sort-Object)
    if (($found -join '|') -cne ($expected -join '|')) {
        throw "Unexpected or missing files in Windows bootstrap ZIP. Found: $($found -join ', ')"
    }

    $metadataPath = Join-Path $temp "build-provenance.json"
    $metadata = Get-Content -LiteralPath $metadataPath -Raw | ConvertFrom-Json
    if ($metadata.source_commit -cne $sourceSha -or
        $metadata.source_branch -cne "native-engine" -or
        $metadata.build -cne "MSVC x64 Release" -or
        $metadata.game_playable -ne $false -or
        $metadata.original_assets_included -ne $false -or
        $metadata.test_outcome -cne "PASS") {
        throw "Provenance or non-playable state invalid"
    }

    $sumFile = Join-Path $temp "SHA256SUMS.txt"
    $lines = @(Get-Content -LiteralPath $sumFile)
    if ($lines.Count -ne 2) { throw "Exactly two binary checksums required" }
    $recorded = @{}
    foreach ($shaLine in $lines) {
        if ($shaLine -cnotmatch '^([0-9a-f]{64})  (rloft_native_bootstrap.exe|rloft_install_probe.exe)$') {
            throw "Unexpected executable checksum record: $shaLine"
        }
        if ($recorded.ContainsKey($Matches[2])) { throw "Duplicate binary hash record" }
        $recorded[$Matches[2]] = $Matches[1]
    }
    foreach ($exe in @("rloft_native_bootstrap.exe", "rloft_install_probe.exe")) {
        $actual = (Get-FileHash -LiteralPath (Join-Path $temp $exe) -Algorithm SHA256).Hash.ToLowerInvariant()
        if ($actual -cne $recorded[$exe]) { throw "Extracted binary hash mismatch: $exe" }
    }
    Write-Host "WINDOWS_PACKAGE_PROVENANCE_PASS: $sourceSha"
    Write-Host "WINDOWS_PACKAGE_ASSET_FREE_ALLOWLIST_PASS"
    Write-Host "WINDOWS_PACKAGE_SHA256_PASS: $actualZipHash"
} finally {
    if (Test-Path -LiteralPath $temp) {
        Remove-Item -LiteralPath $temp -Recurse -Force
    }
}
