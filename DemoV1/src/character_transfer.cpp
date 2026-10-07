#include "rloft/character_transfer.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

constexpr std::size_t kRecordSize = 0x248U;

std::uint32_t le32(const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}

void put16(
    CharacterTransferImage& out,
    std::size_t at,
    std::uint16_t value) {
    if (at + 1U >= out.size()) {
        throw std::out_of_range("CHARS write exceeds 0x920 image");
    }
    out[at] = static_cast<std::uint8_t>(value);
    out[at + 1U] = static_cast<std::uint8_t>(value >> 8U);
}

std::uint16_t get16(
    const CharacterTransferImage& out,
    std::size_t at) {
    return static_cast<std::uint16_t>(out[at]) |
           (static_cast<std::uint16_t>(out[at + 1U]) << 8U);
}

std::uint8_t class_family(
    const CharacterDraft& draft,
    std::uint8_t incoming) {
    const int primary = draft.classes[0];
    const int second = draft.classes[1];

    if (primary < 0 || primary > 5) {
        throw std::logic_error("CHARS actor has invalid primary class");
    }
    if (second == -1) return static_cast<std::uint8_t>(primary);

    const int third = draft.classes[2];
    if (third != -1) {
        return static_cast<std::uint8_t>(
            primary == 5 || second == 5 || third == 5 ? 0x0D : 0x0E);
    }

    std::uint8_t out = incoming;
    if (primary == 0) {
        if (second == 5) out = 6;
        if (second == 3) out = 7;
        if (second == 4) out = 8;
    }
    if (primary == 5) {
        if (second == 0) out = 6;
        if (second == 3) out = 9;
        if (second == 4) out = 0x0C;
    }
    if (primary == 2) out = 0x0A;
    if (primary == 3) {
        if (second == 0) out = 7;
        if (second == 5) out = 9;
        if (second == 4) out = 0x0B;
        if (second == 2) out = 0x0A;
    }
    if (primary == 4) {
        if (second == 0) out = 8;
        if (second == 3) out = 0x0B;
        if (second == 5) out = 0x0C;
    }
    return out;
}

void validate_actor(
    const CharacterTransferActor& actor,
    int index) {
    const auto& d = actor.draft;
    if (d.actor != index ||
        d.sex < 0 || d.sex > 1 ||
        d.race < 0 || d.race > 5 ||
        d.classes[0] < 0 || d.classes[0] > 5 ||
        d.option < 0 || d.option > 5 ||
        d.face < 0 || d.face > 16 ||
        d.name.size() > 15U) {
        throw std::logic_error(
            "actor is incomplete for CODE.1 CHARS handoff");
    }

    for (const int cls : d.classes) {
        if (cls < -1 || cls > 5) {
            throw std::logic_error("CHARS actor has invalid class tuple");
        }
    }
    for (const int value : actor.stats.abilities) {
        if (value < 0 || value > 255) {
            throw std::logic_error("CHARS actor ability exceeds byte");
        }
    }
    for (const int level : actor.stats.levels) {
        if (level < 0 || level > 0xFFFF) {
            throw std::logic_error("CHARS actor level exceeds word");
        }
    }
    if (actor.stats.exceptional_strength < 0 ||
        actor.stats.exceptional_strength > 255 ||
        actor.stats.base_6810 < 0 ||
        actor.stats.base_6810 > 0xFFFF ||
        actor.stats.hit_points < -32768 ||
        actor.stats.hit_points > 32767) {
        throw std::logic_error("CHARS actor generated field is out of range");
    }
}

}  // namespace

CharacterTransferTables CharacterTransferTables::from_assets(
    const RavenloftBootAssets& assets) {
    CharacterTransferTables tables;

    for (std::size_t family = 0;
         family < tables.family_race_words.size();
         ++family) {
        for (std::size_t race = 0;
             race < tables.family_race_words[family].size();
             ++race) {
            for (std::size_t word = 0; word < 14U; ++word) {
                tables.family_race_words[family][race][word] =
                    assets.code1_u16(
                        0x0988U +
                        family * 0xA8U +
                        race * 0x1CU +
                        word * 2U);
            }
        }
    }

    const auto objects = assets.source().read("OBJECTS.DAT");
    if (objects.size() < 4U) {
        throw std::runtime_error("OBJECTS.DAT is truncated");
    }

    const auto count = static_cast<std::size_t>(le32(objects.data()));
    const auto required = 4ULL + static_cast<unsigned long long>(count) * 65ULL;
    if (required > objects.size()) {
        throw std::runtime_error("OBJECTS.DAT record array is truncated");
    }

    tables.object_conflict_flag.resize(count);
    for (std::size_t i = 0; i < count; ++i) {
        tables.object_conflict_flag[i] =
            objects[4U + i * 65U + 0x2FU];
    }

    return tables;
}

CharacterTransferImage build_character_transfer(
    const std::array<CharacterTransferActor, 2>& actors,
    const CharacterTransferTables& tables) {
    CharacterTransferImage out{};
    out.fill(0xFF);

    // function_4DC4 initializes four 0x248 records to FFh, then clears four
    // dwords at +10Bh for every record.
    for (std::size_t actor = 0; actor < 4U; ++actor) {
        const auto base = actor * kRecordSize;
        std::fill_n(out.begin() + static_cast<std::ptrdiff_t>(base + 0x10BU),
                    16,
                    0);
    }

    for (int actor_index = 0; actor_index < 2; ++actor_index) {
        const auto& actor =
            actors[static_cast<std::size_t>(actor_index)];
        validate_actor(actor, actor_index);

        const auto base =
            static_cast<std::size_t>(actor_index) * kRecordSize;
        const auto& d = actor.draft;
        const auto& s = actor.stats;

        // function_2DFC clears the entire 16-byte name field before input.
        std::fill_n(out.begin() + static_cast<std::ptrdiff_t>(base), 16, 0);
        std::copy(
            d.name.begin(),
            d.name.end(),
            out.begin() + static_cast<std::ptrdiff_t>(base));

        // function_3138 accepted actor status.
        put16(out, base + 0x10U, 0);

        out[base + 0x12U] = static_cast<std::uint8_t>(d.face);
        out[base + 0x13U] = static_cast<std::uint8_t>(d.sex);
        out[base + 0x14U] = static_cast<std::uint8_t>(d.race);
        for (std::size_t i = 0; i < 3U; ++i) {
            out[base + 0x15U + i] =
                d.classes[i] < 0
                    ? 0xFF
                    : static_cast<std::uint8_t>(d.classes[i]);
        }

        put16(
            out,
            base + 0x18U,
            static_cast<std::uint16_t>(s.base_6810));
        out[base + 0x1AU] = static_cast<std::uint8_t>(d.option);

        // function_4EF0 fixes 6813 to 10 and copies max HP to current HP.
        out[base + 0x1BU] = 10;
        put16(
            out,
            base + 0x1EU,
            static_cast<std::uint16_t>(
                static_cast<std::int16_t>(s.hit_points)));
        put16(out, base + 0x1CU, get16(out, base + 0x1EU));

        for (std::size_t i = 0; i < 3U; ++i) {
            put16(
                out,
                base + 0x20U + i * 2U,
                static_cast<std::uint16_t>(s.levels[i]));
        }

        // function_334C owns the seven primary bytes; 4EF0 duplicates each
        // primary into the adjacent runtime byte.
        for (std::size_t i = 0; i < 6U; ++i) {
            const auto at = base + 0x32U + i * 2U;
            const auto value =
                static_cast<std::uint8_t>(s.abilities[i]);
            out[at] = value;
            out[at + 1U] = value;
        }
        out[base + 0x3EU] =
            static_cast<std::uint8_t>(s.exceptional_strength);
        out[base + 0x3FU] = out[base + 0x3EU];

        // function_4EF0 clears +10B through the end of each of the first
        // two records before writing the class flags at +131.
        std::fill(
            out.begin() + static_cast<std::ptrdiff_t>(base + 0x10BU),
            out.begin() + static_cast<std::ptrdiff_t>(base + kRecordSize),
            0);

        std::uint8_t class_flags = 0;
        for (const int cls : d.classes) {
            if (cls == 3) class_flags |= 1U;
            if (cls == 4) class_flags |= 2U;
        }
        out[base + 0x131U] = class_flags;
    }

    // function_5F4C runs before 4EF0. Its writes are all before +10B and
    // therefore survive the final tail clear.
    std::array<std::uint8_t, 2> family{};
    family[0] = class_family(actors[0].draft, 0);
    family[1] = class_family(actors[1].draft, 1);

    for (std::size_t actor = 0; actor < 2U; ++actor) {
        const auto base = actor * kRecordSize;
        const auto race =
            static_cast<std::size_t>(actors[actor].draft.race);
        const auto fam = static_cast<std::size_t>(family[actor]);
        if (fam >= tables.family_race_words.size()) {
            throw std::logic_error("CODE.1 class-family index exceeds table");
        }

        const auto& words = tables.family_race_words[fam][race];

        // q4 = 098C,098E,098A,0988 => source words 2,3,1,0.
        constexpr std::array<std::size_t, 4> q4{{2, 3, 1, 0}};
        for (std::size_t j = 0; j < q4.size(); ++j) {
            put16(out, base + 0x5CU + j * 2U, words[q4[j]]);
        }
        for (std::size_t j = 0; j < 10U; ++j) {
            put16(out, base + 0x40U + j * 2U, words[4U + j]);
        }
    }

    // The second family probe inherits BL = family0, not actor index 1.
    const auto compare_family0 =
        class_family(actors[0].draft, 2);
    const auto compare_family1 =
        class_family(actors[1].draft, compare_family0);

    if (compare_family0 == compare_family1 &&
        actors[0].draft.race == actors[1].draft.race) {
        const auto base = kRecordSize;
        for (std::size_t i = 0; i < 10U; ++i) {
            const auto at = base + 0x40U + i * 2U;
            const auto object =
                static_cast<std::int16_t>(get16(out, at));

            // Shipped data uses valid nonnegative entries here. Preserve the
            // binary's lack of a negative prefilter by treating anything
            // outside the supplied OBJECTS.DAT corpus as non-conflicting.
            if (object >= 0 &&
                static_cast<std::size_t>(object) <
                    tables.object_conflict_flag.size() &&
                tables.object_conflict_flag[
                    static_cast<std::size_t>(object)] == 1U) {
                put16(out, at, 0xFFFFU);
            }
        }
    }

    return out;
}

}  // namespace rloft
