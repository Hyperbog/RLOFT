# Requires: Windows, Visual Studio C++ x64, CMake, Git and Python 3.
# No game binaries or copyrighted assets are ever packaged.
[CmdletBinding()]
param(
    [string]$BuildDir = "",
    [string]$OutputDir = ""
)
$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
$root = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
if (-not $BuildDir) { $BuildDir = Join-Path $root "build\native-engine-windows-x64" }
if (-not $OutputDir) { $OutputDir = Join-Path $root "NativeEngine\Windows\dist" }

Push-Location $root
try {
    $sha = (& git rev-parse --verify HEAD).Trim()
    if ($LASTEXITCODE -ne 0 -or $sha -notmatch '^[0-9a-f]{40}$') {
        throw "Cannot determine exact checked-out Git revision."
    }
    Write-Host "NativeEngine source checkout: $sha"
    & cmake -S NativeEngine -B $BuildDir -G "Visual Studio 17 2022" -A x64 "-DRLOFT_NATIVE_SOURCE_SHA=$sha"
    if ($LASTEXITCODE -ne 0) { throw "Windows x64 CMake configure failed" }
    & cmake --build $BuildDir --config Release --parallel 2
    if ($LASTEXITCODE -ne 0) { throw "Windows x64 MSVC compilation failed" }
    $compiled = Join-Path $BuildDir "Release"
    $bootstrap = Join-Path $compiled "rloft_native_bootstrap.exe"
    $inspector = Join-Path $compiled "rloft_install_probe.exe"
    foreach ($binary in @($bootstrap, $inspector)) {
        if (-not (Test-Path $binary)) { throw "Expected Windows EXE missing: $binary" }
    }
    # GUI-subsystem executables may return before completing under a shell '&'.
    # Start-Process -Wait captures the actual exit status deterministically.
    $smoke = Start-Process -FilePath $bootstrap -ArgumentList "--self-test" -PassThru -NoNewWindow
    if (-not $smoke.WaitForExit(20000)) {
        $smoke.Kill()
        throw "Windows GUI bootstrap self-test timed out"
    }
    if ($smoke.ExitCode -ne 0) { throw "Windows GUI bootstrap self-test failed: $($smoke.ExitCode)" }
    & $inspector --help
    if ($LASTEXITCODE -ne 0) { throw "Windows install inspector self-test failed" }
    # There is intentionally no release "skip tests" switch.
    & ctest --test-dir $BuildDir -C Release --output-on-failure --parallel 2
    if ($LASTEXITCODE -ne 0) { throw "Windows CTest suite failed" }
    $testStatus = "PASS"
    New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null
    $stage = Join-Path $OutputDir "RLOFT-NativeEngine-Windows-x64-BOOTSTRAP"
    if (Test-Path $stage) { Remove-Item -Force -Recurse $stage }
    New-Item -ItemType Directory -Force -Path $stage | Out-Null
    Copy-Item -LiteralPath $bootstrap -Destination (Join-Path $stage "rloft_native_bootstrap.exe")
    Copy-Item -LiteralPath $inspector -Destination (Join-Path $stage "rloft_install_probe.exe")
    Copy-Item -LiteralPath (Join-Path $root "NativeEngine\Windows\README-WINDOWS.txt") -Destination (Join-Path $stage "README-WINDOWS.txt")
    $shaLines = foreach ($binaryName in @("rloft_native_bootstrap.exe","rloft_install_probe.exe")) {
        $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $stage $binaryName)).Hash.ToLowerInvariant()
        "$hash  $binaryName"
    }
    $shaLines | Set-Content -Encoding ascii (Join-Path $stage "SHA256SUMS.txt")
    $manifest = [ordered]@{
        product = "RLOFT native-engine Windows bootstrap ONLY"
        game_playable = $false
        source_branch = "native-engine"
        source_commit = $sha
        build = "MSVC x64 Release"
        test_outcome = $testStatus
        original_assets_included = $false
        generated_utc = (Get-Date).ToUniversalTime().ToString("o")
    }
    $manifest | ConvertTo-Json -Depth 4 | Set-Content -Encoding utf8 (Join-Path $stage "build-provenance.json")
    $archive = Join-Path $OutputDir "RLOFT-NativeEngine-Windows-x64-BOOTSTRAP.zip"
    if (Test-Path $archive) { Remove-Item -Force $archive }
    Compress-Archive -Path (Join-Path $stage "*") -DestinationPath $archive -CompressionLevel Optimal
    if (-not (Test-Path $archive)) { throw "No Windows release ZIP was created" }
    $bundleHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $archive).Hash.ToLowerInvariant()
    "$bundleHash  RLOFT-NativeEngine-Windows-x64-BOOTSTRAP.zip" | Set-Content -Encoding ascii (Join-Path $OutputDir "RLOFT-NativeEngine-Windows-x64-BOOTSTRAP.zip.sha256")
    # Validate the actual ZIP on disk, not only the loose staging folder.
    & (Join-Path $PSScriptRoot "Verify-NativeEngine-WindowsPackage.ps1") -Archive $archive -ExpectedSourceSha $sha
    if (-not $?) { throw "Windows ZIP post-packaging validation failed" }
    Write-Host "Windows bootstrap package: $archive"
    Write-Host "SHA256: $bundleHash"
    Write-Host "Exact source: $sha"
    Write-Host "CTest status: $testStatus"
    Write-Host "The Windows bootstrap does NOT yet run the original game."
} finally {
    Pop-Location
}
