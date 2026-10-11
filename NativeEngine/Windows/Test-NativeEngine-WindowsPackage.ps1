# Windows-only synthetic integrity regression. Uses dummy bytes, NO real EXEs
# and NO copyrighted assets; proves the package verifier fails closed.
$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
$verifier = Join-Path $PSScriptRoot "Verify-NativeEngine-WindowsPackage.ps1"
$expectedSource = "1111111111111111111111111111111111111111"
$zipName = "RLOFT-NativeEngine-Windows-x64-BOOTSTRAP.zip"

function Assert-VerificationExit([string]$ZipPath, [int]$Expected) {
    # Use a fresh PowerShell process to test actual script exit status.
    & powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File $verifier -Archive $ZipPath -ExpectedSourceSha $expectedSource *>&1 | Out-Null
    $exitStatus = $LASTEXITCODE
    if ($Expected -eq 0 -and $exitStatus -ne 0) {
        throw "Expected valid Windows bootstrap package to pass (exit $exitStatus)"
    }
    if ($Expected -ne 0 -and $exitStatus -eq 0) {
        throw "Corrupted Windows bootstrap package incorrectly accepted"
    }
}
function Package([string]$Stage, [string]$ZipPath) {
    if (Test-Path -LiteralPath $ZipPath) { Remove-Item -LiteralPath $ZipPath -Force }
    Compress-Archive -Path (Join-Path $Stage "*") -DestinationPath $ZipPath
    $zipHash = (Get-FileHash -LiteralPath $ZipPath -Algorithm SHA256).Hash.ToLowerInvariant()
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
        $hash = (Get-FileHash -LiteralPath (Join-Path $stage $name) -Algorithm SHA256).Hash.ToLowerInvariant()
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
    Write-Host "WINDOWS_PACKAGE_INTEGRITY_CONTRACT_PASS (synthetic fixtures)"
} finally {
    if (Test-Path -LiteralPath $folder) { Remove-Item -LiteralPath $folder -Force -Recurse }
}
