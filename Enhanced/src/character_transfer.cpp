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

bool CharacterStartingLoadout::has_shipped_items() const noexcept {
    return std::any_of(slots.begin(),slots.end(),
                       [](std::int16_t item) {return item!=-1;});
}

CharacterStartingLoadout CharacterTransferTables::starting_loadout(
    std::size_t family,std::size_t race) const {
    if(family>=family_race_words.size() ||
       race>=family_race_words[0].size()) {
        throw std::out_of_range("original CODE.1 starter family/race");
    }
    CharacterStartingLoadout out{};
    out.family=family;
    out.race=race;
    out.slots.fill(-1);
    const auto& words=family_race_words[family][race];
    // CODE.1 function_5F4C takes four equipped words at
    // 098C,098E,098A,0988, then ten ordinary item words
    // at 0990..09A2. Slot 14/15 = hands; 16 = robe/armor;
    // 17 = headwear. Actor slots 10..13 and 18..20 are FF.
    constexpr std::array<std::size_t,4> equipped{{2,3,1,0}};
    for(std::size_t slot=0;slot<10U;++slot) {
        out.slots[slot]=static_cast<std::int16_t>(words[4U+slot]);
    }
    for(std::size_t i=0;i<equipped.size();++i) {
        out.slots[14U+i]=static_cast<std::int16_t>(
            words[equipped[i]]);
    }
    return out;
}

CharacterTransferTables CharacterTransferTables::from_original_bytes(
    const std::vector<std::uint8_t>& code1,
    const std::vector<std::uint8_t>& objects) {
    constexpr std::size_t kCode1Dgroup=0x3E724U;
    constexpr std::size_t kOriginalStarterAt=0x0988U;
    constexpr std::size_t kFamilyStride=0xA8U;
    constexpr std::size_t kRaceStride=0x1CU;
    constexpr std::size_t kFamilyCount=15U;
    constexpr std::size_t kRaceCount=6U;
    constexpr std::size_t kWords=14U;
    constexpr std::size_t kEnd=kCode1Dgroup+kOriginalStarterAt+
        kFamilyStride*kFamilyCount;
    if(code1.size()<kEnd) {
        throw std::runtime_error(
            "CODE.1 truncated before all original 90 starting loadouts");
    }
    if(objects.size()<4U) {
        throw std::runtime_error("OBJECTS.DAT is truncated");
    }
    const auto count=static_cast<std::size_t>(le32(objects.data()));
    if(count>(objects.size()-4U)/65U ||
       4U+count*65U!=objects.size()) {
        throw std::runtime_error(
            "OBJECTS.DAT size disagrees with original counted records");
    }

    CharacterTransferTables tables{};
    static_assert(kFamilyCount==15U && kRaceCount==6U);
    for(std::size_t family=0;family<kFamilyCount;++family) {
        for(std::size_t race=0;race<kRaceCount;++race) {
            const auto offset=kCode1Dgroup+kOriginalStarterAt+
                family*kFamilyStride+race*kRaceStride;
            for(std::size_t word=0;word<kWords;++word) {
                tables.family_race_words[family][race][word]=
                    static_cast<std::uint16_t>(code1[offset+2U*word]) |
                    (static_cast<std::uint16_t>(
                        code1[offset+2U*word+1U])<<8U);
            }
        }
    }
    tables.object_conflict_flag.resize(count);
    for(std::size_t i=0;i<count;++i) {
        tables.object_conflict_flag[i]=
            objects[4U+i*65U+0x2FU];
    }
    // All nonempty entries must reference actual shipped OBJECTS.DAT
    // IDs. A corrupted row must never become random starter equipment.
    for(std::size_t family=0;family<kFamilyCount;++family) {
        for(std::size_t race=0;race<kRaceCount;++race) {
            for(const auto id:tables.family_race_words[family][race]) {
                if(id!=0xFFFFU && id>=count) {
                    throw std::runtime_error(
                        "CODE.1 starting item exceeds OBJECTS.DAT count");
                }
            }
        }
    }
    return tables;
}

CharacterTransferTables CharacterTransferTables::from_assets(
    const RavenloftBootAssets& assets) {
    return from_original_bytes(
        assets.source().read("CODE.1"),
        assets.source().read("OBJECTS.DAT"));
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

        const auto loadout=tables.starting_loadout(fam,race);
        // CODE.1 5F4C authentic family/race loadout, preserving all
        // fourteen original words and the 7 untouched empty slots.
        // Both actor records receive exactly the same mapping;
        // the source's separate second-actor conflict pass follows.
        for(std::size_t j=0U;j<loadout.slots.size();++j) {
            if(j>=10U && j<=13U) continue;
            if(j>=18U) continue;
            put16(out,base+0x40U+j*2U,
                  static_cast<std::uint16_t>(loadout.slots[j]));
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
