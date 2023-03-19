
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

template <typename ElementType, std::size_t DimensionCount> struct vector_ND
{
    vector_ND(const std::array<std::size_t, DimensionCount>& dimension_sizes)
    {
        std::size_t number_of_elements = dimension_sizes.size() > 0 ? 1 : 0;
        for (auto& size : dimension_sizes)
        {
            number_of_elements *= size;
        }
        m_data.resize(number_of_elements, {});
    }
    void resize(const std::array<std::size_t, DimensionCount>& size) {}
    std::size_t size() const { return m_data.size(); }

    ElementType* data() { return this->m_data.data(); }
    const ElementType* data() const { return this->m_data.data(); }

    std::vector<ElementType> m_data{};
};

template <typename T> using Container1D = vector_ND<T, 1>;
template <typename T> using Container2D = vector_ND<T, 2>;

// setup support for our new ND container
namespace ncdlgen
{
template <typename ElementType>
constexpr std::true_type is_supported_ndarray<ElementType, vector_ND<ElementType, 1>>;

} // namespace ncdlgen

TEST(interface, write_ND)
{
    // Write data in VectorND format to the file
    NetCDFInterface interface{"simple.nc"};
    interface.open();

    vector_ND<uint16_t, 1> data{{5}};
    *data.data() = 1;
    *(data.data() + 1) = 2;
    *(data.data() + 2) = 3;
    *(data.data() + 3) = 66;
    *(data.data() + 4) = 5;

    interface.write<vector_ND<uint16_t, 1>, uint16_t>("/foo/bee", data);

    // Read data using already existing interface to make sure
    // writing went ok.
    auto foo = read<ncdlgen::foo>(interface);
    interface.close();

    ASSERT_EQ(foo.bee.size(), 5);
    EXPECT_EQ(foo.bee[0], 1);
    EXPECT_EQ(foo.bee[1], 2);
    EXPECT_EQ(foo.bee[2], 3);
    EXPECT_EQ(foo.bee[3], 66);
    EXPECT_EQ(foo.bee[4], 5);
}
