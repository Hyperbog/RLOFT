# Local Windows build. Nothing in this script calls GitHub Actions.
# Run from any directory on Windows 11 with .NET 10 SDK installed.
param(
    [switch]$Publish,
    [string]$OriginalCode1 = '',
    [string]$OriginalCode2 = '',
    [string]$LocalLogDirectory = ''
)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Push-Location $root
$transcriptStarted = $false
try {
    if ($LocalLogDirectory -ne '') {
        $directory = [System.IO.Path]::GetFullPath($LocalLogDirectory)
        New-Item -Path $directory -ItemType Directory -Force | Out-Null
        $transcript = Join-Path $directory ("DOSDis-Windows-build-" +
            [DateTimeOffset]::UtcNow.ToString('yyyyMMddTHHmmssfff') + '.txt')
        Start-Transcript -Path $transcript -NoClobber -ErrorAction Stop | Out-Null
        $transcriptStarted = $true
        Write-Warning 'Local build transcript can include file paths and machine details. Review before sharing.'
    }
    if (-not (Get-Command dotnet -ErrorAction SilentlyContinue)) {
        throw '.NET SDK was not found. Install .NET 10 SDK for Windows and rerun the local build.'
    }
    & dotnet --version
    if ($LASTEXITCODE -ne 0) { throw 'Could not query installed .NET SDK.' }
    function Invoke-Dotnet([string[]] $Arguments) {
        & dotnet @Arguments
        if ($LASTEXITCODE -ne 0) { throw "dotnet $($Arguments -join ' ') failed ($LASTEXITCODE)" }
    }
    Invoke-Dotnet -Arguments @('build', 'src/DosDis.Desktop/DosDis.Desktop.csproj', '-c', 'Release')
    Invoke-Dotnet -Arguments @('build', 'src/DosDis.Cli/DosDis.Cli.csproj', '-c', 'Release')
    Invoke-Dotnet -Arguments @('run', '--project', 'tests/DosDis.Smoke', '--configuration', 'Release')
    Invoke-Dotnet -Arguments @('run', '--project', 'tests/DosDis.ProjectSmoke', '--configuration', 'Release')
    if (($OriginalCode1 -ne '') -xor ($OriginalCode2 -ne '')) {
        throw 'Provide both -OriginalCode1 and -OriginalCode2 to run original game regression.'
    }
    if ($OriginalCode1 -ne '') {
        Invoke-Dotnet -Arguments @('run', '--project', 'tests/DosDis.Smoke', '--configuration', 'Release', '--', $OriginalCode1, $OriginalCode2)
        Invoke-Dotnet -Arguments @('run', '--project', 'tests/DosDis.ProjectSmoke', '--configuration', 'Release', '--', $OriginalCode1, $OriginalCode2)
    }
    if ($Publish) {
        Invoke-Dotnet -Arguments @('publish', 'src/DosDis.Desktop/DosDis.Desktop.csproj', '-c', 'Release',
            '-r', 'win-x64', '--self-contained', 'true', '--output', 'dist/windows-desktop')
        Invoke-Dotnet -Arguments @('publish', 'src/DosDis.Cli/DosDis.Cli.csproj', '-c', 'Release',
            '-r', 'win-x64', '--self-contained', 'true', '-p:PublishSingleFile=true', '--output', 'dist/windows-cli')
        Copy-Item README.md,THIRD_PARTY_NOTICES.md -Destination dist/windows-cli/ -ErrorAction Stop
    }
    Write-Host 'Local build and tests complete.' -ForegroundColor Green
}
finally {
    if ($transcriptStarted) { Stop-Transcript | Out-Null }
    Pop-Location
}
