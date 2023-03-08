
#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include "foo_wrapper.h"
#include "netcdf_interface.h"

using namespace ncdlgen;

static void make_nc_from_cdl(const std::string& cdl, const std::string& netcdf_filename)
{
    std::string command = fmt::format("echo \"{}\" | ncgen -4 -o {}", cdl, netcdf_filename);
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
}

TEST(interface, netcdf)
{

    foo data{.bar = 5, .baz = 32, .bee = {1, 2, 3, 4, 5}};

    std::string cdl = {"netcdf simple {\n"
                       "group: foo{\n"
                       "dimensions:\n"
                       "    dim = 5;\n"
                       "variables:\n"
                       "    int bar;\n"
                       "    float baz;\n"
                       "    ushort bee(dim);}}"};
    make_nc_from_cdl(cdl, "simple.nc");

    NetCDFInterface interface{"simple.nc"};

    interface.open();

    write(interface, data);

    auto read_data = read<foo>(interface);

    interface.close();

    EXPECT_EQ(data.bar, read_data.bar);
    EXPECT_EQ(data.baz, read_data.baz);
    EXPECT_EQ(data.bee, read_data.bee);
}
