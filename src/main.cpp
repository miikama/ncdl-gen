
#include <iostream>

#include "parser.h"

int main() {
    std::cout << "Hello world!\n";

    std::string input{"netcdf foo {}"};

    ncdlgen::Parser parser{input};
    auto tokens = parser.tokenize();

    std::cout << "tokens\n";
    for (auto &token : tokens) {
        std::cout << token.content << "\n";
    }

    return 0;
}
