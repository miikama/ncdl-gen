

#include <fmt/core.h>

#include "generator.h"
#include "utils.h"

void generate(const std::string& input_cdl)
{

    ncdlgen::Generator generator{{}};

    auto contents = ncdlgen::read_file(input_cdl);

    generator.generate(contents);
}

int main(int argc, char** argv)
{

    if (argc > 1)
    {
        generate(argv[1]);
        return 0;
    }

    fmt::print("Usage:q generator [input_cdl]\n");
    return 0;
}