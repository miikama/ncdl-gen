

#include <string>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include "equality.h"
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

TEST(parser, types)
{
    std::string input{"netcdf foo {\n"
                      "   types:\n"
                      "      int enum enum_t { entry1 = 1, entry2 = 3};\n"
                      "      uint(*) vlen_t;\n"
                      "      compound combined { enum_t t1; vlen_t t2; };\n"
                      "}"};
    auto input_tokens = tokens_from_string(input);

    Parser parser{input_tokens};
    auto result = parser.parse();
    ASSERT_TRUE(result.has_value());

    ASSERT_TRUE(result->group);
    auto types = result->group->types();
    ASSERT_EQ(types.size(), 3);
    EXPECT_EQ(types[0].name(), "enum_t");
    EXPECT_EQ(types[1].name(), "vlen_t");
    EXPECT_EQ(types[2].name(), "combined");
}

TEST(parser, global_attributes)
{
    std::string input{"netcdf foo {\n"
                      "   variables:\n"
                      "      :untyped_global_attribute = \"global\";\n"
                      "      int :globalatt = 1;\n"
                      "}"};
    auto input_tokens = tokens_from_string(input);

    Parser parser{input_tokens};
    auto result = parser.parse();
    ASSERT_TRUE(result.has_value());

    ASSERT_TRUE(result->group);
    auto variables = result->group->variables();
    ASSERT_EQ(variables.size(), 0);

    auto attributes = result->group->attributes();
    ASSERT_EQ(attributes.size(), 2);
    EXPECT_EQ(attributes[0].name(), "untyped_global_attribute");
    // Note: We give untyped attributes type of 'string'
    EXPECT_TRUE(attributes[0].type().has_value());
    EXPECT_EQ(attributes[0].type().value(), NetCDFElementaryType::String);
    EXPECT_EQ(attributes[0].string_data(), std::string("\"global\""));

    EXPECT_EQ(attributes[1].name(), "globalatt");
    EXPECT_TRUE(attributes[1].type().has_value());
    EXPECT_EQ(attributes[1].type().value(), NetCDFType(NetCDFElementaryType::Int));
    EXPECT_EQ(attributes[1].string_data(), std::string(""));
}

TEST(parser, variables)
{
    std::string input{"netcdf foo {\n"
                      "  dimensions:\n"
                      "    dim = 5;\n"
                      "  variables:\n"
                      "    int bar; \n"
                      "    float baz; \n"
                      "    ushort bee(dim); \n"
                      "}"};
    auto input_tokens = tokens_from_string(input);

    Parser parser{input_tokens};
    auto result = parser.parse();
    ASSERT_TRUE(result.has_value());

    ASSERT_TRUE(result->group);
    auto variables = result->group->variables();
    ASSERT_EQ(variables.size(), 3);
    EXPECT_EQ(variables[0].basic_type(), NetCDFElementaryType::Int);
    EXPECT_EQ(variables[0].name(), "bar");
    EXPECT_EQ(variables[0].dimensions().size(), 0);
    EXPECT_EQ(variables[1].basic_type(), NetCDFElementaryType::Float);
    EXPECT_EQ(variables[1].name(), "baz");
    EXPECT_EQ(variables[1].dimensions().size(), 0);
    EXPECT_EQ(variables[2].basic_type(), NetCDFElementaryType::Ushort);
    EXPECT_EQ(variables[2].name(), "bee");
    ASSERT_EQ(variables[2].dimensions().size(), 1);
    // TODO: the variable dimensions are not actually mapped to actual dimensions
    EXPECT_EQ(variables[2].dimensions()[0].name(), "dim");

    auto dimensions = result->group->dimensions();
    ASSERT_EQ(dimensions.size(), 1);
    EXPECT_EQ(dimensions[0].length, 5);
    EXPECT_EQ(dimensions[0].name, "dim");
}
