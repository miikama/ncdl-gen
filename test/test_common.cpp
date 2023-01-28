

#include <string>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include "utils.h"

using namespace ncdlgen;

TEST(common, split_string)
{
    {

        std::string path = {"root"};

        auto split = split_string(path, '/');

        ASSERT_EQ(split.size(), 1);
        ASSERT_EQ(split[0], "root");
    }

    {

        std::string path = {"/root"};

        auto split = split_string(path, '/');

        ASSERT_EQ(split.size(), 1);
        ASSERT_EQ(split[0], "root");
    }
}
