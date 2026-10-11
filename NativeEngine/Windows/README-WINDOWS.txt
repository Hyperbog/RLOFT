RAVENLOFT NATIVE ENGINE - WINDOWS BOOTSTRAP CHECKPOINT
=====================================================

THIS IS NOT YET A PLAYABLE GAME. It is the first INDEPENDENT Windows
native-engine application, separate from Ravenloft Demo v1.

Contents:
  rloft_native_bootstrap.exe   Windows GUI: original install discovery,
                               folder picker, source inventory, and clearly
                               labeled synthetic indexed-frame diagnostic.
  rloft_install_probe.exe      Console read-only install inspector.
  build-provenance.json        Exact Git commit and build/test metadata.
  SHA256SUMS.txt               SHA-256 of the Windows executables.

To begin:
  1. Extract the ZIP to an independent folder (not the original game folder).
  2. Run rloft_native_bootstrap.exe.
  3. Click "Select original install folder..." and select the folder of your
     legally owned Strahd's Possession installation. The bootstrap reads
     loose original RES0/INIT/T media or a supported GOG game.gog CD image.
  4. Review the listed resources. "PASS" confirms only a minimal media
     signature; it does NOT certify the version, graphics, level loading or
     gameplay compatibility.
  5. With an installation containing a valid original RES0, INIT and
     COLORS bank, the viewport attempts to show the authentic default
     RES0 cursor raster using original palette 12. This is only a centered
     source-backed RESOURCE DIAGNOSTIC, not the original game scene.
     If decoding is unavailable, the window shows labeled SYNTHETIC TEST
     ART instead. Neither image means a game is running.

You may also invoke from a Windows terminal:
  rloft_native_bootstrap.exe --install "C:\Games\Ravenloft"
  rloft_native_bootstrap.exe --self-test

For read-only console inventory:
  rloft_install_probe.exe --install "C:\Games\Ravenloft"
  rloft_install_probe.exe --auto
  rloft_install_probe.exe --help

Original game files and saves are NOT bundled with this download. The
bootstrap deliberately performs no writes to the original install. Future
real-game saves must use an independent writable profile.

Next project milestone: replace the synthetic test viewport with the
source-faithful original game frame, then integrate character creation,
world simulation and authenticated original gameplay into the separate
standalone Windows native-engine product. Future SDL3/bgfx/SDL_GPU renderers
remain evaluation candidates. This bootstrap is a temporary Win32/GDI
bring-up host, not a permanent graphics-framework commitment.

Known limitations:
  * Windows x64 only. Windows GUI/retail original-media field acceptance
    must be documented on an actual Windows installation.
  * No DOSBox dependency. No running original DOS executables.
  * No original save/load or gameplay execution in this bootstrap.
  * Installer detection/version inventory is partial; no patch-version
    equivalence has been certified.
  * Folder selection uses the modern Windows Common Item Dialog, with
    native Unicode paths rather than the classic MAX_PATH-limited picker.
  * Automated quality assurance also exercises a noninteractive
    --self-test-install <path> path; this is diagnostic-only, not gameplay.
  * Every distributed archive must pass the independent SHA-256,
    exact-source-provenance and asset-free allowlist validation.

Source:
  https://github.com/Hyperbog/RLOFT/tree/native-engine/NativeEngine
Build tracking:
  https://github.com/Hyperbog/RLOFT/issues/9
