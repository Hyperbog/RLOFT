# Windows x64 original-asset-engine build and QA. Manual execution only.
# Source-identified asset-loader preview, NOT a playable game.
[CmdletBinding()]
param([string]$BuildDir = "", [string]$OutputDir = "")
$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
$repo = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
if (-not $BuildDir) { $BuildDir = Join-Path $repo "build\native-assets-windows-x64" }
if (-not $OutputDir) { $OutputDir = Join-Path $repo "NativeEngine\Windows\dist" }
function Sha256File([string]$path) {
    $inputStream = [IO.File]::OpenRead($path)
    $algorithm = [System.Security.Cryptography.SHA256]::Create()
    try {
        return [BitConverter]::ToString($algorithm.ComputeHash($inputStream)).Replace("-","").ToLowerInvariant()
    } finally { $algorithm.Dispose(); $inputStream.Dispose() }
}
Push-Location $repo
try {
    $sha = (& git rev-parse --verify HEAD).Trim()
    if ($LASTEXITCODE -ne 0 -or $sha -notmatch '^[a-f0-9]{40}$') {
        throw "Cannot obtain exact native source SHA"
    }
    & git diff --quiet HEAD -- NativeEngine
    if ($LASTEXITCODE -ne 0) { throw "Dirty tracked native engine source" }
    & cmake -S NativeEngine -B $BuildDir -G "Visual Studio 17 2022" -A x64 "-DRLOFT_NATIVE_SOURCE_SHA=$sha"
    if ($LASTEXITCODE -ne 0) { throw "Independent native asset CMake configure failed" }
    & cmake --build $BuildDir --config Release --parallel 2
    if ($LASTEXITCODE -ne 0) { throw "Independent native asset MSVC build failed" }
    $exe = Join-Path $BuildDir "Release\rloft_native.exe"
    if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) { throw "Independent Windows executable missing" }
    $names = @(& ctest --test-dir $BuildDir -C Release -N)
    if ($LASTEXITCODE -ne 0) { throw "Cannot enumerate native CTests" }
    $display = $names -join [Environment]::NewLine
    foreach ($test in @("native_asset_media_tests", "native_framework_contract_tests",
        "native_cpu_reference_presenter_tests", "native_disk_mod_mount_tests",
        "native_mod_load_plan_tests", "native_mod_mount_plan_tests",
        "native_renderer_evidence_contract_tests", "native_asset_engine_window_target",
        "native_raster_tests")) {
        if (-not $display.Contains($test)) { throw "Mandatory native asset CTest absent: $test" }
    }
    $count = [regex]::Match($display, 'Total Tests:\s*(\d+)')
    if (-not $count.Success -or [int]$count.Groups[1].Value -lt 10) {
        throw "At least 10 independent asset CTests required"
    }
    & ctest --test-dir $BuildDir -C Release --output-on-failure --parallel 2
    if ($LASTEXITCODE -ne 0) { throw "Windows native asset CTests failed" }
    $name = "RLOFT-NativeEngine-Windows-x64-ASSET-PREVIEW"
    $stage = Join-Path $OutputDir $name
    [void](New-Item -ItemType Directory -Path $OutputDir -Force)
    if (Test-Path -LiteralPath $stage) { Remove-Item -LiteralPath $stage -Recurse -Force }
    [void](New-Item -ItemType Directory -Path $stage -Force)
    Copy-Item -LiteralPath $exe -Destination (Join-Path $stage "rloft_native.exe")
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot "README-NATIVE-ASSETS.txt") -Destination (Join-Path $stage "README-NATIVE-ASSETS.txt")
    $binaryHash = Sha256File (Join-Path $stage "rloft_native.exe")
    "$binaryHash  rloft_native.exe" | Set-Content -Encoding ascii (Join-Path $stage "SHA256SUMS.txt")
    [ordered]@{
        product = "RLOFT independent native replacement engine - asset layer"
        source_branch = "native-engine"
        source_commit = $sha
        build = "MSVC x64 Release"
        engine_stage = "NATIVE_ASSET_LOADING"
        gameplay_verified = $false
        original_assets_included = $false
        full_extraction_required = $false
        test_status = "PASS"
    } | ConvertTo-Json | Set-Content -Encoding utf8 (Join-Path $stage "build-provenance.json")
    $zip = Join-Path $OutputDir "$name.zip"
    if (Test-Path -LiteralPath $zip) { Remove-Item -LiteralPath $zip -Force }
    Compress-Archive -Path (Join-Path $stage "*") -DestinationPath $zip
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    $archive = [System.IO.Compression.ZipFile]::OpenRead($zip)
    try {
        $expected = @("rloft_native.exe", "README-NATIVE-ASSETS.txt",
                      "SHA256SUMS.txt", "build-provenance.json")
        if ($archive.Entries.Count -ne $expected.Count) {
            throw "Native asset preview ZIP has extra or missing files"
        }
        foreach ($entry in $archive.Entries) {
            if ($expected -cnotcontains $entry.FullName) {
                throw "Native asset ZIP contains unexpected media"
            }
        }
        $binary = $archive.GetEntry("rloft_native.exe")
        if ($null -eq $binary) { throw "Native asset executable missing from ZIP" }
        $stream = $binary.Open()
        $shaAlg = [System.Security.Cryptography.SHA256]::Create()
        try {
            $packedHash = [BitConverter]::ToString($shaAlg.ComputeHash($stream)).Replace("-","").ToLowerInvariant()
        } finally { $shaAlg.Dispose(); $stream.Dispose() }
        if ($packedHash -cne $binaryHash) {
            throw "Native asset executable ZIP SHA256 mismatch"
        }
    } finally { $archive.Dispose() }
    $zipHash = Sha256File $zip
    "$zipHash  $name.zip" | Set-Content -Encoding ascii "$zip.sha256"
    Write-Host "NATIVE_ORIGINAL_ASSET_ENGINE_WINDOWS_BUILD_PASS"
    Write-Host "Source SHA: $sha"
    Write-Host "Tests: PASS"
    Write-Host "EXE SHA256: $binaryHash"
    Write-Host "ZIP SHA256: $zipHash"
    Write-Host "Original-game field verification: PENDING"
} finally { Pop-Location }
