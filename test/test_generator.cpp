
#include <gtest/gtest.h>

#include "generated_simple.h"

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

TEST(generator, basic)
{
    // The name of the root group is the name
    // ncdlgen::simple::foo data{.bar = 5, .baz = 32, .bee = {1, 2, 3, 4, 5}};
    // ncdlgen::simple root{.foo_g = data};
    ncdlgen::simple root{.bar = 5, .baz = 32, .bee = {1, 2, 3, 4, 5}};

    std::string cdl = {"netcdf simple {\n"
                       "dimensions:\n"
                       "    dim = 5;\n"
                       "variables:\n"
                       "    int bar;\n"
                       "    float baz;\n"
                       "    ushort bee(dim);}"};
    make_nc_from_cdl(cdl, "generated.nc");

    ncdlgen::NetCDFInterface interface{"generated.nc"};
    interface.open();
    ncdlgen::write(interface, root);

    auto read_root = read<ncdlgen::simple>(interface);
    interface.close();

    static_assert(std::is_same_v<std::decay_t<decltype(read_root)>, ncdlgen::simple>,
                  "The type name of the root structure should match the read type");

    EXPECT_EQ(read_root.bar, 5);
    EXPECT_EQ(read_root.baz, 32);
    ASSERT_EQ(read_root.bee.size(), 5);
    EXPECT_EQ(read_root.bee[0], 1);
    EXPECT_EQ(read_root.bee[1], 2);
    EXPECT_EQ(read_root.bee[2], 3);
    EXPECT_EQ(read_root.bee[3], 4);
    EXPECT_EQ(read_root.bee[4], 5);
}
