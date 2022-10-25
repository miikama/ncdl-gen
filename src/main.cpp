
#include <iostream>

#include "tokeniser.h"
#include "parser.h"

int main() {
    std::cout << "Hello world!\n";

    std::string input{"netcdf foo {}"};

    ncdlgen::Tokeniser tokeniser{input};
    auto tokens = tokeniser.tokenise();

    std::cout << "tokens\n";
    for (auto &token : tokens) {
        std::cout << token.content() << "\n";
    }

    ncdlgen::Parser parser {tokens};
    auto ast = parser.parse();
    std::cout << "Parsed tree:\n";
    if(ast.has_value())
    {
        ast->print_tree();
    }

    return 0;
}
