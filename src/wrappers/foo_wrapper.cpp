
#include "foo_wrapper.h"
#include "vector_interface.h"

namespace ncdlgen
{

void write(NetCDFPipe& pipe, const foo& data)
{
    pipe.write<int, int, VectorInterface>("/foo/bar", data.bar);
    pipe.write<float, float, VectorInterface>("/foo/baz", data.baz);
    pipe.write<std::vector<uint16_t>, uint16_t, VectorInterface>("/foo/bee", data.bee);
}

template <> foo read<foo>(NetCDFPipe& pipe)
{
    foo foo;
    foo.bar = pipe.read<int, int, VectorInterface>("/foo/bar");
    foo.baz = pipe.read<float, float, VectorInterface>("/foo/baz");
    foo.bee = pipe.read<std::vector<uint16_t>, uint16_t, VectorInterface>("/foo/bee");
    return foo;
}

} // namespace ncdlgen
