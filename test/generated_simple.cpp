#include "generated_simple.h"
using namespace ncdlgen;

void ncdlgen::write(NetCDFInterface& interface, const simple& data){
  interface.write<int,int,VectorInterface>("/bar", data.bar);
  interface.write<float,float,VectorInterface>("/baz", data.baz);
  interface.write<VectorInterface::container_type_t<ushort>,ushort,VectorInterface>("/bee", data.bee);
}
template <> simple ncdlgen::read<simple>(NetCDFInterface& interface){
  simple simple;
  simple.bar = interface.read<int,int,VectorInterface>("/bar");
  simple.baz = interface.read<float,float,VectorInterface>("/baz");
  simple.bee = interface.read<VectorInterface::container_type_t<ushort>,ushort,VectorInterface>("/bee");
  return simple;
}
