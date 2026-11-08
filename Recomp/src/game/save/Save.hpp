#pragma once
// Original CODE.2 171FC/181B4 private writable WORK/LEVnnn.DAT overlay.
// Never writes to the owner's original game installation or saved files.
// The caller must validate original source filename bytes before open/remove.
// Handles are deliberately outside the host DOS read-only handle range.
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <exception>
#include <utility>
#include <vector>

namespace rloft { namespace live_level_files {
struct Overlay {
    static constexpr std::uint32_t read_handle=0xFFFEu;
    static constexpr std::uint32_t write_handle=0xFFFDu;
    static constexpr std::uint32_t invalid=0xFFFFFFFFu;
    static constexpr std::uint32_t capacity=4u*1024u*1024u;
    std::map<std::uint32_t,std::vector<std::uint8_t>> levels;
    std::uint32_t reading=invalid,writing=invalid;
    std::uint32_t read_position=0u,write_position=0u;
    std::uint32_t source_reads=0u,source_writes=0u;
    bool failed=false;

    static bool level_name(const std::uint8_t* name,std::size_t count,
                           std::uint32_t& level) noexcept ;
    // A saved game may install a prevalidated, owner-private LEVnnn blob
    // BEFORE invoking original 07938/181B4. No source callback, actor,
    // camera, map or original game file is changed by this transport.
    // A failed insertion leaves the source bytes available to its caller
    // and marks this overlay unusable instead of throwing through the
    // noexcept retained DOS callback or half-installing a saved world.
    bool install_saved_snapshot(std::uint32_t level,
                                std::vector<std::uint8_t>& bytes) noexcept ;
    // The same actual noexcept insertion boundary can be exercised with
    // a deterministic throwing map in media-free CTests. This is not a
    // fallback guest snapshot or an allocation bypass in production.
    template<class Map>
    static bool try_install_into(Map& target,std::uint32_t level,
                                 std::vector<std::uint8_t>& bytes) noexcept {
        try {
            return target.try_emplace(level,std::move(bytes)).second;
        }catch(const std::exception&) {
            return false;
        }
    }
    bool has(std::uint32_t level) const noexcept ;
    bool erase(std::uint32_t level) noexcept ;
    std::uint32_t open(std::uint32_t level) noexcept ;
    std::uint32_t create(std::uint32_t level) noexcept ;
    bool close(std::uint32_t handle) noexcept ;
    // CODE.2 3131C carries a signed 32-bit DOS seek displacement.
    std::uint32_t seek(std::uint32_t handle,std::uint32_t off,
                       std::uint32_t origin) noexcept ;
    std::uint32_t write(const std::uint8_t* data,std::uint32_t count) noexcept ;
    std::uint32_t read(std::uint8_t* dst,std::uint32_t count) noexcept ;
    bool idle() const noexcept ;
};
}} // namespace rloft::live_level_files

// Bounded original 181B4 saved-world actor payload verification.
// Source-authenticated standalone WORK level witness for 181B4's saved
// 22-byte actor path, independent of LEVELS.DAT's packed 11-byte path.
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>
namespace rloft { namespace saved_level_actor_proof {
using U=std::uint32_t; using B=std::uint8_t;
struct Proof { U moving=0,primary=0,alternate=0,indexed_columns=0; };
Proof verify(const std::vector<B>& saved,
                    const std::vector<B>& guest,U level) ;
}} // namespace rloft::saved_level_actor_proof

// Original CODE.2 07938 / 181B4 saved-world and transition policy.
// Authentic CODE.2 07938 writes the unsigned -1 sentinel at C6C8
// before 181B4 reads the level number from SAVE.DAT. CODE.2 uses
// LEVELS.DAT's 32-entry index, not a host allowlist of level pairs.
// These predicates authorize bounded SOURCE-selected requests only;
// the caller must also validate real LEVELS.DAT records, live guest
// dimensions, read-only file ownership and source transition state.
#include <cstdint>
namespace rloft { namespace original_saved_level_policy {
constexpr std::uint32_t no_departing_level=0xFFFFFFFFu;
constexpr std::uint32_t archived_level_count=32u;
bool indexed_level(std::uint32_t level) noexcept ;
bool authenticated_no_departing_level(
    std::uint32_t prior,std::uint32_t requested,
    bool restore_active,std::uint32_t source_level_callbacks,
    bool saved_world_available,std::uint32_t source_reads,
    std::uint32_t source_save_opens) noexcept ;
bool source_selected_transition(std::uint32_t prior,
                                       std::uint32_t requested) noexcept ;
}} // namespace rloft::original_saved_level_policy
