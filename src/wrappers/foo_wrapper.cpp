
#include "foo_wrapper.h"

namespace ncdlgen
{

void write(NetCDFInterface& interface, const foo& data)
{
    interface.write<int, int>("/foo/bar", data.bar);
    interface.write<float, float>("/foo/baz", data.baz);
    interface.write<Container1D<uint16_t>, uint16_t>("/foo/bee", data.bee);
}

template <> foo read<foo>(NetCDFInterface& interface)
{
    foo foo;
    foo.bar = interface.read<int, int>("/foo/bar");
    foo.baz = interface.read<float, float>("/foo/baz");
    foo.bee = interface.read<Container1D<uint16_t>, uint16_t>("/foo/bee");
    return foo;
}

} // namespace ncdlgen
