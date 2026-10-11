# Windows-only synthetic integrity regression. Uses dummy bytes, NO real EXEs
# and NO copyrighted assets; proves the package verifier fails closed.
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

$verifier = Join-Path $PSScriptRoot "Verify-NativeEngine-WindowsPackage.ps1"
$expectedSource = "1111111111111111111111111111111111111111"
$zipName = "RLOFT-NativeEngine-Windows-x64-BOOTSTRAP.zip"

function Assert-VerificationExit([string]$ZipPath, [int]$Expected) {
    # In-process invocation preserves the verifier's fail-closed exception
    # contract without masking expected negative stderr as test failures.
    $accepted = $true
    try {
        & $verifier -Archive $ZipPath -ExpectedSourceSha $expectedSource | Out-Null
    } catch {
        $accepted = $false
        if ($Expected -eq 0) { throw }
    }
    if ($Expected -eq 0 -and -not $accepted) {
        throw "Expected valid Windows bootstrap package to pass"
    }
    if ($Expected -ne 0 -and $accepted) {
        throw "Corrupted Windows bootstrap package incorrectly accepted"
    }
}
function Package([string]$Stage, [string]$ZipPath) {
    if (Test-Path -LiteralPath $ZipPath) { Remove-Item -LiteralPath $ZipPath -Force }
    Compress-Archive -Path (Join-Path $Stage "*") -DestinationPath $ZipPath
    $zipHash = (Get-RloftFileSha256 -Path $ZipPath)
    "$zipHash  $zipName" | Set-Content -Encoding ascii "$ZipPath.sha256"
}
$folder = Join-Path ([IO.Path]::GetTempPath()) ("rloft-zip-fixture-" + [Guid]::NewGuid().ToString("N"))
$stage = Join-Path $folder "stage"
New-Item -ItemType Directory -Force -Path $stage | Out-Null
try {
    [IO.File]::WriteAllBytes((Join-Path $stage "rloft_native_bootstrap.exe"), [byte[]](0x4d,0x5a,0x01,0x02))
    [IO.File]::WriteAllBytes((Join-Path $stage "rloft_install_probe.exe"), [byte[]](0x4d,0x5a,0x03,0x04))
    "Synthetic test package (never ship)" | Set-Content -Encoding utf8 (Join-Path $stage "README-WINDOWS.txt")
    $metadata = [ordered]@{
        product = "RLOFT native-engine Windows bootstrap ONLY"
        game_playable = $false
        source_branch = "native-engine"
        source_commit = $expectedSource
        build = "MSVC x64 Release"
        test_outcome = "PASS"
        original_assets_included = $false
        generated_utc = "2026-10-10T00:00:00Z"
    }
    $metadata | ConvertTo-Json | Set-Content -Encoding utf8 (Join-Path $stage "build-provenance.json")
    $binaryChecksums = foreach ($name in @("rloft_native_bootstrap.exe", "rloft_install_probe.exe")) {
        $hash = (Get-RloftFileSha256 -Path (Join-Path $stage $name))
        "$hash  $name"
    }
    $binaryChecksums | Set-Content -Encoding ascii (Join-Path $stage "SHA256SUMS.txt")
    $archive = Join-Path $folder $zipName
    Package $stage $archive
    Assert-VerificationExit $archive 0
    # Case 1: altered ZIP checksum sidecar must be rejected.
    ("0" * 64 + "  " + $zipName) | Set-Content -Encoding ascii "$archive.sha256"
    Assert-VerificationExit $archive 1
    # Case 2: correctly checksummed ZIP with unapproved data must fail.
    [IO.File]::WriteAllBytes((Join-Path $stage "OBJECTS.DAT"), [byte[]](1,2,3))
    Package $stage $archive
    Assert-VerificationExit $archive 1
    Remove-Item -LiteralPath (Join-Path $stage "OBJECTS.DAT") -Force

    # Case 3: metadata falsely reporting a playable game must fail even
    # if the attacker correctly updates the outer archive hash.
    $metadata['game_playable'] = $true
    $metadata | ConvertTo-Json | Set-Content -Encoding utf8 (Join-Path $stage "build-provenance.json")
    Package $stage $archive
    Assert-VerificationExit $archive 1
    $metadata['game_playable'] = $false
    $metadata | ConvertTo-Json | Set-Content -Encoding utf8 (Join-Path $stage "build-provenance.json")

    # Case 4: changed executable bytes with stale binary checksum must
    # fail even if the outer ZIP sidecar is correctly regenerated.
    [IO.File]::WriteAllBytes((Join-Path $stage "rloft_install_probe.exe"),
                            [byte[]](0x4d,0x5a,0x09,0x09))
    Package $stage $archive
    Assert-VerificationExit $archive 1
    Write-Host "WINDOWS_PACKAGE_INTEGRITY_CONTRACT_PASS (synthetic fixtures)"
} finally {
    if (Test-Path -LiteralPath $folder) { Remove-Item -LiteralPath $folder -Force -Recurse }
}
