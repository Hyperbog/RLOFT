# Validate an asset-free Windows native-engine BOOTSTRAP distribution.
# This is archive/provenance integrity checking, NOT Authenticode or gameplay.
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$Archive,
    [Parameter(Mandatory = $true)][string]$ExpectedSourceSha
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-RloftFileSha256([string]$Path) {
    # PowerShell 5.1 on some GitHub images does not autoload
    # Microsoft.PowerShell.Utility/Get-FileHash from a CTest child.
    # Use the .NET SHA256 implementation present in both PS5.1 and pwsh.
    $stream = [System.IO.File]::OpenRead($Path)
    $algorithm = [System.Security.Cryptography.SHA256]::Create()
    try {
        return [System.BitConverter]::ToString($algorithm.ComputeHash($stream)).Replace("-", "").ToLowerInvariant()
    } finally {
        $algorithm.Dispose()
        $stream.Dispose()
    }
}


$sourceSha = $ExpectedSourceSha.ToLowerInvariant()
if (-not ($sourceSha -cmatch '^[0-9a-f]{40}$')) {
    throw "ExpectedSourceSha must be exactly 40 hexadecimal characters"
}

$archivePath = (Resolve-Path -LiteralPath $Archive).Path
$archiveName = "RLOFT-NativeEngine-Windows-x64-BOOTSTRAP.zip"
if ([IO.Path]::GetFileName($archivePath) -cne $archiveName) {
    throw "Unexpected package name"
}
$sidecar = "$archivePath.sha256"
if (-not (Test-Path -LiteralPath $sidecar -PathType Leaf)) {
    throw "ZIP SHA256 sidecar missing"
}
$line = (Get-Content -LiteralPath $sidecar -Raw).Trim()
if (-not ($line -cmatch '^([0-9a-f]{64})  RLOFT-NativeEngine-Windows-x64-BOOTSTRAP\.zip$')) {
    throw "Invalid ZIP SHA256 sidecar"
}
$expectedArchiveSha = $Matches[1]
$actualArchiveSha = (Get-RloftFileSha256 -Path $archivePath)
if ($actualArchiveSha -cne $expectedArchiveSha) {
    throw "ZIP SHA256 mismatch"
}

Add-Type -AssemblyName System.IO.Compression.FileSystem
$allowed = @(
    "README-WINDOWS.txt",
    "SHA256SUMS.txt",
    "build-provenance.json",
    "rloft_install_probe.exe",
    "rloft_native_bootstrap.exe"
)
$seen = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::Ordinal)
$destination = Join-Path ([IO.Path]::GetTempPath()) ("rloft-native-zip-verify-" + [Guid]::NewGuid().ToString("N"))
[void](New-Item -ItemType Directory -Force -Path $destination)
$zip = $null
try {
    $zip = [System.IO.Compression.ZipFile]::OpenRead($archivePath)
    if ($zip.Entries.Count -ne $allowed.Count) {
        throw "Unexpected number of archive entries"
    }

    # Inspect every name BEFORE extraction, including duplicate names,
    # traversal, alternate directories and path/case substitution.
    foreach ($entry in $zip.Entries) {
        $entryName = $entry.FullName
        if ($allowed -cnotcontains $entryName) {
            throw "Unapproved archive entry: $entryName"
        }
        if (-not $seen.Add($entryName)) {
            throw "Duplicate archive entry: $entryName"
        }
        if ($entry.Length -gt 67108864 -or $entry.Length -lt 0) {
            throw "Archive entry exceeds 64 MiB diagnostic limit: $entryName"
        }
    }
    foreach ($required in $allowed) {
        if (-not $seen.Contains($required)) {
            throw "Required release entry missing: $required"
        }
    }

    # Extract only five whitelisted flat filenames from the already opened
    # ZIP object, not arbitrary user-controlled relative paths.
    foreach ($entry in $zip.Entries) {
        $outputPath = Join-Path $destination $entry.FullName
        $inputStream = $entry.Open()
        $outputStream = [IO.File]::Create($outputPath)
        try {
            $inputStream.CopyTo($outputStream)
        } finally {
            $outputStream.Dispose()
            $inputStream.Dispose()
        }
        $actualLength = (Get-Item -LiteralPath $outputPath).Length
        if ($actualLength -ne $entry.Length) {
            throw "Extracted release file length mismatch"
        }
    }

    $provenance = Get-Content -LiteralPath (Join-Path $destination "build-provenance.json") -Raw | ConvertFrom-Json
    if ($provenance.source_commit -cne $sourceSha -or
        $provenance.source_branch -cne "native-engine" -or
        $provenance.build -cne "MSVC x64 Release" -or
        $provenance.test_outcome -cne "PASS" -or
        $provenance.game_playable -ne $false -or
        $provenance.original_assets_included -ne $false) {
        throw "Incorrect source, test, or non-playable release provenance"
    }

    $sumLines = @(Get-Content -LiteralPath (Join-Path $destination "SHA256SUMS.txt"))
    if ($sumLines.Count -ne 2) {
        throw "Exactly two binary SHA256 records required"
    }
    $recorded = @{}
    foreach ($sumLine in $sumLines) {
        if (-not ($sumLine -cmatch '^([0-9a-f]{64})  (rloft_native_bootstrap\.exe|rloft_install_probe\.exe)$')) {
            throw "Malformed or unauthorized executable checksum record"
        }
        $name = $Matches[2]
        $sha = $Matches[1]
        if ($recorded.ContainsKey($name)) {
            throw "Duplicate executable checksum record"
        }
        $recorded[$name] = $sha
    }
    foreach ($name in @("rloft_native_bootstrap.exe", "rloft_install_probe.exe")) {
        if (-not $recorded.ContainsKey($name)) {
            throw "Missing binary checksum: $name"
        }
        $actual = (Get-RloftFileSha256 -Path (Join-Path $destination $name))
        if ($actual -cne $recorded[$name]) {
            throw "Binary SHA256 mismatch: $name"
        }
    }

    Write-Host "WINDOWS_BOOTSTRAP_PACKAGE_INTEGRITY_PASS"
    Write-Host "WINDOWS_BOOTSTRAP_SOURCE_COMMIT: $sourceSha"
    Write-Host "WINDOWS_BOOTSTRAP_ZIP_SHA256: $actualArchiveSha"
    Write-Host "WARNING: integrity checks do not attest Windows executable behavior or game playability."
} finally {
    if ($null -ne $zip) { $zip.Dispose() }
    if (Test-Path -LiteralPath $destination) {
        Remove-Item -LiteralPath $destination -Recurse -Force
    }
}
