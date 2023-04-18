
#include <cassert>

#include <fmt/core.h>

#include "generator.h"
#include "parser.h"
#include "syntax.h"
#include "tokeniser.h"
#include "utils.h"

namespace ncdlgen
{

std::string_view
Generator::container_for_dimensions(const std::vector<ncdlgen::VariableDimension>& dimensions)
{
    assert(dimensions.size() == 1);
    return options.container_name;
}

void Generator::dump_header(const ncdlgen::Group& group, int indent)
{
    assert(indent >= 0);
    auto indent_str = std::string(indent * 2, ' ');
    fmt::print("{}struct {}\n", indent_str, group.name());
    fmt::print("{}{{\n", indent_str);
    for (auto& variable : group.variables())
    {
        auto indent_str_inner = fmt::format("{}{}", indent_str, std::string((indent + 1) * 2, ' '));
        if (!variable.dimensions().empty())
        {
            fmt::print("{}{}<{}> {};\n", indent_str_inner, container_for_dimensions(variable.dimensions()),
                       variable.type().name(), variable.name());
        }
        else
        {
            fmt::print("{}{} {};\n", indent_str_inner, variable.type().name(), variable.name());
        }
    }
    for (auto& sub_group : group.groups())
    {
        dump_header(sub_group, indent + 1);
        fmt::print("{}{} {}_g{{}};\n", std::string((indent + 1) * 2, ' '), sub_group.name(),
                   sub_group.name());
    }
    fmt::print("{}}};\n\n", indent_str);
}

void Generator::dump_header_reading(const ncdlgen::Group& group, const std::string_view fully_qualified_struct_name)
{
    fmt::print("void read({}& interface, {}&);\n\n", "NetCDFInterface", fully_qualified_struct_name);

    for (auto& sub_group : group.groups())
    {
        auto sub_group_name = fmt::format("{}::{}", fully_qualified_struct_name, sub_group.name());
        dump_header_reading(sub_group, sub_group_name);
    }
}

void Generator::dump_header_writing(const ncdlgen::Group& group, const std::string_view fully_qualified_struct_name)
{
    fmt::print("void write({}& interface, const {}&);\n\n", "NetCDFInterface", fully_qualified_struct_name);

    for (auto& sub_group : group.groups())
    {
        auto sub_group_name = fmt::format("{}::{}", fully_qualified_struct_name, sub_group.name());
        dump_header_writing(sub_group, sub_group_name);
    }
}

void Generator::dump_header_namespace(const ncdlgen::Group& group)
{
    fmt::print("namespace ncdlgen {{\n\n");

    dump_header(group, 0);

    dump_header_reading(group, group.name());

    dump_header_writing(group, group.name());

    fmt::print("}};\n");
}

void Generator::dump_source_read_group(const ncdlgen::Group& group, const std::string_view group_path,
                                const std::string_view name_space_name)
{
    auto fully_qualified_struct_name = fmt::format("{}::{}", name_space_name, group.name());
    auto name_space_root = split_string(name_space_name, ':').at(0);

    fmt::print("void {}::read({}& interface, {}& {}){{\n", name_space_root, "NetCDFInterface",
               fully_qualified_struct_name, group.name());

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

    for (auto& sub_group : group.groups())
    {
        fmt::print("  {}::read(interface, {}.{}_g);\n", name_space_name, group.name(), sub_group.name());
    }
    fmt::print("}}\n\n");

    for (auto& sub_group : group.groups())
    {
        auto sub_group_path = fmt::format("{}/{}", group_path, sub_group.name());
        dump_source_read_group(sub_group, sub_group_path, fully_qualified_struct_name);
    }
}

void Generator::dump_source_write_group(const ncdlgen::Group& group, const std::string_view group_path,
                                 const std::string_view name_space_name)
{
    auto fully_qualified_struct_name = fmt::format("{}::{}", name_space_name, group.name());
    auto name_space_root = split_string(name_space_name, ':').at(0);

    fmt::print("void {}::write({}& interface, const {}& data){{\n", name_space_root, "NetCDFInterface",
               fully_qualified_struct_name);

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

    for (auto& sub_group : group.groups())
    {
        fmt::print("  {}::write(interface, data.{}_g);\n", name_space_name, sub_group.name());
    }

    fmt::print("}}\n\n");

    for (auto& sub_group : group.groups())
    {
        auto sub_group_path = fmt::format("{}/{}", group_path, sub_group.name());
        dump_source_write_group(sub_group, sub_group_path, fully_qualified_struct_name);
    }
}

void Generator::dump_source(const ncdlgen::Group& group, const std::string_view group_path)
{
    fmt::print("#include \"{}.h\"\n\n", "generated_simple");

    // writing
    dump_source_write_group(group, group_path, "ncdlgen");

    // reading
    dump_source_read_group(group, group_path, "ncdlgen");
}

void Generator::dump_source_headers(const ncdlgen::Group& group)
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
        dump_source_headers(group);
        dump_header_namespace(group);
    }
    else
    {
        dump_source(group, "");
    }
}

} // namespace ncdlgen
