
#pragma once

#include "stdint.h"
#include <vector>

#include "pipes/netcdf_pipe.h"
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

// Create member accessor that iterates over members to
// allow serialization one field at a time

void write(NetCDFPipe& pipe, const foo&);

template <typename StructureType> StructureType read(NetCDFPipe& pipe);

} // namespace ncdlgen
