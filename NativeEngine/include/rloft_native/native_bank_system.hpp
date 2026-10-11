#pragma once

#include "rloft_native/native_media.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// Complete independent original-resource bank loader and structural auditor.
// No DemoV1 execution/linkage, no writes to installed original media.
enum class BankRecordKind {
    Empty, NonEh, Raster, Font, Xmidi, Voc, Structured5, Texture, SceneShape,
    UnknownTyped
};

struct NativeBankIssue {
    std::string archive;
    std::size_t record{};
    std::string reason;
};

struct NativeBankSummary {
    std::string archive;
    std::uint64_t original_bytes{};
    std::size_t entry_count{};
    std::size_t validated{};
    std::size_t failed{};
    std::array<std::size_t,8> eh_types{}; // 1..7
    std::size_t empty{};
    std::size_t non_eh{};
    std::size_t raw_wordcode{};
    std::size_t decoded_raster{};
    std::size_t decoded_font{};
    std::size_t decoded_texture{};
    std::size_t decoded_scene_shape{};
    std::size_t decoded_music{};
    std::size_t decoded_sound{};
    std::size_t retained_shipped_anomalies{};
};

struct NativeBankSystemReport {
    std::vector<NativeBankSummary> banks;
    std::vector<NativeBankIssue> issues;
    std::size_t records{};
    std::size_t validated{};
    std::size_t failed{};
    std::size_t absent_banks{};
    std::size_t recognized_eh{};
    bool legacy_included{};
};

// Walks every record of RES0..RES10, optionally RES11/RES12,
// reconstructing and validating all known EH envelope families.
// Ordinary graphics EH1, font EH2, XDIR EH3, VOC EH4, EH6 and EH7
// share a consistent resource dispatch. Non-EH game wordcode is bounded.
// EH7 validates full geometry and per-column display lists; it does NOT
// execute 16-bit SINTAB machine code or claim visual renderer parity.
// Every issue cites its exact archive/index; no silent skipping.
// Optional legacy type4 length anomaly is preserved, not "repaired".
[[nodiscard]] NativeBankSystemReport audit_native_bank_system(
    const NativeMedia& media, bool include_legacy = true);

// Deterministic metadata-only JSON for CI and diagnostic tools. Does not
// embed any original byte payloads, paths to private media, or game art.
[[nodiscard]] std::string native_bank_system_json(
    const NativeBankSystemReport& report);

// A single, typed decoder gate for a resource's actual EH format.
// Validation has a bounded memory budget; decoded payloads are discarded
// unless the caller separately requests that specific resource.
[[nodiscard]] BankRecordKind validate_native_bank_record(
    const std::vector<std::uint8_t>& entry,
    std::string_view archive, std::size_t index,
    bool& shipped_anomaly);

} // namespace rloft_native
