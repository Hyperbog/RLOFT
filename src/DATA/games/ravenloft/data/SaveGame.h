#pragma once
#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace ravenloft::data {

inline constexpr std::size_t kContainerDescriptorCount = 62;
inline constexpr std::size_t kSaveDynamicRecordSize = 0x16;
inline constexpr std::size_t kSaveAuxCharacterBlockSize = 0x16D0;
const std::array<std::uint8_t,kContainerDescriptorCount>& containerDescriptorCapacities();

struct CharacterRecord {
    std::array<std::uint8_t, 0x248> raw{};
    std::string name;
    std::uint16_t status = 0;
    std::uint8_t portrait = 0xFF;
    std::uint8_t sex = 0xFF;
    std::uint8_t race = 0xFF;
    std::array<std::uint8_t,3> classes{{0xFF,0xFF,0xFF}};
    std::uint8_t alignment = 0xFF;
    std::uint16_t currentHitPoints = 0;
    std::uint16_t maximumHitPoints = 0;
    std::array<std::uint8_t,3> levels{{0xFF,0xFF,0xFF}};
    std::array<std::uint32_t,3> xp{{0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu}};
    std::array<std::uint8_t,6> abilities{{0,0,0,0,0,0}};
    std::uint8_t exceptionalStrength = 0;
    std::array<std::uint16_t,14> carriedItems{};

    bool active() const;
};

using SaveDynamicRecord = std::array<std::uint8_t,kSaveDynamicRecordSize>;

struct QuickStartSave {
    // CODE.2 0x67D4 writes this as a fixed 50-byte field. Keep the raw bytes
    // alongside the convenience string so a decode->encode round trip does
    // not normalize padding or invent text encoding behavior.
    std::array<std::uint8_t,50> descriptionRaw{};
    std::string description;
    std::array<std::uint8_t,32> questFlags{};
    std::uint32_t level = 0;
    std::int32_t partyXFixed = 0;
    std::int32_t partyYFixed = 0;
    std::int32_t facingDegrees = 0;

    // Fixed writer fields are address-named until their gameplay semantics are
    // independently proven. The writer serializes them literally in this order.
    std::uint16_t savedF03C = 0;
    std::uint16_t savedF03E = 0;
    std::uint32_t savedFD08 = 0;
    std::uint16_t saved6DAE = 0;

    std::uint32_t timerF58C = 0;           // CODE.2 0x30700 source, SAVE.DAT +0x6C
    std::uint32_t worldClock = 0;          // CODE.2 DS:CF4C, SAVE.DAT +0x70
    std::int16_t heldObjectId = -1;        // CODE.2 DS:8600, SAVE.DAT +0x74
    std::uint8_t heldObjectState = 0;      // CODE.2 DS:8602, SAVE.DAT +0x76

    std::array<CharacterRecord,4> party{};

    std::uint32_t dynamicEntityCount = 0;  // CODE.2 DS:C038
    std::vector<SaveDynamicRecord> dynamicEntities{}; // DS:248EC, 0x16 bytes each
    std::array<std::uint8_t,8> savedD8A8{};
    std::uint32_t gameplayClockA7E4 = 0;   // CODE.2 0x19CC4 source

    std::array<std::vector<std::int16_t>,kContainerDescriptorCount> containerItems{};
    std::array<std::uint8_t,kContainerDescriptorCount> containerState{};
    std::int32_t savedBaseDarkness = 0;    // CODE.2 DS:F920
    std::int32_t ambientLightModifier = 0; // CODE.2 DS:F91C

    // The remaining writer blocks are preserved by source address rather than
    // assigned speculative names. Their exact byte lengths/call ordering are
    // proven by CODE.2 0x6BD2..0x6C55.
    std::array<std::uint8_t,15> saved67EC{};
    std::array<std::uint8_t,kSaveAuxCharacterBlockSize> savedA808{};
    std::array<std::uint8_t,16> saved6FA0{};

    double partyX() const { return partyXFixed / 65536.0; }
    double partyY() const { return partyYFixed / 65536.0; }
};

class SaveGame {
public:
    explicit SaveGame(const std::filesystem::path& path);
    const QuickStartSave& quickStart() const { return quickStart_; }

    // CODE.2 0x67D4..0x6C55 SAVE.DAT serialization order. This emits the
    // original variable-size format (base 9288 bytes + 0x16 per dynamic record).
    static std::vector<std::uint8_t> encode(const QuickStartSave& save);
    static void write(const std::filesystem::path& path,const QuickStartSave& save);
private:
    QuickStartSave quickStart_{};
};

const char* raceName(std::uint8_t race);
const char* className(std::uint8_t cls);
const char* alignmentName(std::uint8_t alignment);

}
