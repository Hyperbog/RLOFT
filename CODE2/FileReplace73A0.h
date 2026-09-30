#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <vector>
namespace ravenloft::game {
struct FileReplace73A0Ops {
 std::function<int(const char*)> stat339A2;
 std::function<int(const char*)> size3127C;
 std::function<int(const char*)> openRead312A0;
 std::function<int(const char*)> create31380;
 std::function<int(const char*)> openWrite312B4;
 std::function<int(void*,int,int)> read312EC;
 std::function<int(const void*,int,int)> write31304;
 std::function<void(int)> close312DC;
 std::function<void(const char*)> remove312C8;
 std::function<int(void*)> statBuffer339CF;
};
class FileReplace73A0 { public: static int run(const FileReplace73A0Ops&,const char* sourceBase,const char* destBase,const char* sourceSuffix,const char* destSuffix); };
}
