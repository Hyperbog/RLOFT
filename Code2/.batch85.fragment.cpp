// Batch 85: 32F4C..32F8A initialize the current 1Ch-byte sweep record.
void function_32f4c(Machine& m) noexcept {
    const u32 rec=0x15A18u+load_le32(dg(m,0xC6FCu))*0x1Cu;
    store_le32(dg(m,rec+0x00u),0u);
    store_le32(dg(m,rec+0x04u),0x93u);
    store_le32(dg(m,rec+0x08u),0x93u);
    store_le32(dg(m,rec+0x0Cu),0u);
    store_le32(dg(m,rec+0x10u),0u);
    store_le32(dg(m,rec+0x14u),0u);
    store_le32(dg(m,rec+0x18u),0u);
}
static_assert(0x32F8Bu-0x32F4Cu==63u);
// 32F8B is an unreferenced NOP alignment byte.
