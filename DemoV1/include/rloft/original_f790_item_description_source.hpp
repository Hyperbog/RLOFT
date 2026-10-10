#pragma once

#include "rloft/original_f790_item_category_route.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

namespace rloft {

// CODE.2 F7E4..FD57 source-only item-description construction.
// DS C strings (610/614/618/61C/620/624/6D98), decrypted TEXT indices,
// F5F8 nested weight, and subtype tables 7332/67EC are caller-provided
// ORIGINAL resources. Never substitute guessed English punctuation/text.
struct OriginalF790ItemDescriptionSource final {
    using Record = std::array<std::uint8_t,0x41>;

    template<class DataText,class ResourceText,class NestedHalfWeight,
             class Slot7332,class Slot67ec>
    [[nodiscard]] static std::string compose(
        const Record& record, std::int32_t item_id, bool known,
        DataText&& data_text, ResourceText&& resource_text,
        NestedHalfWeight&& nested_half_weight, Slot7332&& slot_7332,
        Slot67ec&& slot_67ec) {
        // The original name is the NUL-terminated C-string at +0.
        // Reject invalid 40-byte input rather than reading record metadata.
        std::size_t len=0;
        while(len<40U && record[len]!=0U)++len;
        if(len==40U)
            throw std::out_of_range("F790 original item name lacks terminator");
        std::string result(reinterpret_cast<const char*>(record.data()),len);
        const auto ds=[&](std::uint32_t address) {result+=data_text(address);};
        const auto t=[&](std::uint32_t index) {result+=resource_text(index);};
        const auto num=[&](std::int32_t value) {result+=std::to_string(value);};
        const auto signed_byte=[](std::uint8_t raw)->int {
            const int value=static_cast<int>(raw);
            return value<128 ? value : value-256;
        };

        // F812..F882: identification only reveals a nonzero enchantment.
        if(known && record[0x37]!=0U) {
            ds(0x610U);
            num(static_cast<std::int32_t>(record[0x37]));
        }
        ds(0x614U); // Always present before CS item-type dispatch.

        switch(OriginalF790ItemCategoryRoute::destination(record[0x2f])) {
        case OriginalF790ItemCategoryRoute::kType2To4:
            // FA85..FB3C: T[40], DS 618, NEG(SX byte +34), DS 614.
            t(0x40U);ds(0x618U);num(-signed_byte(record[0x34]));ds(0x614U);
            break;
        case OriginalF790ItemCategoryRoute::kType9To11: {
            // F8D4..FA83: first range +38..3A; second +3B..3D.
            const int a=record[0x38], b=record[0x39], c=record[0x3a];
            const int d=record[0x3b], e=record[0x3c], f=record[0x3d];
            t(0x3eU);ds(0x618U);num(a+c);ds(0x61cU);num(a*b+c);
            ds(0x620U);num(d+f);ds(0x61cU);num(d*e+f);
            break;
        }
        case OriginalF790ItemCategoryRoute::kType5To8AndDefault: {
            // FB3D..FC05: T[3D], WORD-truncated total half-unit weight.
            t(0x3dU);
            const int subtype=signed_byte(record[0x31]);
            std::uint32_t halves=record[0x30];
            if(record[0x2f]==1U)
                halves+=static_cast<std::uint32_t>(nested_half_weight(subtype));
            const auto word=static_cast<std::uint16_t>(halves);
            num(static_cast<std::int32_t>(word/2U));
            if((word & 1U)!=0U)ds(0x624U);
            if(record[0x2f]==1U) {
                // FC15..FCC9: +31 is signed; source uses subtype*10
                // at 7332. 7F suppresses the optional subtype suffix.
                const auto code=slot_7332(subtype);
                if(code!=0x7fU) {
                    ds(0x614U);t(0x41U);ds(0x618U);
                    num(static_cast<std::int32_t>(code));
                }
            } else if((record[0x31]!=0U || item_id==0x4a) && known) {
                // FCCB..FD57: extra known-only text and 67EC table.
                ds(0x6d98U);
                num(static_cast<std::int32_t>(slot_67ec(subtype)));
            }
            break;
        }
        default:
            throw std::logic_error("unknown original F790 category route");
        }
        return result;
    }
};
} // namespace rloft
