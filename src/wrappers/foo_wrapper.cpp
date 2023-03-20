
#include "foo_wrapper.h"
#include "vector_interface.h"

namespace ncdlgen
{

void write(NetCDFInterface& interface, const foo& data)
{
    interface.write<int, int, VectorInterface>("/foo/bar", data.bar);
    interface.write<float, float, VectorInterface>("/foo/baz", data.baz);
    interface.write<std::vector<uint16_t>, uint16_t, VectorInterface>("/foo/bee", data.bee);
}

template <> foo read<foo>(NetCDFInterface& interface)
{
    foo foo;
    foo.bar = interface.read<int, int, VectorInterface>("/foo/bar");
    foo.baz = interface.read<float, float, VectorInterface>("/foo/baz");
    foo.bee = interface.read<std::vector<uint16_t>, uint16_t, VectorInterface>("/foo/bee");
    return foo;
}

} // namespace ncdlgen
