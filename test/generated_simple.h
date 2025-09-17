#pragma once

#include "pipes/netcdf_pipe.h"
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
        std::vector<std::vector<int>> foobar;
    };

    foo foo_g{};
};

void read(NetCDFPipe& pipe, simple&);

void read(NetCDFPipe& pipe, simple::foo&);

void write(NetCDFPipe& pipe, const simple&);

void write(NetCDFPipe& pipe, const simple::foo&);

}; // namespace ncdlgen
