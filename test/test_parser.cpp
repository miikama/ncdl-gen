

#include <string>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include "equality.h"
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

TEST(types, opaque)
{
    auto ref_type = OpaqueType("test_opaque", 1);
    auto wrong = OpaqueType("test_opaqu", 1);
    auto wrong2 = OpaqueType("test_opaque", 2);
    auto correct_type = OpaqueType("test_opaque", 1);

    ASSERT_EQ(ref_type, correct_type);
    ASSERT_NE(ref_type, wrong);
    ASSERT_NE(ref_type, wrong2);
}

TEST(types, vlen)
{
    auto ref_type = VLenType("test_vlen", NetCDFElementaryType::Int64);
    auto wrong = VLenType("test_vl", NetCDFElementaryType::Int64);
    auto wrong2 = VLenType("test_vlen", NetCDFElementaryType::Char);
    auto correct_type = VLenType("test_vlen", NetCDFElementaryType::Int64);

    ASSERT_EQ(ref_type, correct_type);
    ASSERT_NE(ref_type, wrong);
    ASSERT_NE(ref_type, wrong2);
}

TEST(types, complex)
{
    auto vlen_type = VLenType("vlen", NetCDFElementaryType::Int64);
    auto opaque_type = OpaqueType("opaque", 1);
    auto wrong_vlen = VLenType("vlen_wrong", NetCDFElementaryType::Char);
    auto wrong_opaque = VLenType("opaque_wrong", NetCDFElementaryType::Char);

    auto vlen_correct = ComplexType(vlen_type);
    auto opaque_correct = ComplexType(opaque_type);

    // comparing vlen_type to vlen_type
    ASSERT_EQ(ComplexType(vlen_type), ComplexType(vlen_type));
    ASSERT_EQ(NetCDFType(ComplexType(vlen_type)), NetCDFType(ComplexType(vlen_type)));

    // comparing vlen_type to wrong_vlen
    ASSERT_NE(ComplexType(vlen_type), ComplexType(wrong_vlen));
    ASSERT_NE(NetCDFType(ComplexType(vlen_type)), NetCDFType(ComplexType(wrong_vlen)));

    // comparing opauqe_type to opauqe_type
    ASSERT_EQ(ComplexType(opaque_type), ComplexType(opaque_type));
    ASSERT_EQ(NetCDFType(ComplexType(opaque_type)), NetCDFType(ComplexType(opaque_type)));

    // comparing opaque_type to wrong_opaque
    ASSERT_NE(ComplexType(opaque_type), ComplexType(wrong_opaque));
    ASSERT_NE(NetCDFType(ComplexType(opaque_type)), NetCDFType(ComplexType(wrong_opaque)));

    // Comparing opaque to vlen
    ASSERT_NE(ComplexType(opaque_type), ComplexType(vlen_type));
    ASSERT_NE(NetCDFType(ComplexType(opaque_type)), NetCDFType(ComplexType(vlen_type)));
    ASSERT_NE(ComplexType(vlen_type), ComplexType(opaque_type));
    ASSERT_NE(NetCDFType(ComplexType(vlen_type)), NetCDFType(ComplexType(opaque_type)));
}

// TODO: tests for enum, compound, array types missing
