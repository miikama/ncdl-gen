
#include "foo_wrapper.h"

namespace ncdlgen
{

void write(NetCDFInterface& interface, const foo& data)
{
    interface.write<int>("/foo/bar", data.bar);
    interface.write<float>("/foo/baz", data.baz);
    interface.write<Container1D<uint16_t>>("/foo/bee", data.bee);
}

template <> foo read<foo>(NetCDFInterface& interface)
{
    foo foo;
    foo.bar = interface.read<int>("/foo/bar");
    foo.baz = interface.read<float>("/foo/baz");
    foo.bee = interface.read<Container1D<uint16_t>>("/foo/bee");
    return foo;
}

} // namespace ncdlgen
