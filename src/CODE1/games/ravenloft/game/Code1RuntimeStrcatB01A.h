#pragma once

#include <string>

namespace ravenloft::game {

class Code1RuntimeStrcatB01A {
public:
    // Observable reconstruction of CODE.1 0xB01A..0xB054.
    static std::string& append(std::string& destination,
                               const std::string& source);
};

} // namespace ravenloft::game
