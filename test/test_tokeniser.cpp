

#include <string>

#include <gtest/gtest.h>

#include "tokeniser.h"

void print_tokens(const std::vector<std::string>& expected_tokens, const std::vector<ncdlgen::Token>& tokens,
                  const std::string& label)
{
    std::cout << label << "\n";
    for (size_t i = 0; i < tokens.size() || i < expected_tokens.size(); i++)
    {
        auto received = i < tokens.size() ? tokens[i].content() : "";
        auto expected = i < expected_tokens.size() ? expected_tokens[i] : "";
        std::cout << "expected: " << std::setw(15) << expected << "          "
                  << "received: " << std::setw(15) << received << "\n";
    }
};

void check_tokeniser(const std::string& input, const std::vector<std::string>& expected_tokens)
{
    ncdlgen::Tokeniser parser{input};
    auto tokens = parser.tokenise();

    if (tokens.size() != expected_tokens.size())
    {
        // print_tokens(expected_tokens,tokens, "expected");
    }

    ASSERT_EQ(tokens.size(), expected_tokens.size());

    for (size_t i = 0; i < tokens.size(); i++)
    {
        EXPECT_EQ(tokens[i].content(), expected_tokens[i])
            << "Token " << i << ": " << tokens[i].content() << " is not " << expected_tokens[i] << ".";
    }
}

TEST(tokeniser, root)
{

    std::string input{"netcdf foo {}"};
    std::vector<std::string> expected_tokens{
        "netcdf",
        "foo",
        "{",
        "}",
    };

    check_tokeniser(input, expected_tokens);
}

TEST(tokeniser, types)
{

    const char* cdl = ""
                      "netcdf foo {  // an example netCDF specification in CDL\n"
                      "     types:\n"
                      "         ubyte enum enum_t {Clear = 0, Stratus = 2};\n"
                      "         opaque(11) opaque_t;\n"
                      "         int(*) vlen_t;\n"
                      "}";

    std::string input{cdl};
    std::vector<std::string> expected_tokens{
        "netcdf", "foo",      "{",       "types:", "ubyte", "enum", "enum_t", "{",      "Clear", "=",
        "0",      ",",        "Stratus", "=",      "2",     "}",    ";",      "opaque", "(",     "11",
        ")",      "opaque_t", ";",       "int",    "(",     "*",    ")",      "vlen_t", ";",     "}",

    };

    check_tokeniser(input, expected_tokens);
}

TEST(tokeniser, dimensions)
{

    const char* cdl = ""
                      "netcdf foo {  // an example netCDF specification in CDL\n"
                      "dimensions:\n"
                      "   lat = 10, lon = 5, time = unlimited ;\n"
                      "   dim2 = 2 ;\n"
                      "   dim3 = UNLIMITED ;"
                      "}";

    std::string input{cdl};
    std::vector<std::string> expected_tokens{
        "netcdf", "foo", "{",    "dimensions:", "lat",       "=",         "10", ",",    "lon",
        "=",      "5",   ",",    "time",        "=",         "unlimited", ";",  "dim2", "=",
        "2",      ";",   "dim3", "=",           "UNLIMITED", ";",         "}",

    };

    check_tokeniser(input, expected_tokens);
}

TEST(tokeniser, variables)
{

    const char* cdl = ""
                      "netcdf foo {  // an example netCDF specification in CDL\n"
                      "variables:\n"
                      "long    lat(lat), lon(lon), time(time);\n"
                      "float   Z(time,lat,lon),t(time,lat,lon);\n"
                      "double  p(time,lat,lon);\n"
                      "ubyte   tag;\n"
                      "}";

    std::string input{cdl};
    std::vector<std::string> expected_tokens{
        "netcdf", "foo",  "{",   "variables:", "long", "lat",   "(",      "lat", ")", ",",     "lon",
        "(",      "lon",  ")",   ",",          "time", "(",     "time",   ")",   ";", "float", "Z",
        "(",      "time", ",",   "lat",        ",",    "lon",   ")",      ",",   "t", "(",     "time",
        ",",      "lat",  ",",   "lon",        ")",    ";",     "double", "p",   "(", "time",  ",",
        "lat",    ",",    "lon", ")",          ";",    "ubyte", "tag",    ";",   "}",

    };

    check_tokeniser(input, expected_tokens);
}

TEST(tokeniser, attributes)
{

    const char* cdl = ""
                      "netcdf foo {  // an example netCDF specification in CDL\n"
                      "variables:\n"
                      "// variable attributes\n"
                      "lat:long_name = \"latitude\";\n"
                      "lat:units = \"degrees_north\";\n"
                      "time:units = \"seconds since 1992-1-1 00:00:00\";\n"
                      "// typed variable attributes\n"
                      "string Z:units = \"geopotential meters\";\n"
                      "float Z:valid_range = 0., 5000.;\n"
                      "double p:_FillValue = -9999.;\n"
                      "long rh:_FillValue = -1;\n"
                      "vlen_t :globalatt = {17, 18, 19};\n"
                      "}";

    std::string input{cdl};
    std::vector<std::string> expected_tokens{
        "netcdf",
        "foo",
        "{",
        "variables:",
        "lat:long_name",
        "=",
        "\"latitude\"",
        ";",
        "lat:units",
        "=",
        "\"degrees_north\"",
        ";",
        "time:units",
        "=",
        "\"seconds since 1992-1-1 00:00:00\"",
        ";",
        "string",
        "Z:units",
        "=",
        "\"geopotential meters\"",
        ";",
        "float",
        "Z:valid_range",
        "=",
        "0.",
        ",",
        "5000.",
        ";",
        "double",
        "p:_FillValue",
        "=",
        "-9999.",
        ";",
        "long",
        "rh:_FillValue",
        "=",
        "-1",
        ";",
        "vlen_t",
        ":globalatt",
        "=",
        "{",
        "17",
        ",",
        "18",
        ",",
        "19",
        "}",
        ";",
        "}",

    };

    check_tokeniser(input, expected_tokens);
}

TEST(tokeniser, data)
{

    const char* cdl = ""
                      "netcdf foo {  // an example netCDF specification in CDL\n"
                      "data:\n"
                      "    lat   = 0, 10, 60, 90;\n"
                      "    lon   = -140, -118;\n"
                      "}";

    std::string input{cdl};
    std::vector<std::string> expected_tokens{
        "netcdf", "foo", "{", "data:", "lat", "=",    "0", ",",    "10", ",", "60",
        ",",      "90",  ";", "lon",   "=",   "-140", ",", "-118", ";",  "}",

    };

    check_tokeniser(input, expected_tokens);
}

TEST(tokeniser, groups)
{

    const char* cdl = ""
                      "netcdf foo {  // an example netCDF specification in CDL\n"
                      "group: g {\n"
                      "types:\n"
                      "    compound cmpd_t { vlen_t f1; enum_t f2;};\n"
                      "} // group g\n"
                      "group: h {\n"
                      "variables:\n"
                      "    /g/cmpd_t  compoundvar;\n"
                      " data:\n"
                      "    compoundvar = { {3,4,5}, enum_t.Stratus } ;\n"
                      "} // group h\n"
                      "}";

    std::string input{cdl};
    std::vector<std::string> expected_tokens{
        "netcdf",      "foo",        "{",
        "group:",      "g",          "{",
        "types:",      "compound",   "cmpd_t",
        "{",           "vlen_t",     "f1",
        ";",           "enum_t",     "f2",
        ";",           "}",          ";",
        "}",           "group:",     "h",
        "{",           "variables:", "/g/cmpd_t",
        "compoundvar", ";",          "data:",
        "compoundvar", "=",          "{",
        "{",           "3",          ",",
        "4",           ",",          "5",
        "}",           ",",          "enum_t.Stratus",
        "}",           ";",          "}",
        "}",

    };

    check_tokeniser(input, expected_tokens);
}

TEST(tokeniser, source_locations)
{

    const char* cdl = "\n"
                      "netcdf foo {  // an example netCDF specification in CDL\n"
                      "data:\n"
                      "    lat   = 0, 10, 60, 90;\n"
                      "    lon   = -140, -118;\n"
                      "}";

    std::string input{cdl};

    ncdlgen::Tokeniser parser{input};
    auto tokens = parser.tokenise();

    ASSERT_GT(tokens.size(), 10);
    EXPECT_EQ(tokens[0].content(), "netcdf");
    // line numbers are 0-based, first word on 2nd line
    EXPECT_EQ(tokens[0].source_location.line, 1);
    // column numbers are zero based and at the end of the token
    EXPECT_EQ(tokens[0].source_location.column, 6);

    EXPECT_EQ(tokens[4].content(), "lat");
    // lat is on 4th line
    EXPECT_EQ(tokens[4].source_location.line, 3);
    // column numbers also include the whitespace
    EXPECT_EQ(tokens[4].source_location.column, 7);
}
