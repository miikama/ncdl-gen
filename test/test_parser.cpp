

#include <string>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include "parser.h"
#include "tokeniser.h"

using namespace ncdlgen;

static void print_tokens(const std::vector<Token>& tokens)
{
    for (auto& token : tokens)
    {
        fmt::print("{}\n", token.content());
    }
}

static std::vector<Token> tokens_from_string(const std::string& input_string)
{
    Tokeniser tokeniser{input_string};
    return tokeniser.tokenise();
}

TEST(parser, empty_input)
{

    auto input_tokens = tokens_from_string("");

    Parser parser{input_tokens};
    auto result = parser.parse();
    ASSERT_FALSE(result.has_value());
}

TEST(parser, empty_root)
{

    std::string input{"netcdf foo { }"};
    auto input_tokens = tokens_from_string(input);

    Parser parser{input_tokens};
    auto result = parser.parse();
    ASSERT_TRUE(result.has_value());

    ASSERT_TRUE(result->group);
    ASSERT_EQ(result->group->name(), "foo");
}

TEST(parser, child_groups)
{
    std::string input{"netcdf foo { "
                      "   group: bar { "
                      " variables:  "
                      " int i;"
                      " }"
                      "}"};
    auto input_tokens = tokens_from_string(input);

    Parser parser{input_tokens};
    auto result = parser.parse();
    ASSERT_TRUE(result.has_value());

    ASSERT_TRUE(result->group);

    ASSERT_EQ(result->group->groups().size(), 1);
    EXPECT_EQ(result->group->groups().front().name(), "bar");
    ASSERT_EQ(result->group->groups().front().variables().size(), 1);
    EXPECT_EQ(result->group->groups().front().variables().front().name(), "i");
    EXPECT_EQ(result->group->groups().front().variables().front().basic_type(), NetCDFElementaryType::Int);
}
