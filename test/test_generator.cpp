
#include <gtest/gtest.h>

#include "generated_simple.h"

TEST(generator, basic)
{

    // The name of the root group is the name
    ncdlgen::simple root_group{};

    auto& foo_group = root_group.foo_g;

    ASSERT_TRUE(true);
}
