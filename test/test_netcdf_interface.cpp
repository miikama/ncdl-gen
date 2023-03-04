
#include <gtest/gtest.h>

#include "foo_wrapper.h"
#include "netcdf_interface.h"

using namespace ncdlgen;

TEST(interface, netcdf)
{

    foo data{};

    // TODO: we just assume the location of the test directory
    NetCDFInterface interface{"../test/simple.nc"};

    interface.open();

    write(interface, data);

    auto read_data = read<foo>(interface);

    interface.close();
}
