
#include <gtest/gtest.h>

#include "netcdf.h"
#include "netcdf_interface.h"

using namespace ncdlgen;

TEST(interface, netcdf)
{

    foo data{};

    NetCDFInterface interface{};

    write(interface, data);

    auto read_data = read<foo>(interface);
}
