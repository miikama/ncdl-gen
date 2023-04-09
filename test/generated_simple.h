#pragma once

#include "stdint.h"

#include <vector>

#include "netcdf_interface.h"
#include "utils.h"
#include "vector_interface.h"

namespace ncdlgen {

struct simple
{
  int bar;
  float baz;
  std::vector<ushort> bee;
};

template <typename StructureType> StructureType read(NetCDFInterface& interface);

void write(NetCDFInterface& interface, const simple&);

};
