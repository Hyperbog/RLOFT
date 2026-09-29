#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>
namespace ravenloft::game {
struct ResourceRecord8978 {
    std::array<std::uint8_t,9> raw{};
    std::uint8_t& flags0() noexcept{return raw[0];}
    std::uint8_t& loaded2() noexcept{return raw[2];}
    std::uint16_t word3() const noexcept;
    void setWord3(std::uint16_t) noexcept;
    std::uint32_t dword5() const noexcept;
    void setDword5(std::uint32_t) noexcept;
};
struct ResourceGroup8978 {
    std::int32_t handle=0; std::uint32_t dword4=0,count=0;
    std::uintptr_t recordAllocation=0;
    std::vector<ResourceRecord8978> records;
};
struct ResourceIndexState8978 {
    std::array<std::uint8_t,13> infoDigits2AA4{};
    std::array<char,80> basePath2AB1{};
    std::int32_t groupCount2B04=0;
    std::uintptr_t groupTableAllocation2B08=0;
    std::vector<ResourceGroup8978> groups;
};
struct ResourceIndexOps8978 {
    std::function<std::int32_t(const std::string&)> openRead62A8;
    std::function<std::int32_t(void*,std::int32_t,std::int32_t)> read6304;
    std::function<std::int32_t(std::int32_t)> close62F0;
    std::function<void(std::int32_t,std::int32_t)> seekIndexed6358;
    std::function<std::uintptr_t(std::size_t)> heapAllocate71C8;
    std::function<void(std::uintptr_t)> heapFree7240;
    std::function<void(std::uint32_t)> fatalId9814;
    std::function<void(const std::string&)> fatalText9814;
    std::function<std::vector<ResourceRecord8978>(std::size_t)> allocateRecordStorage;
    std::function<void(std::int32_t,std::int32_t)> call9488;
    std::function<void(std::int32_t,std::int32_t)> call91D8;
};
struct ResourceHeader8D0C {
    std::uint16_t signature=0; std::uint8_t byte2=0; std::int16_t payloadLength=0;
    std::uintptr_t payloadAllocation=0; std::vector<std::uint8_t> payload;
};
class ResourceIndex8978 {
public:
    static void loadInfo(ResourceIndexState8978&,const ResourceIndexOps8978&);
    static void openGroups(const std::string&,std::int32_t,ResourceIndexState8978&,const ResourceIndexOps8978&);
    static void releaseAllItems(ResourceIndexState8978&,const ResourceIndexOps8978&);
    static void setLoadedWord3(std::uint32_t,ResourceIndexState8978&) noexcept;
    static void shutdown(ResourceIndexState8978&,const ResourceIndexOps8978&);
    static std::uint32_t sumDword4(const ResourceIndexState8978&) noexcept;
    static void readHeader(ResourceHeader8D0C&,std::int32_t,std::int32_t,std::uint32_t,const ResourceIndexOps8978&);
    static std::int32_t groupHandle(std::int32_t,const ResourceIndexState8978&) noexcept;
    static std::uint32_t groupItemCount(std::int32_t,const ResourceIndexState8978&) noexcept;
    // 0x8E20..0x8E3B jump table: 8F44,8F87,8FC3,8FFF,903B,9077,90B1.
};
} // namespace ravenloft::game
