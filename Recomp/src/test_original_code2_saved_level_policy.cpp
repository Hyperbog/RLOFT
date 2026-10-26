// Source-selected CODE.2 181B4 validation must not contain a
// hard-coded Level22/23/26 transition allowlist. This is a policy
// test, not evidence that all 32 original levels are playable.
#include "original_code2_saved_level_policy.hpp"
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <stdexcept>

namespace p=rloft::original_saved_level_policy;
void require(bool condition,const char* reason) {
    if(!condition)throw std::runtime_error(reason);
}
int main() {
    try {
        unsigned authorized=0u,disallowed=0u;
        for(std::uint32_t from=0u;from<p::archived_level_count;++from) {
            for(std::uint32_t to=0u;to<p::archived_level_count;++to) {
                const bool expected=from!=to;
                require(p::source_selected_transition(from,to)==expected,
                    "original archive level selection was narrowed");
                if(expected)++authorized;else ++disallowed;
            }
        }
        require(authorized==32u*31u && disallowed==32u,
            "source transition matrix census invalid");
        for(const std::uint32_t invalid:{32u,33u,255u,0xFFFFFFFFu}) {
            require(!p::indexed_level(invalid),"invalid index allowed");
            for(std::uint32_t known=0u;known<32u;++known) {
                require(!p::source_selected_transition(invalid,known) &&
                        !p::source_selected_transition(known,invalid),
                        "out-of-archive source selection allowed");
            }
        }
        for(std::uint32_t to=0u;to<32u;++to) {
            require(p::authenticated_no_departing_level(
                        p::no_departing_level,to,true,1u,true,1u,1u),
                    "valid indexed saved slot rejected");
            require(!p::authenticated_no_departing_level(
                        22u,to,true,1u,true,1u,1u),
                    "sentinel-free saved restore allowed");
            require(!p::authenticated_no_departing_level(
                        p::no_departing_level,to,false,1u,true,1u,1u),
                    "unowned saved restore allowed");
            require(!p::authenticated_no_departing_level(
                        p::no_departing_level,to,true,0u,true,1u,1u),
                    "missing 07938 source callback allowed");
            require(!p::authenticated_no_departing_level(
                        p::no_departing_level,to,true,1u,false,1u,1u),
                    "missing saved level allowed");
            require(!p::authenticated_no_departing_level(
                        p::no_departing_level,to,true,1u,true,0u,1u),
                    "missing source saved reads allowed");
            require(!p::authenticated_no_departing_level(
                        p::no_departing_level,to,true,1u,true,1u,0u),
                    "missing original save open allowed");
            require(!p::authenticated_no_departing_level(
                        p::no_departing_level,to,true,1u,true,1u,2u),
                    "unexpected save opens allowed");
        }
        for(const std::uint32_t invalid:{32u,33u,0xFFFFFFFFu})
            require(!p::authenticated_no_departing_level(
                    p::no_departing_level,invalid,true,1u,true,1u,1u),
                "saved restore escaped original 32-entry level index");
        std::cout<<"ORIGINAL_CODE2_181B4_INDEXED_SOURCE_POLICY_PASS"
                 <<" indexed_levels=32 source_selections="<<authorized
                 <<" valid_restore_destinations=32 invalid_indices_rejected=YES"
                 <<" original_archive_record_validation=LIVE_SOURCE_ONLY"
                 <<" native_full_game=NOT_CERTIFIED\n";
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<e.what()<<"\n";return 1;
    }
}
