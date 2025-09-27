
#include <fmt/core.h>
#include <gtest/gtest.h>

#include "generated_simple.h"
#include "pipes/zeromq_pipe.h"

using namespace ncdlgen;

TEST(pipe, zeromq_variable_info_to_string_empty)
{

    ZeroMQVariableInfo info{"/bar", {}};
    EXPECT_EQ("/bar;", info.to_string());
}
TEST(pipe, zeromq_variable_info_to_string_1d)
{

    ZeroMQVariableInfo info{"/bar", {1}};
    EXPECT_EQ("/bar;1", info.to_string());
}
TEST(pipe, zeromq_variable_info_to_string_2d)
{

    ZeroMQVariableInfo info{"/bar", {1, 4}};
    EXPECT_EQ("/bar;1,4", info.to_string());
}

TEST(pipe, zeromq_variable_info_from_string_empty)
{

    auto info = ZeroMQVariableInfo::from_string_view("/bar;");

    EXPECT_EQ("/bar", info.name);
    EXPECT_EQ(info.dimension_sizes.size(), 0);
}
TEST(pipe, zeromq_variable_info_from_string_1d)
{

    auto info = ZeroMQVariableInfo::from_string_view("/bar;2");

    EXPECT_EQ("/bar", info.name);
    EXPECT_EQ(info.dimension_sizes.size(), 1);
    EXPECT_EQ(info.dimension_sizes[0], 2);
}

TEST(pipe, zeromq_variable_info_from_string_2d)
{

    auto info = ZeroMQVariableInfo::from_string_view("/bar;3,5");

    EXPECT_EQ("/bar", info.name);
    EXPECT_EQ(info.dimension_sizes.size(), 2);
    EXPECT_EQ(info.dimension_sizes[0], 3);
    EXPECT_EQ(info.dimension_sizes[1], 5);
}

TEST(pipe, zeromq_scalar)
{

    ZeroMQPipe pipe{};

    int data{4};
    pipe.write<int, int, VectorInterface>("/foo/bar", data);

    auto read_data = pipe.read<int, int, VectorInterface>("/foo/bar");

    EXPECT_EQ(read_data, data);
}

TEST(pipe, zeromq_many_scalar)
{

    ZeroMQPipe pipe{};

    {
        int data{4};
        pipe.write<int, int, VectorInterface>("/foo/bar", data);
    }

    {
        double data{2.0};
        pipe.write<double, double, VectorInterface>("/baz", data);
    }

    {
        auto read_data = pipe.read<int, int, VectorInterface>("/foo/bar");
        EXPECT_EQ(read_data, 4);
    }

    {
        auto read_data = pipe.read<double, double, VectorInterface>("/baz");
        EXPECT_DOUBLE_EQ(read_data, 2.0);
    }
}

TEST(pipe, zeromq_read_incorrect_type)
{

    ZeroMQPipe pipe{};

    {
        int data{4};
        pipe.write<int, int, VectorInterface>("/foo/bar", data);
    }

    {
        auto helper = [&] { pipe.read<uint16_t, uint16_t, VectorInterface>("/foo/bar"); };
        EXPECT_ANY_THROW(helper());
    }
}

TEST(pipe, zeromq_read_incorrect_path)
{

    ZeroMQPipe pipe{};

    {
        int data{4};
        pipe.write<int, int, VectorInterface>("/foo/bar", data);
    }

    {
        auto helper = [&] { pipe.read<int, int, VectorInterface>("/foo"); };
        EXPECT_ANY_THROW(helper());
    }
}

TEST(pipe, zeromq_vector)
{

    ZeroMQPipe pipe{};

    std::vector<double> data{1, 2, 3, 4, 5, 6};
    pipe.write<std::vector<double>, double, VectorInterface>("/foo/bar", data);

    auto read_data = pipe.read<std::vector<double>, double, VectorInterface>("/foo/bar");

    EXPECT_EQ(read_data.size(), 6);
    EXPECT_EQ(read_data[0], 1);
    EXPECT_EQ(read_data[1], 2);
    EXPECT_EQ(read_data[2], 3);
    EXPECT_EQ(read_data[3], 4);
    EXPECT_EQ(read_data[4], 5);
    EXPECT_EQ(read_data[5], 6);
}

TEST(pipe, zeromq_vector_2d)
{

    ZeroMQPipe pipe{};

    std::vector<std::vector<int>> data{{1, 2, 3}, {4, 5, 6}};
    pipe.write<std::vector<std::vector<int>>, int, VectorInterface>("/foo/bar", data);

    auto read_data = pipe.read<std::vector<std::vector<int>>, int, VectorInterface>("/foo/bar");

    EXPECT_EQ(read_data.size(), 2);
    EXPECT_EQ(read_data[0].size(), 3);
    EXPECT_EQ(read_data[1].size(), 3);
    EXPECT_EQ(read_data[0][0], 1);
    EXPECT_EQ(read_data[0][1], 2);
    EXPECT_EQ(read_data[0][2], 3);
    EXPECT_EQ(read_data[1][0], 4);
    EXPECT_EQ(read_data[1][1], 5);
    EXPECT_EQ(read_data[1][2], 6);
}

TEST(pipe, zeromq_vector_incorrect_dimensions)
{

    ZeroMQPipe pipe{};

    std::vector<double> data{1, 1, 1, 2, 2, 2};
    pipe.write<std::vector<double>, double, VectorInterface>("/foo/bar", data);

    // Read back 2d vector instead of written 1d vector
    auto helper = [&]
    { return pipe.read<std::vector<std::vector<double>>, double, VectorInterface>("/foo/bar"); };
    EXPECT_ANY_THROW(helper());
}

TEST(pipe, zeromq_generated)
{

    // ZeroMQPipe pipe{};

    // ncdlgen::simple data{};
    // ncdlgen::write(ZeroMQPipe, data);
}
