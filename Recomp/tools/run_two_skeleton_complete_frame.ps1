# Read-only, original-source two-skeleton full-frame capture.
# Outputs remain private. Neither the save nor original graphics are committed.
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$SaveZip,
    [Parameter(Mandatory=$true)][string]$GameDataDir,
    [Parameter(Mandatory=$true)][string]$ExecutableDir,
    [Parameter(Mandatory=$true)][string]$OutputDir,
    [string]$DosIndex = "",
    [string]$DosPalette = "",
    [string]$ComparatorPath = "",
    [string]$Python = "python"
)
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$expectedSave = "28a4b28473997d2682d0a4671e104b13fbad078eea49b0369205ed8ed3669542"
$expectedLevel = "b058964ce39d931e89417ef7f36f93640e48c57cbfdc9d5b363457fba238a672"
$expectedCode2 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"

function Require-File([string]$path) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Required file not found: $path"
    }
    return (Resolve-Path -LiteralPath $path).Path
}
function Require-Sha256([string]$path, [string]$expected) {
    $actual = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actual -ne $expected) {
        throw "Original file SHA256 mismatch for $([IO.Path]::GetFileName($path)): $actual"
    }
    return $actual
}
function Single-ZipEntry([string]$root, [string]$name) {
    $hits = @(Get-ChildItem -LiteralPath $root -File -Recurse |
        Where-Object { $_.Name -ieq $name })
    if ($hits.Count -gt 1) { throw "Ambiguous archived $name ($($hits.Count) matches)" }
    if ($hits.Count -eq 0) { return $null }
    return $hits[0].FullName
}

$SaveZip = Require-File $SaveZip
$GameDataDir = (Resolve-Path -LiteralPath $GameDataDir).Path
$ExecutableDir = (Resolve-Path -LiteralPath $ExecutableDir).Path
$exe = Require-File (Join-Path $ExecutableDir "rloft_original_code2_world_frame.exe")
if (($DosIndex -eq "") -xor ($DosPalette -eq "")) {
    throw "Both -DosIndex and -DosPalette are required for a DOS comparison."
}
if ($DosIndex -ne "") {
    $DosIndex = Require-File $DosIndex
    $DosPalette = Require-File $DosPalette
    $ComparatorPath = Require-File $ComparatorPath
}
if (Test-Path -LiteralPath $OutputDir) {
    if (@(Get-ChildItem -LiteralPath $OutputDir -Force).Count -ne 0) {
        throw "OutputDir must be a new or empty directory; original captures cannot be overwritten."
    }
} else {
    $null = New-Item -ItemType Directory -Path $OutputDir -Force
}
$OutputDir = (Resolve-Path -LiteralPath $OutputDir).Path
$stage = Join-Path $OutputDir ".private-original-stage"
$unzipped = Join-Path $stage "zip"
$media = Join-Path $stage "media"
$null = New-Item -ItemType Directory -Path $unzipped,$media -Force

try {
    Expand-Archive -LiteralPath $SaveZip -DestinationPath $unzipped
    $save = Single-ZipEntry $unzipped "SAVE.DAT"
    if ($null -eq $save) { throw "Archive has no original SAVE.DAT." }
    $null = Require-Sha256 $save $expectedSave
    Copy-Item -LiteralPath $save -Destination (Join-Path $media "SAVE.DAT")

    $level = Single-ZipEntry $unzipped "LEV005.DAT"
    if ($null -eq $level) {
        $level = Require-File (Join-Path $GameDataDir "LEV005.DAT")
    }
    $null = Require-Sha256 $level $expectedLevel
    Copy-Item -LiteralPath $level -Destination (Join-Path $media "LEV005.DAT")

    foreach ($name in @("SINTAB", "RES4", "COLORS", "CODE.2", "INIT",
                        "RES0", "NPCS.DAT", "OBJECTS.DAT", "RES3", "RES1")) {
        Copy-Item -LiteralPath (Require-File (Join-Path $GameDataDir $name)) -Destination (Join-Path $media $name)
    }
    $null = Require-Sha256 (Join-Path $media "CODE.2") $expectedCode2

    $stem = Join-Path $OutputDir "two_skeleton_complete_native"
    $nativeLog = Join-Path $OutputDir "native_source_full_frame.log"
    $nativeArgs = @(
        "--save-complete-frame",
        (Join-Path $media "SAVE.DAT"),
        (Join-Path $media "LEV005.DAT"),
        (Join-Path $media "SINTAB"),
        (Join-Path $media "RES4"),
        (Join-Path $media "COLORS"),
        (Join-Path $media "CODE.2"),
        (Join-Path $media "INIT"),
        (Join-Path $media "RES0"),
        (Join-Path $media "NPCS.DAT"),
        (Join-Path $media "OBJECTS.DAT"),
        (Join-Path $media "RES3"),
        $stem,
        (Join-Path $media "RES1")
    )
    # Windows PowerShell 5.1 promotes native stderr into a terminating
    # NativeCommandError when ErrorActionPreference=Stop. Capture both
    # streams while retaining the real executable exit code.
    $ErrorActionPreference = "Continue"
    try {
        & $exe @nativeArgs 2>&1 | Out-File -LiteralPath $nativeLog -Encoding utf8
        $nativeExit = $LASTEXITCODE
    } finally {
        $ErrorActionPreference = "Stop"
    }
    if ($nativeExit -ne 0) {
        # Re-execute the *same original save* with its previously successful
        # sprite-only path. This distinguishes scene composition from source
        # sprite decoding without manufacturing visibility or changing files.
        $isolatedLog = Join-Path $OutputDir "native_isolated_sprite_check.log"
        $isolatedStem = Join-Path $OutputDir "two_skeleton_isolated_check"
        # --save-sprite-probe retains the legacy 12-file contract;
        # the extra RES1 argument belongs only to the 1F494 full frame.
        $isolatedArgs = @($nativeArgs[0..($nativeArgs.Count - 2)])
        $isolatedArgs[0] = "--save-sprite-probe"
        $isolatedArgs[$isolatedArgs.Count - 1] = $isolatedStem
        $ErrorActionPreference = "Continue"
        try {
            & $exe @isolatedArgs 2>&1 | Out-File -LiteralPath $isolatedLog -Encoding utf8
            $isolatedExit = $LASTEXITCODE
        } finally {
            $ErrorActionPreference = "Stop"
        }
        $isolatedLogText = Get-Content -LiteralPath $isolatedLog -Raw
        $isolatedPass = ($isolatedExit -eq 0 -and
            $isolatedLogText -match 'CODE2_GENUINE_RES3_SOURCE_PROBE level=5 source_36cec_calls=2 raster_indexed_writes=5515')
        $diagnostic = @(Get-Content -LiteralPath $nativeLog |
            Select-String -Pattern 'CODE2_SOURCE_36CEC_RASTER_FAILURE|CODE2_SINTAB_POST_WORLD_PRE_SPRITE_STATE|CODE2_AUTHENTIC_WORLD_RES4_FRAME_FAIL' |
            Select-Object -Last 5 | ForEach-Object { $_.Line })
        @(
            "FULL_FRAME_EXIT=$nativeExit",
            "ISOLATED_SPRITE_EXIT=$isolatedExit",
            "ISOLATED_TWO_SKELETON_RASTER_PASS=$isolatedPass",
            "FULL_FRAME_LOG=$nativeLog",
            "ISOLATED_SPRITE_LOG=$isolatedLog"
        ) | Set-Content -LiteralPath (Join-Path $OutputDir "native_failure_differential.txt") -Encoding UTF8
        throw "Complete source frame failed; isolated sprite succeeds=$isolatedPass. $($diagnostic -join '; '); see native_failure_differential.txt and the two private logs in $OutputDir"
    }
    $log = Get-Content -LiteralPath $nativeLog -Raw
    $marker = [regex]::Match($log,
        "CODE2_GENUINE_SAVED_SOURCE_FULL_FRAME_PASS level=5 source_36cec_calls=(\d+) raster_indexed_writes=(\d+) off_aperture_cpu_writes=(\d+) frame_indices=128000 palette_rgb6=768 DOS_VGA_PARITY=NOT_VERIFIED")
    if (-not $marker.Success) { throw "Missing native complete-frame acceptance marker; see $nativeLog" }
    $calls = [int]$marker.Groups[1].Value
    $writes = [int]$marker.Groups[2].Value
    $offscreenWrites = [int]$marker.Groups[3].Value
    if ($calls -ne 2 -or $writes -ne 5515) {
        throw "The original two-skeleton source fingerprint changed: calls=$calls writes=$writes (expected 2/5515)"
    }
    if ($log -notmatch "primary_cells_visible=2") {
        throw "Missing two original ray-visible primary actors in source frame."
    }
    if ($log -notmatch "primary_actor_callback_matches=2") {
        throw "The two original actors were not both projected."
    }
    if ($log -notmatch "CODE2_ORIGINAL_1F8FC_2E510_RES0_GAMEPLAY_FRAME_BASE_PASS res0_loads=2 frame_blits=400") {
        throw "Original 1F8FC gameplay frame base was not reconstructed (expected two RES0 resources and 400 source image frames)."
    }
    # The original 36CEC callbacks are not themselves proof that a
    # skeleton is on screen. Each callback must independently change
    # pixels on the active 320x400 VGA page inside the 256-row world.
    $visibleSprites = @([regex]::Matches($log,
        'CODE2_SOURCE_RES3_SPRITE_ACTIVE_SCANOUT sprite_call=(\d+) res3_id=(\d+) source_x=(\d+) projected_size=(\d+) active_page=(\d+) changed_pixels=(\d+) world_rows_changed=(\d+) bbox=(\d+),(\d+),(\d+),(\d+)'))
    if ($visibleSprites.Count -ne 2) {
        throw "Expected two independently measured active-page sprite scanouts; got $($visibleSprites.Count). See $nativeLog"
    }
    $spriteDescriptions = @()
    foreach ($match in $visibleSprites) {
        $callNum = [int]$match.Groups[1].Value
        $changedPixels = [int]$match.Groups[6].Value
        $worldPixels = [int]$match.Groups[7].Value
        $minX = [int]$match.Groups[8].Value
        $minY = [int]$match.Groups[9].Value
        $maxX = [int]$match.Groups[10].Value
        $maxY = [int]$match.Groups[11].Value
        if ($changedPixels -le 0 -or $worldPixels -le 0 -or
            $minX -lt 32 -or $maxX -gt 287 -or
            $minY -ge 256 -or $minY -gt $maxY) {
            throw "Sprite call $callNum did not establish visible source pixels within the original first-person viewport: changed=$changedPixels world=$worldPixels bbox=$minX,$minY,$maxX,$maxY"
        }
        $spriteDescriptions += "SPRITE_$($callNum)_VISIBILITY=$changedPixels world=$worldPixels bbox=$minX,$minY,$maxX,$maxY"
    }
    $panel = [regex]::Match($log,
        'CODE2_ORIGINAL_1F494_1EA90_PARTY_HUD active_original_characters=(\d+) source_state_lut_nonzero=(\d+) resource_calls=(\d+) masked_298f0_2a584=(\d+) direct_2975c=(\d+) fills_29aa4=(\d+) borders_2e374=(\d+) hand_slots_1ea90=(\d+) decimal_2e6f0=(\d+) centered_30508=(\d+) glyph_303c8=(\d+) unsupported_text=(\d+) callback_failure=(\d+)')
    if (-not $panel.Success) {
        throw "Missing original 1F494/1EA90 party compositor evidence in $nativeLog"
    }
    $activeCharacters = [int]$panel.Groups[1].Value
    $lutNonzero = [int]$panel.Groups[2].Value
    $maskedDraws = [int]$panel.Groups[4].Value
    $handCalls = [int]$panel.Groups[8].Value
    $decimalText = [int]$panel.Groups[9].Value
    $centeredText = [int]$panel.Groups[10].Value
    $glyphDraws = [int]$panel.Groups[11].Value
    $unsupportedText = [int]$panel.Groups[12].Value
    $panelFailure = [int]$panel.Groups[13].Value
    if ($activeCharacters -ne 4 -or $lutNonzero -eq 0 -or
        $maskedDraws -lt 8 -or $handCalls -ne 8 -or
        $decimalText -lt 2 -or $decimalText -ne $centeredText -or
        $glyphDraws -lt $centeredText -or
        $unsupportedText -ne 0 -or $panelFailure -ne 0) {
        throw "Original party HUD incomplete: actors=$activeCharacters LUT=$lutNonzero masked=$maskedDraws hand_calls=$handCalls decimal=$decimalText centered=$centeredText glyphs=$glyphDraws unsupported=$unsupportedText failure=$panelFailure"
    }
    $idx = Require-File ($stem + ".idx")
    $pal = Require-File ($stem + ".pal")
    $bmp = Require-File ($stem + ".bmp")
    if ((Get-Item -LiteralPath $idx).Length -ne 128000) {
        throw "Native indexed framebuffer must contain exactly 128000 bytes."
    }
    if ((Get-Item -LiteralPath $pal).Length -ne 768) {
        throw "Native RGB6 DAC palette must contain exactly 768 bytes."
    }
    $palette = [IO.File]::ReadAllBytes($pal)
    if (@($palette | Where-Object { $_ -gt 63 }).Count -gt 0) {
        throw "Native palette contains invalid RGB6 channels."
    }
    $idxHash = (Get-FileHash -LiteralPath $idx -Algorithm SHA256).Hash.ToLowerInvariant()
    $palHash = (Get-FileHash -LiteralPath $pal -Algorithm SHA256).Hash.ToLowerInvariant()

    @(
        "CODE2_PRIVATE_COMPLETE_FRAME_VALIDATED=YES",
        "ORIGINAL_SAVE_SHA256=$expectedSave",
        "ORIGINAL_LEVEL_SHA256=$expectedLevel",
        "SOURCE_ACTOR_36CEC_CALLS=$calls",
        "SOURCE_369D8_INDEXED_WRITES=$writes",
        "SOURCE_369D8_OFF_APERTURE_CPU_WRITES=$offscreenWrites",
        "SOURCE_RES0_FRAME_BASE_IMAGES=400",
        "TWO_SKELETONS_ACTIVE_WORLD_SCANOUT_VISIBLE=YES",
        "ORIGINAL_1F494_PARTY_CHARACTERS=$activeCharacters",
        "ORIGINAL_298F0_2A584_MASKED_BLITS=$maskedDraws",
        "ORIGINAL_1EA90_HAND_SLOTS=$handCalls",
        "ORIGINAL_1F494_LUT_NONZERO_BYTES=$lutNonzero",
        "ORIGINAL_2E6F0_DECIMAL_FORMATS=$decimalText",
        "ORIGINAL_30508_CENTERED_TEXT_CALLS=$centeredText",
        "ORIGINAL_303C8_SOURCE_GLYPH_DRAWS=$glyphDraws",
        "ORIGINAL_PARTY_HUD_COMPLETE=NOT_DOS_VERIFIED",
        $spriteDescriptions,
        "NATIVE_FRAME_SHA256=$idxHash",
        "NATIVE_PALETTE_SHA256=$palHash",
        "DOS_VGA_PARITY=NOT_VERIFIED",
        "FRAME_IDX=$idx",
        "FRAME_PAL=$pal",
        "FRAME_PREVIEW_BMP=$bmp"
    ) | Set-Content -LiteralPath (Join-Path $OutputDir "private_validation_receipt.txt") -Encoding UTF8

    if ($DosIndex -ne "") {
        if ([IO.Path]::GetFullPath($DosIndex) -eq [IO.Path]::GetFullPath($idx) -or
            [IO.Path]::GetFullPath($DosPalette) -eq [IO.Path]::GetFullPath($pal)) {
            throw "DOS reference must not be the native reconstruction."
        }
        $report = Join-Path $OutputDir "dos_native_comparison.json"
        & $Python $ComparatorPath --dos-index $DosIndex --native-index $idx --dos-palette $DosPalette --native-palette $pal --report $report
        if ($LASTEXITCODE -ne 0) {
            throw "DOS/native pixel or palette divergence; see $report"
        }
        Write-Host "BYTES_EQUAL=YES; DOS_CAPTURE_PROVENANCE_REQUIRES_INDEPENDENT_ATTESTATION"
    } else {
        Write-Host "DOS comparison not run: no independent DOS indexed-frame and DAC captures supplied."
    }
    Write-Host "PRIVATE_NATIVE_FULL_FRAME_PASS calls=$calls writes=$writes"
    Write-Host "Owner-local preview: $bmp"
    Write-Host "Receipt: $(Join-Path $OutputDir 'private_validation_receipt.txt')"
} finally {
    # Original save/level/resources are temporary; never publish or alter.
    if (Test-Path -LiteralPath $stage) {
        Remove-Item -LiteralPath $stage -Recurse -Force
    }
}
