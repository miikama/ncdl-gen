#include "generated_simple.h"

void ncdlgen::write(NetCDFPipe& pipe, const ncdlgen::simple& data) { ncdlgen::write(pipe, data.foo_g); }

void ncdlgen::write(ZeroMQPipe& pipe, const ncdlgen::simple& data) { ncdlgen::write(pipe, data.foo_g); }

void ncdlgen::write(NetCDFPipe& pipe, const ncdlgen::simple::foo& data)
{
    pipe.write<int, int, VectorInterface>("/foo/bar", data.bar);
    pipe.write<float, float, VectorInterface>("/foo/baz", data.baz);
    pipe.write<std::vector<uint16_t>, uint16_t, VectorInterface>("/foo/bee", data.bee);
    pipe.write<std::vector<std::vector<int>>, int, VectorInterface>("/foo/foobar", data.foobar);
}

void ncdlgen::write(ZeroMQPipe& pipe, const ncdlgen::simple::foo& data)
{
    pipe.write<int, int, VectorInterface>("/foo/bar", data.bar);
    pipe.write<float, float, VectorInterface>("/foo/baz", data.baz);
    pipe.write<std::vector<uint16_t>, uint16_t, VectorInterface>("/foo/bee", data.bee);
    pipe.write<std::vector<std::vector<int>>, int, VectorInterface>("/foo/foobar", data.foobar);
}

void ncdlgen::read(NetCDFPipe& pipe, ncdlgen::simple& simple) { ncdlgen::read(pipe, simple.foo_g); }

void ncdlgen::read(ZeroMQPipe& pipe, ncdlgen::simple& simple) { ncdlgen::read(pipe, simple.foo_g); }

void ncdlgen::read(NetCDFPipe& pipe, ncdlgen::simple::foo& foo)
{
    foo.bar = pipe.read<int, int, VectorInterface>("/foo/bar");
    foo.baz = pipe.read<float, float, VectorInterface>("/foo/baz");
    foo.bee = pipe.read<std::vector<uint16_t>, uint16_t, VectorInterface>("/foo/bee");
    foo.foobar = pipe.read<std::vector<std::vector<int>>, int, VectorInterface>("/foo/foobar");
}

void ncdlgen::read(ZeroMQPipe& pipe, ncdlgen::simple::foo& foo)
{
    foo.bar = pipe.read<int, int, VectorInterface>("/foo/bar");
    foo.baz = pipe.read<float, float, VectorInterface>("/foo/baz");
    foo.bee = pipe.read<std::vector<uint16_t>, uint16_t, VectorInterface>("/foo/bee");
    foo.foobar = pipe.read<std::vector<std::vector<int>>, int, VectorInterface>("/foo/foobar");
}
