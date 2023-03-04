
#include <gtest/gtest.h>

#include "foo_wrapper.h"
#include "netcdf_interface.h"

using namespace ncdlgen;

TEST(interface, netcdf)
{

    foo data{.bar = 5, .baz = 32};

    // TODO: we just assume the location of the test directory
    auto ret = system("cp ../test/simple.nc .");
    ASSERT_EQ(ret, 0);

    NetCDFInterface interface{"simple.nc"};

    interface.open();

    write(interface, data);

    auto read_data = read<foo>(interface);

    interface.close();

    EXPECT_EQ(data.bar, read_data.bar);
    EXPECT_EQ(data.baz, read_data.baz);
    EXPECT_EQ(data.bee, read_data.bee);
}
