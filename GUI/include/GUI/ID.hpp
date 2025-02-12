#pragma once

#include <cstdint>

using ID = uint64_t;

class IDGeneragor {
    ID number_ = 1;

  public:
    ID generate() { return number_++; }
};
