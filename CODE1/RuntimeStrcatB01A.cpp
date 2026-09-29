#include "RuntimeStrcatB01A.h"

namespace ravenloft::game {

std::string& RuntimeStrcatB01A::append(
    std::string& destination,
    const std::string& source) {
    destination += source;
    return destination;
}

} // namespace ravenloft::game
