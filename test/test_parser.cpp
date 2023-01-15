

#include <string>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include "parser.h"
#include "tokeniser.h"

using namespace ncdlgen;

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

    auto input_tokens = tokens_from_string("netcdf root { }");

    Parser parser{input_tokens};
    auto result = parser.parse();
    ASSERT_TRUE(result.has_value());

    ASSERT_TRUE(result->group);
    ASSERT_EQ(result->group->name(), "root");
}
