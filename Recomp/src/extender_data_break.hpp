#pragma once
// Phar Lap 386|DOS-Extender CODE.1 CCDE: INT 21h AH=4Ah, ES=DS and
// BX=required size in 4096-byte units (NOT ordinary 16-byte DOS paragraphs).
// A fail-closed host guest-DGROUP backing store; not DOS/4GW memory parity.
#include <cstddef>
#include <cstdint>
#include <new>
#include <vector>

namespace rloft_recomp {
struct ExtenderBreakResult {
    bool carry;
    std::uint16_t ax; // 8=insufficient guest memory, 9=invalid selector
};
class ExtenderDataBreak {
public:
    static constexpr std::uint32_t kPage=4096u;
    static constexpr std::uint32_t kBudget=16u*1024u*1024u;
    explicit ExtenderDataBreak(std::uint16_t data_selector,
                               std::size_t required_static_bytes)
        : selector_(data_selector),static_bytes_(required_static_bytes) {}
    ExtenderBreakResult resize(std::uint16_t es,std::uint16_t pages,
                               std::vector<std::uint8_t>& guest) noexcept {
        if(es!=selector_)return {true,9u};
        const auto bytes=static_cast<std::uint32_t>(pages)*kPage;
        if(bytes<static_bytes_ || bytes>kBudget || pages==0u)
            return {true,8u};
        try {
            guest.resize(bytes,0u);
        }catch(const std::bad_alloc&){
            return {true,8u};
        }catch(const std::length_error&){
            return {true,8u};
        }
        ++resize_calls_;
        return {false,0u};
    }
    unsigned successful_resizes() const noexcept{return resize_calls_;}
private:
    std::uint16_t selector_;
    std::size_t static_bytes_;
    unsigned resize_calls_=0;
};
} // namespace rloft_recomp
