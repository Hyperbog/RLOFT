## Mandatory delivery-first rule — applies to the entire Git portfolio

**Scope:** RLOFT, CVIII, Ys, Phantasy Star, and every other repository or workstream. This is a hard instruction for AI coding/research agents, not an optional preference. Existing source-authentication, copyright, safety, branch-ownership, and required acceptance gates remain binding.

1. **Deliver functioning code, not a procession of tests.** Begin with the requested user-visible end state. Identify the causal defect, make the smallest principled production-level correction, and integrate it. Test infrastructure, static inventories, synthetic fixtures, reports, and experimental variants are supporting tools, **not completed features or progress milestones** by themselves.
2. **Hard cap on exploratory loops.** Reuse the code, logs, recordings, and existing tests first. Allow **at most one baseline reproduction and two focused diagnostic experiments per unresolved defect**. Each experiment must decide a specific implementation choice. After that, **stop experimenting**: implement the best source-supported correction, switch to a fundamentally different implementation strategy, or report the precise blocker. Do not create new named stages/checkpoints to evade this limit.
3. **No speculative piles of variants.** Do not keep adjusting constants, offsets, timing, flags, emulator settings, or synthetic tests until one happens to match. Do not build adjacent diagnostic frameworks where direct product implementation is possible. A narrow diagnostic is justified only when it directly changes the next code decision.
4. **One integrated fix, then essential proof.** Validate the changed functionality with a focused regression and the existing relevant end-to-end/CI gates. Preserve independent original-vs-native evidence where required. **After two non-improving implementation/validation attempts, stop that approach**; isolate the blocker and change strategy rather than loop.
5. **Progress means real capability.** Never call a milestone successful merely because tools compile, CI smoke passes, an experimental replay looks better, or a report is committed. Claim “fixed,” “working,” “playable,” or “certified” **only when the required real acceptance behavior passes**. Otherwise say “partial” or “blocked” with exact evidence. Do not inflate source-match or gameplay metrics.
6. **Prioritize the highest-value next action.** When blocked, make one concrete, useful code deliverable if possible, state the exact impediment and necessary next decision, and stop. No busywork, no routine micro-commits, no repetitive status theater, and no requests for new user playthroughs when existing inputs suffice.
7. **Brief completion reporting.** Report what actually changed, the end-to-end result, what remains broken, and the next causal fix. Separate user-performed tests from internal/CI tests. Do not promise work that has not run.

**Non-negotiable principle:** **Fix the real problem. Prove the fix. Move the project forward. Stop the test rabbit hole.** The user must explicitly authorize an extended exploratory investigation beyond the limits above; such authorization never permits weakening mandatory safety, authenticity, or regression checks.

---

# Repository agent instructions

