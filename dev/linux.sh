#!/usr/bin/env bash
# Linux-first workspace for the current RLOFT branch. No original media required.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
ACTION="${1:-test}"
JOBS="${RLOFT_JOBS:-2}"
BUILD="$ROOT/build/linux"
if [ -n "${RLOFT_WORKSTREAM:-}" ]; then
  OWNER="$RLOFT_WORKSTREAM"
elif [ -d "$ROOT/Recomp" ]; then
  OWNER="recomp"
elif [ -d "$ROOT/Decomp" ]; then
  OWNER="decomp"
elif [ -d "$ROOT/NativeEngine" ]; then
  OWNER="native-engine"
elif [ -d "$ROOT/Tools/Disassembler/src" ]; then
  OWNER="DosDis"
elif [ -d "$ROOT/Tools/WpfExtractor" ]; then
  OWNER="extractor"
elif [ -d "$ROOT/DemoV1" ]; then
  OWNER="demo-v1"
else
  echo "Cannot identify RLOFT workstream from source tree" >&2
  exit 2
fi
need() { if ! command -v "$1" >/dev/null 2>&1; then echo "Missing $1 for $OWNER" >&2; exit 2; fi; }
doctor() {
  need git
  need python3
  printf 'RLOFT Linux workstream: %s\n' "$OWNER"
  printf 'Root: %s\n' "$ROOT"
  printf 'Git commit: '
  git rev-parse --verify HEAD 2>/dev/null || echo "(not a git checkout)"
  case "$OWNER" in
    recomp|native-engine) need cmake; need ninja; need c++ ;;
    DosDis) need dotnet ;;
    extractor) need python3 ;;
  esac
  echo "Linux environment prerequisites: present"
  if [ "$OWNER" = "demo-v1" ]; then
    echo "Windows Win32/OpenGL/audio player acceptance remains on MSVC/Windows CI."
  fi
  if [ "$OWNER" = "DosDis" ] || [ "$OWNER" = "extractor" ]; then
    echo "Windows WPF UI cannot be runtime-tested on Linux."
  fi
}
config() {
  mkdir -p "$BUILD"
  case "$OWNER" in
    recomp) cmake -S Recomp -B "$BUILD/recomp" -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON ;;
    native-engine) cmake -S NativeEngine -B "$BUILD/native-engine" -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON ;;
    extractor)
      if [ ! -x "$BUILD/extractor-venv/bin/python" ]; then
        python3 -m venv "$BUILD/extractor-venv"
      fi
      ;;
    *) echo "No CMake configure step for $OWNER on Linux." ;;
  esac
}
compile_source() {
  case "$OWNER" in
    recomp) cmake --build "$BUILD/recomp" --parallel "$JOBS" ;;
    native-engine) cmake --build "$BUILD/native-engine" --parallel "$JOBS" ;;
    DosDis) dotnet build Tools/Disassembler/src/DosDis.Core/DosDis.Core.csproj --configuration Release ;;
    extractor) python3 -m compileall -q Tools/anm ;;
    decomp) python3 -m compileall -q ResearchInfra Code2/tools ;;
    demo-v1) python3 -m compileall -q DemoV1/tools ;;
    *) echo "Unknown workstream $OWNER" >&2; exit 2 ;;
  esac
}
tests() {
  case "$OWNER" in
    recomp)
      ctest --test-dir "$BUILD/recomp" --output-on-failure --parallel "$JOBS"
      python3 Recomp/tools/recomp_progress.py --json
      ;;
    native-engine) ctest --test-dir "$BUILD/native-engine" --output-on-failure --parallel "$JOBS" ;;
    decomp)
      python3 -m unittest discover -s docs/tools -p 'test_*.py' -v
      python3 docs/tools/check_docs.py
      python3 Code2/graphics/verify_original_vga.py --self-test
      ;;
    demo-v1)
      python3 DemoV1/tools/verify_enhanced_port_build_pipeline.py
      echo "Only source/build-contract tests on Linux: Win32 game must be checked on Windows."
      ;;
    DosDis)
      dotnet run --project Tools/Disassembler/tests/DosDis.Smoke --configuration Release
      dotnet run --project Tools/Disassembler/tests/DosDis.ProjectSmoke --configuration Release
      echo "WPF desktop is Windows-only; Linux tests cover portable .NET components."
      ;;
    extractor)
      "$BUILD/extractor-venv/bin/python" -m unittest discover -s Tools/anm -p 'test_*.py' -v
      echo "WPF and Windows distribution packaging are Windows-only."
      ;;
  esac
}
case "$ACTION" in
  doctor) doctor ;;
  setup)
    doctor
    config
    if [ "$OWNER" = "extractor" ]; then
      "$BUILD/extractor-venv/bin/python" -m pip install -r Tools/anm/requirements.txt
    fi
    ;;
  configure) doctor; config ;;
  build) doctor; config; compile_source ;;
  test|all) doctor; config; compile_source; tests ;;
  *)
    echo "Usage: bash dev/linux.sh [doctor|setup|configure|build|test]" >&2
    exit 2
    ;;
esac
