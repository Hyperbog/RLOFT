User authorization: 2026-10-09. Run ONE Full Windows build for demo-v1 with all registered CTests. The workflow push trigger filters only this file. Restore manual-only workflow afterward.

Retry 2: fixes committed 24413f71 and 2f7d5d06; run Full Windows build, all 172 CTests. Original failure: workflow run 37983635591.

Retry 3: fixes fdbb3afe, 509ea23d, 5b5c7e77 after #643 compiled all targets but CTest 169/172; rerun Full.

Retry 4: source fixture isolation and Q32 phase corrections f97a641f, 422bd17a, 7cb9d6b1 after Full compiled/169 of 172 assertions passed in #644.

Retry 5: only inventory_runtime_tests failed silently after 171/172 in #645. New termination diagnostics committed 46f73383; rerun authorized Full Windows build.

Retry 5: #646 isolated uncaught OBJECTS.DAT object id in controlled weight fixture; corrected stale pseudo-item ordinary slots with commit f185cbdd. Full Windows 172 tests.
