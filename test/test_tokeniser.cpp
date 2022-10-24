

#include <string>

#include <gtest/gtest.h>

#include "parser.h"

void print_tokens(const std::vector<std::string> &tokens,
                  const std::string &label) {
    std::cout << label << "\n";
    for (auto &token : tokens) {
        std::cout << token << "\n";
    }
};

void print_tokens(const std::vector<std::string> &expected_tokens,
                  const std::vector<ncdlgen::Token> &tokens,
                  const std::string &label) {
    std::cout << label << "\n";
    for (size_t i = 0; i < tokens.size() || i < expected_tokens.size(); i++)
    {
        auto received = i < tokens.size() ? tokens[i] .content: "";
        auto expected = i < expected_tokens.size() ? expected_tokens[i] : "";
        std::cout << "expected: " << std::setw(15) <<  expected << "          " << "received: " << std::setw(15) << received << "\n";
    }
};

void check_tokeniser(const std::string &input,
                     const std::vector<std::string> &expected_tokens) {
    ncdlgen::Parser parser{input};
    auto tokens = parser.tokenize();

    print_tokens(expected_tokens,tokens, "expected");
    if (tokens.size() != expected_tokens.size()) {
        // print_tokens(, "parser");
    }


    ASSERT_EQ(tokens.size(), expected_tokens.size());

    for (size_t i = 0; i < tokens.size(); i++) {
        EXPECT_EQ(tokens[i].content, expected_tokens[i])
            << "Token " << i << ": " << tokens[i].content << " is not "
            << expected_tokens[i] << ".";
    }
}

TEST(tokeniser, root) {

    std::string input{"netcdf foo {}"};
    std::vector<std::string> expected_tokens{
        "netcdf",
        "foo",
        "{",
        "}",
    };

    check_tokeniser(input, expected_tokens);
}

TEST(tokeniser, types) {

    const char *cdl = ""
                      "netcdf foo {  // an example netCDF specification in CDL\n"
                      "     types:\n"
                      "         ubyte enum enum_t {Clear = 0, Stratus = 2};\n"
                      "         opaque(11) opaque_t;\n"
                      "         int(*) vlen_t;\n"
                      "}";

    std::string input{cdl};
    std::vector<std::string> expected_tokens{
        "netcdf",
        "foo",
        "{",
        "types:",
        "ubyte",
        "enum",
        "enum_t",
        "{",
        "Clear",
        "=",
        "0",
        ",",
        "Stratus",
        "=",
        "2",
        "}",
        ";",
        "opaque",
        "(",
        "11",
        ")",
        "opaque_t",
        ";",
        "int",
        "(",
        "*",
        ")",
        "vlen_t",
        ";",
        "}",

    };

    check_tokeniser(input, expected_tokens);
}
