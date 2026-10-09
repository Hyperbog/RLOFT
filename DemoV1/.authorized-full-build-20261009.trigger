User authorization: 2026-10-09. Run ONE Full Windows build for demo-v1 with all registered CTests. The workflow push trigger filters only this file. Restore manual-only workflow afterward.

Retry 2: fixes committed 24413f71 and 2f7d5d06; run Full Windows build, all 172 CTests. Original failure: workflow run 37983635591.

Retry 3: fixes fdbb3afe, 509ea23d, 5b5c7e77 after #643 compiled all targets but CTest 169/172; rerun Full.
