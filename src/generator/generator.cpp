
#include <cassert>
#include <sstream>

#include <fmt/core.h>

#include "generator.h"
#include "parser.h"
#include "syntax.h"
#include "tokeniser.h"
#include "utils.h"

namespace ncdlgen
{

std::string container_for_dimensions(const std::vector<ncdlgen::VariableDimension>& dimensions)
{
    assert(dimensions.size() == 1);
    return "std::vector";
}

void dump_contents(const ncdlgen::Group& group, int indent)
{
    assert(indent >= 0);
    auto indent_str = std::string(indent * 2, ' ');
    fmt::print("{}struct {}\n", indent_str, group.name());
    fmt::print("{}{{\n", indent_str);
    for (auto& variable : group.variables())
    {
        auto indent_str_inner = std::string((indent + 1) * 2, ' ');
        std::stringstream ss{};
        if (!variable.dimensions().empty())
        {
            ss << fmt::format("{}{}<{}> {}", indent_str_inner,
                              container_for_dimensions(variable.dimensions()), variable.type().name(),
                              variable.name());
        }
        else
        {
            ss << fmt::format("{}{} {}", indent_str_inner, variable.type().name(), variable.name());
        }

        fmt::print("{}{};\n", indent_str, ss.str());
    }
    for (auto& sub_group : group.groups())
    {
        dump_contents(sub_group, indent + 1);
        fmt::print("{}{} {}_g{{}};\n", std::string((indent + 1) * 2, ' '), sub_group.name(),
                   sub_group.name());
    }
    fmt::print("{}}};\n\n", indent_str);
}

void dump_reading(const ncdlgen::Group& group)
{
    fmt::print("template <typename StructureType> StructureType read({}& interface);\n\n", "NetCDFInterface");
}

void dump_writing(const ncdlgen::Group& group)
{
    fmt::print("void write({}& interface, const {}&);\n\n", "NetCDFInterface", group.name());
}

void dump_namespace(const ncdlgen::Group& group)
{
    fmt::print("namespace ncdlgen {{\n\n");

    dump_contents(group, 0);

    dump_reading(group);

    dump_writing(group);

    fmt::print("}};\n");
}

void dump_read_group(const ncdlgen::Group& group, const std::string_view group_path)
{

    fmt::print("template <> {} {}::read<{}>({}& interface){{\n", group.name(), "ncdlgen", group.name(),
               "NetCDFInterface");
    fmt::print("  {} {};\n", group.name(), group.name());

    for (auto& variable : group.variables())
    {
        auto full_path = fmt::format("{}/{}", group_path, variable.name());
        if (variable.is_scalar())
        {
            fmt::print("  {}.{} = interface.read<{},{},{}>(\"{}\");\n", group.name(), variable.name(),
                       variable.type().name(), variable.type().name(), "VectorInterface", full_path);
        }
        else
        {
            auto container_type_name =
                fmt::format("{}::container_type_t<{}>", "VectorInterface", variable.type().name());
            fmt::print("  {}.{} = interface.read<{},{},{}>(\"{}\");\n", group.name(), variable.name(),
                       container_type_name, variable.type().name(), "VectorInterface", full_path);
        }
    }

    for (auto& group : group.groups())
    {
        fmt::print("  // TODO: reading of group '{}' not generated\n", group.name());
    }
    fmt::print("  return {};\n", group.name());
    fmt::print("}}\n");
}

void dump_write_group(const ncdlgen::Group& group, const std::string_view group_path)
{

    fmt::print("void {}::write({}& interface, const {}& data){{\n", "ncdlgen", "NetCDFInterface",
               group.name());

    for (auto& variable : group.variables())
    {
        auto full_path = fmt::format("{}/{}", group_path, variable.name());
        if (variable.is_scalar())
        {
            fmt::print("  interface.write<{},{},{}>(\"{}\", data.{});\n", variable.type().name(),
                       variable.type().name(), "VectorInterface", full_path, variable.name());
        }
        else
        {
            auto container_type_name =
                fmt::format("{}::container_type_t<{}>", "VectorInterface", variable.type().name());
            fmt::print("  interface.write<{},{},{}>(\"{}\", data.{});\n", container_type_name,
                       variable.type().name(), "VectorInterface", full_path, variable.name());
        }
    }

    for (auto& group : group.groups())
    {
        fmt::print("  // TODO: writing of group '{}' not generated\n", group.name());
    }

    fmt::print("}}\n");
}

void dump_source(const ncdlgen::Group& group, const std::string_view group_path)
{
    fmt::print("#include \"{}.h\"\n", "generated_simple");
    fmt::print("using namespace {};\n\n", "ncdlgen");

    // writing
    dump_write_group(group, group_path);

    // reading
    dump_read_group(group, group_path);
}

void dump_headers(const ncdlgen::Group& group)
{
    fmt::print("#pragma once\n\n");
    fmt::print("#include \"stdint.h\"\n");
    fmt::print("\n");
    fmt::print("#include <vector>\n");
    fmt::print("\n");
    fmt::print("#include \"netcdf_interface.h\"\n");
    fmt::print("#include \"utils.h\"\n");
    fmt::print("#include \"vector_interface.h\"\n");
    fmt::print("\n");
}

void Generator::generate(const std::string_view input_cdl)
{
    auto copy = std::string(input_cdl);

    ncdlgen::Tokeniser tokeniser{copy};
    auto tokens = tokeniser.tokenise();

    ncdlgen::Parser parser{tokens};
    auto ast = parser.parse();
    if (!ast.has_value() || !ast->group)
    {
        fmt::print("Parsing file failed\n");
        return;
    }
    // ast->print_tree();

    auto& group = *(ast->group);
    if (options.target == GenerateTarget::Header)
    {
        dump_headers(group);
        dump_namespace(group);
    }
    else
    {
        dump_source(group, "");
    }
}

} // namespace ncdlgen
