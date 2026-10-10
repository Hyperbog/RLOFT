#pragma once

#include "rloft/original_f790_item_description_source.hpp"
#include "rloft/original_f790_static_resources.hpp"
#include "rloft/original_t_text_archive.hpp"

#include <cstdint>
#include <string>
#include <utility>

namespace rloft {

// Source-only join of original item record, CODE.2 data-object literals,
// and XOR-decoded original T records. Runtime F5F8 recursive weight must
// come from the MUTABLE inventory rather than original initial slot lists.
struct OriginalF790ResolvedDescription final {
    template<class RuntimeNestedHalfWeight>
    [[nodiscard]] static std::string compose(
        const OriginalF790ItemDescriptionSource::Record& object_record,
        std::int32_t item_id,bool known,
        const OriginalF790StaticResources& code2_static,
        const OriginalTTextArchive& original_t,
        RuntimeNestedHalfWeight&& current_nested_weight) {
        return OriginalF790ItemDescriptionSource::compose(
            object_record,item_id,known,
            [&](std::uint32_t ds){return code2_static.ds_text(ds);},
            [&](std::uint32_t t){return original_t.decoded(t);},
            std::forward<RuntimeNestedHalfWeight>(current_nested_weight),
            [&](int subtype){return code2_static.suffix_7332(subtype);},
            [&](int subtype){return code2_static.charge_67ec(subtype);});
    }
};
} // namespace rloft
