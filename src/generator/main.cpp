
#include "CLI/CLI.hpp"
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
    CLI::App app{"Interface generator"};

    std::string interface_cdl;
    bool create_header{false};
    bool create_source{false};
    // Create the code for writing to these pipes
    std::vector<std::string> target_pipes = {"NetCDFPipe"};

    app.add_option("interface_cdl", interface_cdl, "The input .cdl file path")->required();
    app.add_flag("--header", create_header, "Create the interface header");
    app.add_flag("--source", create_source, "Create the interface header");
    app.add_option("--target_pipes", target_pipes,
                   "Create interfaces for specific pipes (NetCDFPipe, ZeroMQPipe).");

    CLI11_PARSE(app, argc, argv);

    if (create_header && create_source)
    {
        fmt::print("Interface generator: select either --header or --source.\n");
        return 1;
    }

    if (create_header)
    {
        generate(interface_cdl, Generator::GenerateTarget::Header);
    }
    if (create_source)
    {
        generate(interface_cdl, Generator::GenerateTarget::Source);
    }

    return 0;
}