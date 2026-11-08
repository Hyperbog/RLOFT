#pragma once

#include "rloft/T.hpp"

namespace rloft {
// Existing CODE.2 and F790 source-only callers keep their established name.
// The canonical reader/serializer is TArchive (T.hpp / T.cpp).
using OriginalTTextArchive = TArchive;
} // namespace rloft
