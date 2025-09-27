
#include <gtest/gtest.h>

#include "vector_interface.h"

using namespace ncdlgen;

TEST(vector_interface, number_of_elements)
{

    std::vector<std::size_t> a{1, 2, 3, 4, 5};
    std::vector<std::size_t> b{};

    EXPECT_EQ(VectorOperations::number_of_elements(a), 1 * 2 * 3 * 4 * 5);
    EXPECT_EQ(VectorOperations::number_of_elements(b), 0);
}
TEST(vector_interface, assign_scalar)
{

    int a{};
    std::vector<int> data = {1};
    VectorOperations::assign(a, data);
    EXPECT_EQ(a, 1);

    // TODO: Having extra data is allowed
    data.push_back(2);
    VectorOperations::assign(a, data);
    EXPECT_EQ(a, 1);
}

TEST(vector_interface, assign_1d)
{

    std::vector<int> a = {
        0,
        0,
        0,
        0,
    };

    std::vector<int> data = {1, 2, 3, 4};
    VectorOperations::assign(a, data);
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a[1], 2);
    EXPECT_EQ(a[2], 3);
    EXPECT_EQ(a[3], 4);

    // throw with incorrect target dimensions
    a.push_back(2);
    EXPECT_ANY_THROW(VectorOperations::assign(a, data));
}

TEST(vector_interface, assign_2d)
{

    std::vector<std::vector<int>> a;
    VectorOperations::resize(a, {2, 3});

    size_t flat_index{};
    std::vector<int> data = {1, 2, 3, 4, 5, 6};
    VectorOperations::assign(a, data, flat_index);
    EXPECT_EQ(flat_index, 6);
    EXPECT_EQ(a[0][0], 1);
    EXPECT_EQ(a[0][1], 2);
    EXPECT_EQ(a[0][2], 3);
    EXPECT_EQ(a[1][0], 4);
    EXPECT_EQ(a[1][1], 5);
    EXPECT_EQ(a[1][2], 6);
}

TEST(vector_interface, resize_scalar)
{
    int a{1};
    std::vector<std::size_t> target_size{};
    VectorOperations::resize(a, target_size);
    EXPECT_EQ(a, 1);

    // throw with incorrect target dimensions
    EXPECT_ANY_THROW(VectorOperations::resize(a, {1}));
    EXPECT_ANY_THROW(VectorOperations::resize(a, {1, 2}));
}
TEST(vector_interface, resize_1d)
{
    std::vector<int> a{};
    std::vector<std::size_t> target_size{2};
    VectorOperations::resize(a, target_size);

    ASSERT_EQ(a.size(), 2);
    EXPECT_EQ(a[0], 0);
    EXPECT_EQ(a[1], 0);

    // throw with incorrect target dimensions
    EXPECT_ANY_THROW(VectorOperations::resize(a, {}));
    EXPECT_ANY_THROW(VectorOperations::resize(a, {1, 2}));
}
TEST(vector_interface, resize_2d)
{
    std::vector<std::vector<int>> a{};
    std::vector<std::size_t> target_size{2, 3};

    VectorOperations::resize(a, target_size);
    ASSERT_EQ(a.size(), 2);
    ASSERT_EQ(a[0].size(), 3);
    ASSERT_EQ(a[1].size(), 3);

    EXPECT_EQ(a[0][1], 0);
    EXPECT_EQ(a[0][2], 0);
    EXPECT_EQ(a[0][3], 0);
    EXPECT_EQ(a[1][0], 0);
    EXPECT_EQ(a[1][1], 0);
    EXPECT_EQ(a[1][3], 0);

    // throw with incorrect target dimensions
    EXPECT_ANY_THROW(VectorOperations::resize(a, {}));
    EXPECT_ANY_THROW(VectorOperations::resize(a, {1}));
}

TEST(vector_interface, dimension_sizes)
{
    {

        std::vector<std::vector<int>> data{};
        auto dimension_sizes =
            VectorOperations::container_dimension_sizes<int, std::vector<std::vector<int>>>(data);
        EXPECT_EQ(dimension_sizes.size(), 2);
        EXPECT_EQ(dimension_sizes[0], 0);
        EXPECT_EQ(dimension_sizes[1], 0);
    }

    {
        std::vector<std::vector<int>> data{{1, 2, 3}, {2, 3, 4}};
        auto dimension_sizes =
            VectorOperations::container_dimension_sizes<int, std::vector<std::vector<int>>>(data);
        EXPECT_EQ(dimension_sizes.size(), 2);
        EXPECT_EQ(dimension_sizes[0], 2);
        EXPECT_EQ(dimension_sizes[1], 3);
    }
}
