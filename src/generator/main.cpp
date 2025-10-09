#include <unordered_map>

#include "CLI/CLI.hpp"
#include <fmt/core.h>

#include "generator.h"
#include "utils.h"

using namespace ncdlgen;

void generate(const std::string& input_cdl, Generator::GenerateTarget target,
              const std::vector<std::string>& target_pipes, const std::string& interface_name,
              const std::string& namespace_name, bool use_library_include)
{
    // The pipe includes for internal use in ncdlgen
    std::unordered_map<std::string, std::string> supported_pipes = {
        {"NetCDFPipe", "\"pipes/netcdf_pipe.h\""},
        {"ZeroMQPipe", "\"pipes/zeromq_pipe.h\""},
    };

    // The pipe includes when using ncdlgen as library
    std::unordered_map<std::string, std::string> supported_library_pipes = {
        {"NetCDFPipe", "<ncdlgen/netcdf_pipe.h>"},
        {"ZeroMQPipe", "<ncdlgen/zeromq_pipe.h>"},
    };

    // Support internal and external use
    auto& pipes = use_library_include ? supported_library_pipes : supported_pipes;

    // The interface includes for internal use in ncdlgen
    std::vector<std::string> supported_interfaces = {"\"vector_interface.h\""};

    // The interface includes when using ncdlgen as library
    std::vector<std::string> supported_library_interfaces = {"<ncdlgen/vector_interface.h>"};

    // Support internal and external use
    auto interfaces = use_library_include ? supported_library_interfaces : supported_interfaces;

    Generator::Options options{
        .target = target, .header_name = interface_name, .generated_namespace = namespace_name};

    options.interface_headers = interfaces;
    options.serialisation_pipes = target_pipes;
    options.pipe_headers = {};
    for (auto& pipe : target_pipes)
    {
        if (pipes.find(pipe) == pipes.end())
        {
            throw std::runtime_error(fmt::format("Interface Generator: Unsupported pipe {}.", pipe));
        }
        options.pipe_headers.push_back(pipes.at(pipe));
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
    std::string interface_name{};
    std::string namespace_name{"ncdlgen"};
    bool use_library_include{};

    app.add_option("interface_cdl", interface_cdl, "The input .cdl file path")->required();
    app.add_flag("--header", create_header, "Create the interface header");
    app.add_flag("--source", create_source, "Create the interface header");
    app.add_option("--target_pipes", target_pipes,
                   "Create interfaces for specific pipes (NetCDFPipe, ZeroMQPipe).")
        ->expected(0, -1);
    app.add_option("--interface_class_name", interface_name, "The name of the generated interface class");
    app.add_option("--interface_namespace_name", namespace_name,
                   "The name of the namespace of generated interface");
    app.add_flag("--use_library_include", use_library_include,
                 "Include files as '<ncdlgen/interface.h> (true) or 'interface.h' (false)");

    CLI11_PARSE(app, argc, argv);

    if (create_header && create_source)
    {
        fmt::print("Interface generator: select either --header or --source.\n");
        return 1;
    }

    if (create_header)
    {
        generate(interface_cdl, Generator::GenerateTarget::Header, target_pipes, interface_name,
                 namespace_name, use_library_include);
    }
    if (create_source)
    {
        generate(interface_cdl, Generator::GenerateTarget::Source, target_pipes, interface_name,
                 namespace_name, use_library_include);
    }

    return 0;
}