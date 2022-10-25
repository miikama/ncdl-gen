
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
        std::cout << token.content << "\n";
    }

    ncdlgen::Parser parser {};
    auto ast = parser.parse(tokens);
    if(ast.has_value())
    {
        ast->print();
    }

    return 0;
}
