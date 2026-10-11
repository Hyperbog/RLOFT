// Canonical-source compilation unit. The exact Code1.cpp is compiled as C++;
// do not reimplement its behavior here or substitute the original executable.
#include "../../Code1/Code1.cpp"

#include <vector>

extern "C" bool rloft_recomp_code1_memory_probe() {
    // Original CODE.1 0x7F50 reads DGROUP[0x2934] (32-bit tick value).
    // Allocate the complete CODE.1 data-object virtual size, zero initialized.
    std::vector<code1::u8> image(code1::kDataObjectVirtualSize, 0);
    code1::Machine state{};
    state.data=image.data();
    constexpr code1::u32 expected=0x8A7031D2u;
    code1::store_le32(image.data()+0x2934u, expected);
    return code1::function_7f50(state)==expected;
}
