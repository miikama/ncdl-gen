

#include <fmt/core.h>

#include "generator.h"
#include "utils.h"

using namespace ncdlgen;

void generate(const std::string& input_cdl, Generator::GenerateTarget target)
{
    Generator::Options options{.target = target};
    Generator generator{options};

    auto contents = read_file(input_cdl);

    generator.generate(contents);
}

int main(int argc, char** argv)
{

    if (argc > 2)
    {
        if (std::string_view(argv[2]) == "--header")
        {
            generate(argv[1], Generator::GenerateTarget::Header);
        }
        else if (std::string_view(argv[2]) == "--source")
        {
            generate(argv[1], Generator::GenerateTarget::Source);
        }
        else
        {
            fmt::print("Usage: generator [input_cdl] [OPTION..]\n\n  --header Create header\n  --source "
                       "Create source\n");
        }
        return 0;
    }

    fmt::print(
        "Usage: generator [input_cdl] [OPTION..]\n\n  --header Create header\n  --source  Create source\n");
    return 0;
}