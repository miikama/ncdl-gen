#include "generated_simple.h"

void ncdlgen::write(ncdlgen::NetCDFPipe& pipe, const ncdlgen::simple& data)
{
    ncdlgen::write(pipe, data.foo_g);
}

void ncdlgen::write(ncdlgen::ZeroMQPipe& pipe, const ncdlgen::simple& data)
{
    ncdlgen::write(pipe, data.foo_g);
}

void ncdlgen::write(ncdlgen::NetCDFPipe& pipe, const ncdlgen::simple::foo& data)
{
    pipe.write<int, int, ncdlgen::VectorInterface>("/foo/bar", data.bar);
    pipe.write<float, float, ncdlgen::VectorInterface>("/foo/baz", data.baz);
    pipe.write<std::vector<uint16_t>, uint16_t, ncdlgen::VectorInterface>("/foo/bee", data.bee);
    pipe.write<std::vector<std::vector<int>>, int, ncdlgen::VectorInterface>("/foo/foobar", data.foobar);
}

void ncdlgen::write(ncdlgen::ZeroMQPipe& pipe, const ncdlgen::simple::foo& data)
{
    pipe.write<int, int, ncdlgen::VectorInterface>("/foo/bar", data.bar);
    pipe.write<float, float, ncdlgen::VectorInterface>("/foo/baz", data.baz);
    pipe.write<std::vector<uint16_t>, uint16_t, ncdlgen::VectorInterface>("/foo/bee", data.bee);
    pipe.write<std::vector<std::vector<int>>, int, ncdlgen::VectorInterface>("/foo/foobar", data.foobar);
}

void ncdlgen::read(ncdlgen::NetCDFPipe& pipe, ncdlgen::simple& simple) { ncdlgen::read(pipe, simple.foo_g); }

void ncdlgen::read(ncdlgen::ZeroMQPipe& pipe, ncdlgen::simple& simple) { ncdlgen::read(pipe, simple.foo_g); }

void ncdlgen::read(ncdlgen::NetCDFPipe& pipe, ncdlgen::simple::foo& foo)
{
    foo.bar = pipe.read<int, int, ncdlgen::VectorInterface>("/foo/bar");
    foo.baz = pipe.read<float, float, ncdlgen::VectorInterface>("/foo/baz");
    foo.bee = pipe.read<std::vector<uint16_t>, uint16_t, ncdlgen::VectorInterface>("/foo/bee");
    foo.foobar = pipe.read<std::vector<std::vector<int>>, int, ncdlgen::VectorInterface>("/foo/foobar");
}

void ncdlgen::read(ncdlgen::ZeroMQPipe& pipe, ncdlgen::simple::foo& foo)
{
    foo.bar = pipe.read<int, int, ncdlgen::VectorInterface>("/foo/bar");
    foo.baz = pipe.read<float, float, ncdlgen::VectorInterface>("/foo/baz");
    foo.bee = pipe.read<std::vector<uint16_t>, uint16_t, ncdlgen::VectorInterface>("/foo/bee");
    foo.foobar = pipe.read<std::vector<std::vector<int>>, int, ncdlgen::VectorInterface>("/foo/foobar");
}
