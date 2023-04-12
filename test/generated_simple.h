#pragma once

#include "stdint.h"

#include <vector>

#include "netcdf_interface.h"
#include "utils.h"
#include "vector_interface.h"

namespace ncdlgen {

struct simple
{
  struct foo
  {
      int bar;
      float baz;
      std::vector<ushort> bee;
  };

  foo foo_g{};
};

void read(NetCDFInterface& interface, simple&);

void read(NetCDFInterface& interface, simple::foo&);

void write(NetCDFInterface& interface, const simple&);

void write(NetCDFInterface& interface, const simple::foo&);

};
