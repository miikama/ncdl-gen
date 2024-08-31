#pragma once

#include "netcdf_interface.h"
#include "stdint.h"

#include <vector>

#include "vector_interface.h"

namespace ncdlgen
{

struct simple
{
    struct foo
    {
        int bar;
        float baz;
        std::vector<uint16_t> bee;
    };

    foo foo_g{};
};

void read(NetCDFInterface& interface, simple&);

void read(NetCDFInterface& interface, simple::foo&);

void write(NetCDFInterface& interface, const simple&);

void write(NetCDFInterface& interface, const simple::foo&);

}; // namespace ncdlgen
