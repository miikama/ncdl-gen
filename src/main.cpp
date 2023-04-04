
#include <iostream>

#include "parser.h"
#include "tokeniser.h"
#include "utils.h"

void parse_file(const std::string& filename)
{

    auto input = ncdlgen::read_file(filename);

    ncdlgen::Tokeniser tokeniser{input};
    auto tokens = tokeniser.tokenise();

    ncdlgen::Parser parser{tokens};
    auto ast = parser.parse();
    std::cout << "Parsed tree:\n";
    if (ast.has_value())
    {
        ast->print_tree();
    }
}

int main(int argc, char** argv)
{

    if (argc > 1)
    {
        parse_file(argv[1]);
        return 0;
    }

    std::string input{"netcdf foo {}"};
    ncdlgen::Tokeniser tokeniser{input};
    auto tokens = tokeniser.tokenise();

    std::cout << "tokens\n";
    for (auto& token : tokens)
    {
        std::cout << token.content() << "\n";
    }

    ncdlgen::Parser parser{tokens};
    auto ast = parser.parse();
    std::cout << "Parsed tree:\n";
    if (ast.has_value())
    {
        ast->print_tree();
    }

    return 0;
}
