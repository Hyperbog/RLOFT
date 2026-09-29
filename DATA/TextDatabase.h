#pragma once
#include <filesystem>
#include <string>
#include <vector>
namespace ravenloft::data {
class TextDatabase { public: explicit TextDatabase(const std::filesystem::path& path); std::size_t count() const{return entries_.size();} const std::string& at(std::size_t i)const{return entries_.at(i);} private: std::vector<std::string> entries_; };
}
