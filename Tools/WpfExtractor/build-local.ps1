<#
Build the native WPF frontend and original Python engine locally on Windows.
Requires Windows, the .NET 10 SDK and Python 3.12 x64 (py launcher).
FFmpeg is optional for cinematic video encoding; extraction still works without.
#>
[CmdletBinding()]
param([switch] $SkipTests, [string] $FFmpegPath = "")
$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
$root = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$engine = Join-Path $root "Tools\anm"
$project = Join-Path $PSScriptRoot "RavenloftExtractor\RavenloftExtractor.csproj"
$publish = Join-Path $PSScriptRoot "build-output\RavenloftExtractor-Windows11-x64"
$zip = Join-Path $PSScriptRoot "build-output\RavenloftExtractor-Windows11-x64.zip"

Write-Host "Building Ravenloft WPF Fluent extractor" -ForegroundColor Cyan
& dotnet --version
if ($LASTEXITCODE -ne 0) { throw "Install the .NET 10 SDK first." }
& py -3.12 --version
if ($LASTEXITCODE -ne 0) { throw "Install Python 3.12 x64 first." }

Push-Location $engine
try {
  & py -3.12 -m pip install -r requirements.txt "pyinstaller==6.22.3"
  if ($LASTEXITCODE -ne 0) { throw "Python dependencies failed." }
  if (-not $SkipTests) {
    & py -3.12 -m unittest discover -p "test_*.py" -v
    if ($LASTEXITCODE -ne 0) { throw "Python regression suite failed." }
  }
  & py -3.12 -m PyInstaller --noconfirm --clean --onefile --console --name RavenloftExtractorBackend wpf_bridge.py
  if ($LASTEXITCODE -ne 0) { throw "Could not freeze the Python extraction backend." }
  & (Join-Path $engine "dist\RavenloftExtractorBackend.exe") --self-test
  if ($LASTEXITCODE -ne 0) { throw "Frozen backend self-test failed." }
} finally { Pop-Location }

if (Test-Path $publish) { Remove-Item $publish -Force -Recurse }
New-Item -ItemType Directory -Force -Path $publish | Out-Null
& dotnet publish $project -c Release -r win-x64 --self-contained true -o $publish
if ($LASTEXITCODE -ne 0) { throw "WPF frontend publish failed." }
Copy-Item (Join-Path $engine "dist\RavenloftExtractorBackend.exe") $publish -Force
Copy-Item (Join-Path $PSScriptRoot "README.md") $publish -Force

if (-not $FFmpegPath) {
  $ffmpeg = Get-Command ffmpeg.exe -ErrorAction SilentlyContinue
  if ($ffmpeg) { $FFmpegPath = $ffmpeg.Source }
}
if ($FFmpegPath -and (Test-Path $FFmpegPath -PathType Leaf)) {
  Copy-Item $FFmpegPath (Join-Path $publish "ffmpeg.exe") -Force
} else {
  Write-Warning "No FFmpeg: frame extraction works; video encoding requires ffmpeg.exe."
}

$exe = Join-Path $publish "RavenloftExtractor.exe"
$backend = Join-Path $publish "RavenloftExtractorBackend.exe"
if (-not (Test-Path $exe) -or -not (Test-Path $backend)) {
  throw "Incomplete publish output."
}
@(
  "WPF Fluent Ravenloft Extractor - Windows 11 x64"
  "Frontend SHA256: $((Get-FileHash $exe -Algorithm SHA256).Hash.ToLowerInvariant())"
  "Backend SHA256: $((Get-FileHash $backend -Algorithm SHA256).Hash.ToLowerInvariant())"
) | Set-Content (Join-Path $publish "BUILD_INFO.txt") -Encoding UTF8
if (Test-Path $zip) { Remove-Item $zip -Force }
Compress-Archive -Path "$publish\*" -DestinationPath $zip -CompressionLevel Optimal
(Get-FileHash $zip -Algorithm SHA256).Hash.ToLowerInvariant() |
  Set-Content "$zip.sha256" -Encoding ASCII
Write-Host "Build complete:" -ForegroundColor Green
Write-Host $zip
