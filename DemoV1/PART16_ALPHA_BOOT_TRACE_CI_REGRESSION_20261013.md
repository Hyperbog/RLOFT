# CI regression correction — Alpha first-logo guard

Windows Full [#680](https://github.com/Hyperbog/RLOFT/actions/runs/38349772543)
compiled all C++ targets and passed **259/260 CTests**, including the
new `alpha_boot_stage_trace_contract_tests`, but the existing
`alpha_title_startup_order_contract_tests` failed.

Cause: the older test demanded the literal adjacent lines
`if (!skipped) {` and `const auto deadline=...` before
`presenter.first_logo()`. The opt-in `startup_event("boot.splash.first")`
marker introduced between those lines changed formatting without
changing the tested control branch.

The corrected verifier tests the actual enclosing `if (!skipped)`
region before the first logo, requires the original deadline and
the optional diagnostic stage inside that region, and mutates the
guard by its source position. It continues to reject all six
negative variants of cinematic skip, mouse hit-testing, menu action,
CHARS reread and mistaken Alpha-to-Level23 requirements.

This is source contract repair, not a gameplay change or a claim
that DOS startup fidelity has been accepted.
