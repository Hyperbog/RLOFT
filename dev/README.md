# RLOFT Linux-first development

Run from a **real local Git checkout** of the owning branch. The ChatGPT container is session-based; push all reviewed source changes to GitHub.

```bash
git clone --branch recomp https://github.com/Hyperbog/RLOFT.git
cd RLOFT
bash dev/linux.sh doctor
bash dev/linux.sh test
```

Substitute `code1-decompilation`, `demo-v1`, `native-engine`, `DosDis`, or `Ravenloft-Asset-Extractor` to test another owning branch. The same script detects the source tree. For the extractor, run `bash dev/linux.sh setup` once to create a private Python virtualenv and install its declared dependencies. For native C++ work, install Git, Python 3, CMake, Ninja and GCC/Clang. For DOSDis, install the .NET 10 SDK. Output goes under ignored `build/linux/`; set `RLOFT_JOBS=2` to control parallelism.

**Test boundaries:** `recomp` compiles C++17 source/link and original-entry tests, **not a playable DOS game**. `demo-v1` performs Python source/build-contract tests on Linux, but the Win32 game, sound, menus and graphical acceptance must be verified on Windows. DOSDis and extractor WPF desktop applications likewise require Windows testing. The experimental native engine's portable code can be compiled under Linux; the real Windows player behavior cannot be certified there.

### Windows recompilation milestones

The `recomp` branch retains its normal Windows/MSVC source-link workflow and offers a [milestone workflow](https://github.com/Hyperbog/RLOFT/blob/recomp/.github/workflows/recomp-milestone-windows.yml) using the R01–R12 rubric. Milestone builds run manually, on milestone-manifest changes and on the first and fifteenth day of each month. They compile, test and package SHA-stamped **engineering** Windows executables when possible. Selecting a milestone does **not** mark it accepted; only separately recorded evidence can change its `accepted` field.
