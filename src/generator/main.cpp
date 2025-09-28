#include <unordered_map>

#include "CLI/CLI.hpp"
#include <fmt/core.h>

#include "generator.h"
#include "utils.h"

using namespace ncdlgen;

void generate(const std::string& input_cdl, Generator::GenerateTarget target,
              const std::vector<std::string>& target_pipes)
{
    std::unordered_map<std::string, std::string> supported_pipes = {
        {"NetCDFPipe", "pipes/netcdf_pipe.h"},
        {"ZeroMQPipe", "pipes/zeromq_pipe.h"},
    };
    Generator::Options options{.target = target};

    options.serialisation_pipes = target_pipes;
    options.pipe_headers = {};
    for (auto& pipe : target_pipes)
    {
        if (supported_pipes.find(pipe) == supported_pipes.end())
        {
            throw std::runtime_error(fmt::format("Interface Generator: Unsupported pipe {}.", pipe));
        }
        options.pipe_headers.push_back(supported_pipes.at(pipe));
    }

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
    std::vector<std::string> target_pipes = {"NetCDFPipe", "ZeroMQPipe"};

    app.add_option("interface_cdl", interface_cdl, "The input .cdl file path")->required();
    app.add_flag("--header", create_header, "Create the interface header");
    app.add_flag("--source", create_source, "Create the interface header");
    app.add_option("--target_pipes", target_pipes,
                   "Create interfaces for specific pipes (NetCDFPipe, ZeroMQPipe).")
        ->expected(0, -1);

    CLI11_PARSE(app, argc, argv);

    if (create_header && create_source)
    {
        fmt::print("Interface generator: select either --header or --source.\n");
        return 1;
    }

    if (create_header)
    {
        generate(interface_cdl, Generator::GenerateTarget::Header, target_pipes);
    }
    if (create_source)
    {
        generate(interface_cdl, Generator::GenerateTarget::Source, target_pipes);
    }

    return 0;
}