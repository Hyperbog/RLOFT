#!/usr/bin/env python3
"""Source-only guard for opt-in Win32 scan observation, not a DOS parity test."""
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]

def verify():
    source=(ROOT/"DemoV1/src/win32_main.cpp").read_text(encoding="utf-8")
    header=(ROOT/"DemoV1/include/rloft/win32_scan_observation.hpp").read_text(encoding="utf-8")
    decoder=(ROOT/"DemoV1/src/win32_scan_observation.cpp").read_text(encoding="utf-8")
    tests=(ROOT/"DemoV1/tests/win32_scan_observation_tests.cpp").read_text(encoding="utf-8")
    cm=(ROOT/"DemoV1/CMakeLists.txt").read_text(encoding="utf-8")
    audit=(ROOT/"DemoV1/WIN32_SCAN_OBSERVATION_20261011.md").read_text(encoding="utf-8")
    body=source.split("    LRESULT handle(UINT msg, WPARAM wparam, LPARAM lparam) {",1)[1]
    observation=body.split("        switch (msg) {",1)[0]
    assert 'if(session_trace_ &&' in observation
    assert 'Win32ScanObservationModel::decode(' in observation
    assert 'session_trace_->event("key.scan_observation"' in observation
    assert "WM_SYSKEYDOWN" in observation and "WM_SYSKEYUP" in observation
    assert "WM_KEYDOWN" in observation and "WM_KEYUP" in observation
    assert 'WM_KEYDOWN:' in body and 'WM_KEYUP:' in body
    assert 'legacy_scan_from_vk(wparam)' in body
    # The tracer cannot become a hidden new DOS IRQ input path.
    for forbidden in ('receive_port60(', 'dispatch_30950(',
                      'OriginalDosKeyboardSession::from_installed_code2('):
        assert forbidden not in source,forbidden
    for token in ("ExtendedSequenceUnresolved","RepeatTimingUnresolved",
                  "InvalidTransition","candidate_port60"):
        assert token in header+decoder+tests,token
    assert "NAME win32_scan_observation_tests" in cm
    assert "NAME win32_scan_observation_contract_tests" in cm
    for token in ("not wired","E0/E1","WM_SYSKEYDOWN","M-PLAY-01","0x60","source-only"):
        assert token in audit,token
    print("SOURCE_ONLY_PASS: opt-in Win32 scan trace preserves native input route")

if __name__=="__main__":
    verify()
