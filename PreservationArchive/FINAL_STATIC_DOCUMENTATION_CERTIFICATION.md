# Supplied 1.2a static documentation certification

**Decision: PASS, scoped to the currently evidenced DOS 1.2a source.**

The twenty-two numbered chapters (00–21) are present, with consecutive section numbering and 108 valid relative document links. The independently available original-file and extracted-patch SHA-256 checks passed (9/9 and 4/4). The previously certified 33-file resource corpus has been re-identified; fresh outer structure checks passed for 13 RES archives and 13 ANM streams. All 16 A32 driver service-rate descriptors were checked against dispatch-table entries. Chapter 21 contains the original correction and UNKNOWN dispositions.

**Explicit exclusions:** unverified original version 1.0 and CD patches, historical authoring names, observed DOS hardware/IRQ timing, exhaustive original execution, and the separate modern native-port integration. This is a documentation certificate, **not an unconditional claim that the game, all historical revisions, or the native port is 100% complete**.

Original byte evidence remains controlling. The immutable baseline is `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`; the authoritative technical audit is [Chapter 21](21_FINAL_CONTRADICTION_UNKNOWN_AND_EVIDENCE_AUDIT.md).

## Certification interpretation

This final certificate supersedes earlier time-stamped *in-progress* or *not yet certified* status statements in Chapter 21. Those sections remain useful historical audit checkpoints; the most recent verified source evidence is described in Chapter 21 §§19–23 and this certificate. The documented UNKNOWN register (U-001–U-016) and chapter-qualified correction registry (43 records) remain active as preservation records, not unperformed documentation chapters.

**Meaning of 100%:** the planned **22/22 historical static documentation chapters are complete to currently recoverable evidence**, and their source provenance, identified behavior and limitations have been accounted for. The number is a coverage statement for this explicitly bounded documentation workstream; it is not a measurement of runtime test coverage, all original media, or gameplay implementation.

The workstream's final published state is the specific Git commit that contains this certificate. Future commits to the shared `code1-decompilation` branch—including concurrent asset-tool edits—must not be represented as part of this certificate unless independently reviewed. The frozen baseline and current game source binaries remain controlling evidence.

## Post-certification research register

The originally certified **22 historical static chapters (00–21)** remain complete to their defined supplied-1.2a evidence boundary. A distinct forward-looking **[Chapter 22 — Remaining Research and Future Evidence Roadmap](22_REMAINING_RESEARCH_AND_FUTURE_EVIDENCE_ROADMAP.md)** has subsequently been added as an appendix to catalogue **91 not-yet-completed research and original-versus-native verification strands**. That appendix is not part of the already completed original 22-chapter static certification target and does not revoke it. Native execution fidelity, alternate release archaeology, historical symbolic names and measured driver/hardware behavior remain explicitly separate tasks.
