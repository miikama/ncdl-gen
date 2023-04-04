
#include <cassert>
#include <sstream>

#include <fmt/core.h>

#include "generator.h"
#include "parser.h"
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
        std::stringstream ss{};
        if (!variable.dimensions().empty())
        {
            ss << fmt::format("{}{}<{}> {}", indent_str, container_for_dimensions(variable.dimensions()),
                              variable.type().name(), variable.name());
        }
        else
        {
            ss << fmt::format("{}{} {}", indent_str, variable.type().name(), variable.name());
        }

        fmt::print("{}{};\n", indent_str, ss.str());
    }
    for (auto& sub_group : group.groups())
    {
        dump_contents(sub_group, indent + 1);
        fmt::print("{}{} {}_g{{}};\n", std::string((indent + 1) * 2, ' '), sub_group.name(),
                   sub_group.name());
    }
    fmt::print("{}}};\n", indent_str);
}

void dump_namespace(const ncdlgen::Group& group)
{
    fmt::print("namespace ncdlgen {{\n");

    dump_contents(group, 0);

    fmt::print("}};\n");
}

void dump_headers(const ncdlgen::Group& group)
{
    fmt::print("#pragma once\n\n");
    fmt::print("#include \"stdint.h\"\n\n");
    fmt::print("#include <vector>\n\n");
    fmt::print("#include \"netcdf_interface.h\"\n");
    fmt::print("#include \"utils.h\"\n");
    fmt::print("#include \"vector_interface.h\"\n");

    dump_namespace(group);
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

    dump_headers(*(ast->group));
}

} // namespace ncdlgen
