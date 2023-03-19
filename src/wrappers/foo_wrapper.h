
#pragma once

#include "stdint.h"
#include <vector>

#include "forward.h"
#include "utils.h"

namespace ncdlgen
{

template <typename T> using Container1D = std::vector<T>;

struct foo
{
    int bar;
    float baz;
    Container1D<uint16_t> bee;
};

void write(NetCDFInterface& interface, const foo&);

template <typename StructureType> StructureType read(NetCDFInterface& interface);

} // namespace ncdlgen
