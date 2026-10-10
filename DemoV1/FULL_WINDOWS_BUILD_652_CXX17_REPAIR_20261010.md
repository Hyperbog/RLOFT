# Windows Full #652 MSVC language-standard failure and repair — October 10, 2026

## Failure checkpoint

Authorized Windows Full GitHub Actions [run #38078746990](https://github.com/Hyperbog/RLOFT/actions/runs/38078746990), source checkout `f0c10fbcc2aad1faa3414616c77bb66f74771480`, exited **failure** during `Build full (all C++ regression targets)`. No Windows CTest phase, new executable artifact or original-DOS gameplay acceptance resulted from that run. The original CODE.2 C++17 syntax preflight and CMake Configure passed.

The job reported 92 MSVC diagnostics, mostly cascades from missing language/library features in stand-alone header-only F790 regressions:

- `original_f790_caller_catalog.hpp` uses `std::optional` but standalone `original_f790_caller_catalog_tests` did not specify `cxx_std_17`.
- `original_f790_selected_poll_tests` likewise compiled its optional-returning helper under the compiler default language mode.
- `original_feedback_ribbon_text_tests` and `original_feedback_ribbon_epoch_tests` require `std::string_view` and `std::optional`; subsequent identifier/overload diagnostics were parser cascades.

Each affected header already included the proper `<optional>` or `<string_view>` header. The actual defect was **CMake's language-standard inheritance**: `rloft_boot_flow` specifies public C++17, which propagates only to targets that link to it, while separate standalone tests did not link to that library. The original gameplay implementation and data were not implicated.

## Repair

`DemoV1/CMakeLists.txt` now sets `CMAKE_CXX_STANDARD=17` and `CMAKE_CXX_STANDARD_REQUIRED=ON` immediately after `project()`, so **every** executable/test target uses C++17, including standalone C++ regressions. All previous target-specific C++17 requirements are retained. No compiler flags affecting game physics, original RNG, asset loading or DOS behavior changed. The `original_inventory_f790_cadence_tests` source verifier now asserts that these project-wide requirements exist.

## Verification boundary

Build #652 failed before testing. This change addresses its first independently identified root-cause diagnostics; it must be recompiled and retested in a **new user-authorized Windows Full** run before any pass claim. The original-DOS comparison gate remains unaccepted; subsystem readiness values stay unchanged.

The temporary CI trigger used to initiate #652 was restored to the original manual-only workflow blob `7c8e5720f38c0690d01457ea3daf556124725ef2`, and its marker removed. Any subsequent authorized CI run must likewise restore manual-only status.
