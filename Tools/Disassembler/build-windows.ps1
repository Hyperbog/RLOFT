# Local Windows build. Nothing in this script calls GitHub Actions.
# Run from any directory on Windows 11 with .NET 10 SDK installed.
param(
    [switch]$Publish,
    [string]$OriginalCode1 = '',
    [string]$OriginalCode2 = ''
)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Push-Location $root
try {
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
finally { Pop-Location }
