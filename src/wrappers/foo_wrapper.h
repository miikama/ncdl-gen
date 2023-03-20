
#pragma once

#include "stdint.h"
#include <vector>

#include "netcdf_interface.h"
#include "utils.h"
#include "vector_interface.h"

namespace ncdlgen
{

struct foo
{
    int bar;
    float baz;
    std::vector<uint16_t> bee;
};

void write(NetCDFInterface& interface, const foo&);

template <typename StructureType> StructureType read(NetCDFInterface& interface);

} // namespace ncdlgen
