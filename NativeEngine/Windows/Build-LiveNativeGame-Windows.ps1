# Owner-authorized execution only. This script does not start GitHub CI.
# Builds an independent original-source game EXE, not a renamed demo binary.
[CmdletBinding()]
param(
    [string]$BuildDir = "",
    [string]$OutputDir = ""
)
$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
$repo = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
if (-not $BuildDir) { $BuildDir = Join-Path $repo "build\native-engine-windows-x64" }
if (-not $OutputDir) { $OutputDir = Join-Path $repo "NativeEngine\Windows\dist" }
$sha = (& git -C $repo rev-parse --verify HEAD).Trim()
if ($LASTEXITCODE -ne 0 -or $sha -notmatch '^[a-f0-9]{40}$') {
    throw "Exact native-engine Git source SHA unavailable"
}
# Existing script builds ALL targets and runs ALL native CTests.
& (Join-Path $PSScriptRoot "Build-NativeEngine-Windows.ps1") -BuildDir $BuildDir -OutputDir $OutputDir
if (-not $?) { throw "MSVC compilation, tests or bootstrap package failed" }

$registered = @(& ctest --test-dir $BuildDir -C Release -N)
if ($LASTEXITCODE -ne 0) { throw "Cannot enumerate native CTests" }
$names = $registered -join [Environment]::NewLine
foreach ($required in @(
    "native_profile_isolation_tests",
    "native_live_game_product_self_test",
    "native_original_menu_preview_tests",
    "native_windows_package_integrity_contract"
)) {
    if (-not $names.Contains($required)) { throw "Missing live native gate: $required" }
}
$count = [regex]::Match($names, 'Total Tests:\s*(\d+)')
if (-not $count.Success -or [int]$count.Groups[1].Value -lt 17) {
    throw "Native game preview requires at least 17 CTests"
}
$exe = Join-Path $BuildDir "Release\rloft_native.exe"
if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) {
    throw "Independent rloft_native.exe was not compiled"
}
$smoke = Start-Process -FilePath $exe -ArgumentList "--self-test-product" -Wait -PassThru -NoNewWindow
if ($smoke.ExitCode -ne 0) {
    throw "Independent game/profile smoke failed: $($smoke.ExitCode)"
}
function Sha256File([string]$file) {
    $stream = [IO.File]::OpenRead($file)
    $digest = [System.Security.Cryptography.SHA256]::Create()
    try {
        return [BitConverter]::ToString($digest.ComputeHash($stream)).Replace("-", "").ToLowerInvariant()
    } finally { $digest.Dispose(); $stream.Dispose() }
}
$name = "RLOFT-NativeEngine-Windows-x64-LIVE-PREVIEW"
$staging = Join-Path $OutputDir $name
[void](New-Item -ItemType Directory -Path $OutputDir -Force)
if (Test-Path -LiteralPath $staging) { Remove-Item -LiteralPath $staging -Recurse -Force }
[void](New-Item -ItemType Directory -Path $staging -Force)
Copy-Item -LiteralPath $exe -Destination (Join-Path $staging "rloft_native.exe")
Copy-Item -LiteralPath (Join-Path $PSScriptRoot "README-LIVE-PREVIEW.txt") -Destination (Join-Path $staging "README-LIVE-PREVIEW.txt")
$binaryHash = Sha256File (Join-Path $staging "rloft_native.exe")
"$binaryHash  rloft_native.exe" | Set-Content -Encoding ascii (Join-Path $staging "SHA256SUMS.txt")
[ordered]@{
    product = "Ravenloft native-engine source-shared live integration preview"
    source_branch = "native-engine"
    source_commit = $sha
    build = "MSVC x64 Release"
    ctest_outcome = "PASS"
    original_install_field_test = "NOT_YET_VERIFIED"
    game_playability_verified = $false
    original_assets_included = $false
    native_profile = "%LOCALAPPDATA%\Hyperbog\RLOFT\NativeEngine\Default"
    built_utc = (Get-Date).ToUniversalTime().ToString("o")
} | ConvertTo-Json | Set-Content -Encoding utf8 (Join-Path $staging "build-provenance.json")
$zip = Join-Path $OutputDir "$name.zip"
if (Test-Path -LiteralPath $zip) { Remove-Item -LiteralPath $zip -Force }
Compress-Archive -Path (Join-Path $staging "*") -DestinationPath $zip
if (-not (Test-Path -LiteralPath $zip -PathType Leaf)) { throw "Native game ZIP not created" }
$zipHash = Sha256File $zip
"$zipHash  $name.zip" | Set-Content -Encoding ascii "$zip.sha256"
# Reopen the FINAL ZIP; never infer distribution integrity from staging files.
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archiveReader = [System.IO.Compression.ZipFile]::OpenRead($zip)
try {
    $expectedNames = @(
        "rloft_native.exe",
        "README-LIVE-PREVIEW.txt",
        "SHA256SUMS.txt",
        "build-provenance.json"
    )
    if ($archiveReader.Entries.Count -ne $expectedNames.Count) {
        throw "Live game release ZIP has unexpected entry count"
    }
    $seen = [System.Collections.Generic.HashSet[string]]::new(
        [System.StringComparer]::Ordinal)
    foreach ($entry in $archiveReader.Entries) {
        if ($expectedNames -cnotcontains $entry.FullName -or
            -not $seen.Add($entry.FullName)) {
            throw "Unexpected or duplicate Windows live-game ZIP entry"
        }
    }
    $packedExe = $archiveReader.GetEntry("rloft_native.exe")
    if ($null -eq $packedExe -or $packedExe.Length -le 0) {
        throw "Windows native-game executable absent from final ZIP"
    }
    $shaAlgorithm = [System.Security.Cryptography.SHA256]::Create()
    $zipExeStream = $packedExe.Open()
    try {
        $packedHash = [BitConverter]::ToString(
            $shaAlgorithm.ComputeHash($zipExeStream)).Replace("-", "").ToLowerInvariant()
    } finally {
        $zipExeStream.Dispose()
        $shaAlgorithm.Dispose()
    }
    if ($packedHash -cne $binaryHash) {
        throw "ZIP native-game EXE checksum mismatch"
    }
    $metaEntry = $archiveReader.GetEntry("build-provenance.json")
    if ($null -eq $metaEntry) { throw "Native-game provenance missing" }
    $metaStream = $metaEntry.Open()
    $reader = [IO.StreamReader]::new($metaStream)
    try {
        $meta = $reader.ReadToEnd() | ConvertFrom-Json
    } finally {
        $reader.Dispose()
        $metaStream.Dispose()
    }
    if ($meta.source_commit -cne $sha -or
        $meta.game_playability_verified -ne $false -or
        $meta.original_assets_included -ne $false -or
        $meta.ctest_outcome -cne "PASS") {
        throw "Native-game archive provenance invalid"
    }
} finally {
    $archiveReader.Dispose()
}
Write-Host "NATIVE_GAME_SOURCE_SHARED_BUILD_AND_TEST_PASS"
Write-Host "Source SHA: $sha"
Write-Host "Windows game preview ZIP: $zip"
Write-Host "ZIP SHA256: $zipHash"
Write-Host "Executable SHA256: $binaryHash"
Write-Host "Original game field acceptance: NOT YET VERIFIED"
