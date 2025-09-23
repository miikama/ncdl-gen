
#include <fmt/core.h>
#include <gtest/gtest.h>

#include "generated_simple.h"
#include "pipes/zeromq_pipe.h"

using namespace ncdlgen;

TEST(pipe, zeromq_elementary_scalar)
{

    ZeroMQPipe pipe{};

    int data{4};
    pipe.write<int, int, VectorInterface>("/foo/bar", data);

    auto read_data = pipe.read<int, int, VectorInterface>("/foo/bar");

    EXPECT_EQ(read_data, data);
}

TEST(pipe, zeromq_elementary_many_scalar)
{

    ZeroMQPipe pipe{};

    {
        int data{4};
        pipe.write<int, int, VectorInterface>("/foo/bar", data);
    }

    {
        double data{2.0};
        pipe.write<double, double, VectorInterface>("/foo/bar", data);
    }

    {
        auto read_data = pipe.read<int, int, VectorInterface>("/foo/bar");
        EXPECT_EQ(read_data, 4);
    }

    {
        auto read_data = pipe.read<double, double, VectorInterface>("/foo/bar");
        EXPECT_DOUBLE_EQ(read_data, 2.0);
    }
}

TEST(pipe, zeromq_elementary_read_incorrect_type)
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

TEST(pipe, zeromq_elementary_read_incorrect_path)
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

TEST(pipe, zeromq_elementary_vector)
{

    ZeroMQPipe pipe{};

    // std::vector<double> data{1, 1, 1, 2, 2, 2};
    // pipe.write<std::vector<double>, double, VectorInterface>("/foo/bar", data);

    // auto read_data = pipe.read<std::vector<std::vector<double>>, double, VectorInterface>("/foo/bar");
}

TEST(pipe, zeromq_generated)
{

    // ZeroMQPipe pipe{};

    // ncdlgen::simple data{};
    // ncdlgen::write(ZeroMQPipe, data);
}
