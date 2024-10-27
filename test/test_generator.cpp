
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
    ncdlgen::simple::foo data{
        .bar = 5, .baz = 32, .bee = {1, 2, 3, 4, 5}, .foobar = {{1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}}};
    ncdlgen::simple root{.foo_g = data};

    std::string cdl = {"netcdf simple {\n"
                       "  group: foo{\n"
                       "  dimensions:\n"
                       "      dim = 5;\n"
                       "  variables:\n"
                       "      int bar;\n"
                       "      float baz;\n"
                       "      ushort bee(dim);\n"
                       "      int foobar(dim, dim);}}"};
    make_nc_from_cdl(cdl, "generated.nc");

    ncdlgen::NetCDFInterface interface {
        "generated.nc"
    };
    interface.open();
    ncdlgen::write(interface, root);

    ncdlgen::simple read_root;
    read(interface, read_root);
    interface.close();

    EXPECT_EQ(read_root.foo_g.bar, 5);
    EXPECT_EQ(read_root.foo_g.baz, 32);
    ASSERT_EQ(read_root.foo_g.bee.size(), 5);
    EXPECT_EQ(read_root.foo_g.bee[0], 1);
    EXPECT_EQ(read_root.foo_g.bee[1], 2);
    EXPECT_EQ(read_root.foo_g.bee[2], 3);
    EXPECT_EQ(read_root.foo_g.bee[3], 4);
    EXPECT_EQ(read_root.foo_g.bee[4], 5);
}
