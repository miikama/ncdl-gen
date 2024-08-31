#include "generated_simple.h"

void ncdlgen::write(NetCDFInterface& interface, const ncdlgen::simple& data)
{
    ncdlgen::write(interface, data.foo_g);
}

void ncdlgen::write(NetCDFInterface& interface, const ncdlgen::simple::foo& data)
{
    interface.write<int, int, VectorInterface>("/foo/bar", data.bar);
    interface.write<float, float, VectorInterface>("/foo/baz", data.baz);
    interface.write<std::vector<uint16_t>, uint16_t, VectorInterface>("/foo/bee", data.bee);
}

void ncdlgen::read(NetCDFInterface& interface, ncdlgen::simple& simple)
{
    ncdlgen::read(interface, simple.foo_g);
}

void ncdlgen::read(NetCDFInterface& interface, ncdlgen::simple::foo& foo)
{
    foo.bar = interface.read<int, int, VectorInterface>("/foo/bar");
    foo.baz = interface.read<float, float, VectorInterface>("/foo/baz");
    foo.bee = interface.read<std::vector<uint16_t>, uint16_t, VectorInterface>("/foo/bee");
}
