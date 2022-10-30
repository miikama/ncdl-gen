
#include <iostream>

#include <fmt/core.h>

#include "logging.h"
#include "parser.h"
#include "syntax.h"
#include "types.h"

namespace ncdlgen
{

std::string OpaqueType::description(int indent) const
{
    Description description(indent);
    description << fmt::format("OpaqueType opaque({}) {}", m_length, m_name);
    return description.description;
}
std::string EnumType::description(int indent) const
{
    Description description(indent);
    description << fmt::format("EnumType {} {}", m_name, name_for_type(m_type));
    description.push_indent();
    description.push_indent();
    description.push_indent();
    for (auto &value : m_values)
    {
        description << fmt::format("{} = {}", value.name, value.value);
    }
    return description.description;
}
std::string VLenType::description(int indent) const
{
    Description description(indent);
    description << fmt::format("VLenType {} (*)", name_for_type(m_type));
    return description.description;
}

std::string Types::description(int indent) const
{
    Description description(indent, false);
    description << "Types\n";
    for (auto &type : m_types)
    {
        description << type->description(indent + 1);
    }
    return description.description;
}

std::string Dimensions::description(int indent) const
{
    Description description(indent);
    description << "Dimensions";
    for (auto &dimension : m_dimensions)
    {
        description << dimension.description(indent + 1);
    }
    return description.description;
}

std::string Dimension::description(int indent) const
{
    Description description(indent, false);
    if (m_length == 0)
    {
        description << fmt::format("{} = unlimited", m_name);
    }
    else
    {
        description << fmt::format("{} = {}", m_name, m_length);
    }
    return description.description;
}

std::string RootGroup::description(int indent) const
{
    Description description(indent);
    description << "RootGroup " + m_name;
    if (m_group)
    {
        Description group_description(indent, false);
        group_description << m_group->description(indent + 1);
        return description.description + group_description.description;
    }
    return description.description;
}

void RootGroup::print_tree() { std::cout << description(0); }

std::optional<Dimension> Dimension::parse(Parser &parser)
{

    auto next_token = parser.peek();
    if (!next_token || is_keyword(next_token->content()))
    {
        return {};
    }
    auto name = parser.pop();
    auto equals = parser.pop_specific({"="});
    auto value = parser.pop();
    auto line_end_or_comma = parser.pop_specific({",", ";"});
    if (!name || !equals || !value || !line_end_or_comma)
    {
        return {};
    }

    Dimension dim{};
    dim.m_name = name->content();
    if (value->content() == "UNLIMITED" || value->content() == "unlimited")
    {
        dim.m_length = 0;
    }
    else
    {
        dim.m_length = std::stoi(std::string(value->content()));
    }
    return dim;
}

std::optional<Dimensions> Dimensions::parse(Parser &parser)
{
    // dimensions:
    //     lat = 10, lon = 5, time = unlimited ;

    Dimensions dimensions{};
    dimensions.m_name = "dimensions:";
    while (auto dimension = Dimension::parse(parser))
    {
        dimensions.m_dimensions.push_back(*dimension);
    }
    if (dimensions.m_dimensions.empty())
    {
        return {};
    }
    return dimensions;
}

std::string VariableDimension::description(int indent) const { return m_name; }

std::optional<VariableDimension> VariableDimension::parse(Parser &parser)
{
    auto dim_name = parser.pop();
    auto comma_or_close_brace = parser.pop_specific({")", ","});
    if (!dim_name || !comma_or_close_brace)
    {
        return {};
    }
    VariableDimension dim{dim_name->content()};
    return dim;
}

std::string Variable::description(int indent) const
{
    Description description(indent, false);
    description << fmt::format("{} {}", name_for_type(m_type), m_name);

    if (m_dimensions.empty())
    {
        return description.description;
    }

    std::string dim_description = "";
    dim_description += fmt::format(" ({}", m_dimensions.front().description(0));
    for (size_t i = 1; i < m_dimensions.size(); i++)
    {
        dim_description += fmt::format(", {}", m_dimensions[i].description(0));
    }
    dim_description += ")";
    description.indent = 0;
    description << dim_description;
    return description.description;
}

std::optional<Variable> Variable::parse(Parser &parser)
{
    auto next_token = parser.peek();
    if (!next_token || is_keyword(next_token->content()))
    {
        return {};
    }

    // variables:
    //    ubyte   tag;
    //    double  p(time,lat,lon);
    //    long    rh(time,lat,lon);
    //    string  country(time,lat,lon);
    //    long    lat(lat), lon(lon), time(time);
    //    float   Z(time,lat,lon), t(time,lat,lon);
    auto type = parser.pop_type();
    auto name = parser.pop();
    auto line_end_or_open_bracket = parser.pop_specific({"(", ";"});
    if (!type || !name || !line_end_or_open_bracket)
    {
        return {};
    }

    Variable var{};
    var.m_name = name->content();
    auto surely_this_is_type_already = type_for_token(*type);
    if (!surely_this_is_type_already)
    {
        return {};
    }
    var.m_type = *surely_this_is_type_already;

    if (line_end_or_open_bracket->content() == ";")
    {
        return var;
    }

    while (auto dimension = VariableDimension::parse(parser))
    {
        var.m_dimensions.push_back(*dimension);
        if (parser.peek() && parser.peek()->content() == ";") {
            break;
        }
    }
    auto line_end = parser.pop_specific({";"});

    if (!line_end)
    {
        std::cout << "Could not find line end for variable definition\n";
        return {};
    }

    return var;
}

std::string Variables::description(int indent) const
{
    Description description(indent);
    description << "Variables";
    for (auto &variable : m_variables)
    {
        description << variable.description(indent + 1);
    }
    return description.description;
}

std::optional<Variables> Variables::parse(Parser &parser)
{
    Variables variables{};
    variables.m_name = "variables:";
    while (auto variable = Variable::parse(parser))
    {
        std::cout << "actually parsin var going at " << parser.peek()->content()
                  << "\n";
        variables.m_variables.push_back(*variable);
    }
    return variables;
}

std::optional<EnumValue> EnumValue::parse(Parser &parser)
{
    auto next_token = parser.peek();
    if (!next_token || is_keyword(next_token->content()))
    {
        return {};
    }
    if (next_token->content() == ";")
    {
        parser.pop();
        return {};
    }

    auto name = parser.pop();
    auto equals = parser.pop_specific({"="});
    auto value = parser.pop();
    auto bracket_or_comma = parser.pop_specific({",", "}"});
    if (!name || !equals || !value || !bracket_or_comma)
    {
        return {};
    }

    EnumValue enum_value{.name = std::string(name->content()),
                         .value = std::stoi(std::string(value->content()))};
    return enum_value;
}

std::unique_ptr<Type> Type::parse(Parser &parser)
{

    auto next_token = parser.peek();
    if (!next_token || is_keyword(next_token->content()))
    {
        return nullptr;
    }

    auto type_name = parser.pop();
    if (!type_name)
        return nullptr;

    if (type_name->content() == "opaque")
    {
        //     opaque(11) opaque_t;
        auto left_brace = parser.pop();
        auto opaque_size = parser.pop();
        auto right_brace = parser.pop();
        auto opaque_name = parser.pop();
        auto line_end = parser.pop();
        if (!left_brace || !opaque_size || !right_brace || !opaque_name ||
            !line_end)
            return nullptr;

        auto type = std::make_unique<OpaqueType>(
            opaque_name->content(),
            std::stoi(std::string(opaque_size->content())));
        return type;
    }

    auto actual_type = type_for_token(*type_name);
    if (!actual_type)
        return {};

    if (parser.peek() && parser.peek()->content() == "enum")
    {
        //     ubyte enum enum_t {Clear = 0, Cumulonimbus = 1, Stratus = 2};
        parser.pop();
        auto enum_name = parser.pop();
        auto left_bracket = parser.pop();
        if (!left_bracket || !enum_name)
            return {};

        auto enum_type =
            std::make_unique<EnumType>(enum_name->content(), *actual_type);

        while (auto enum_entry = EnumValue::parse(parser))
        {
            enum_type->m_values.push_back(*enum_entry);
        }
        return enum_type;
    }

    //     int(*) vlen_t;
    auto left_bracket = parser.pop();
    auto star = parser.pop();
    auto right_bracket = parser.pop();
    auto vlen_name = parser.pop();
    auto line_end = parser.pop();
    if (!left_bracket || !star || !right_bracket || !vlen_name || !line_end)
    {
        return {};
    }
    if (left_bracket->content() != "(" || star->content() != "*" ||
        right_bracket->content() != ")")
    {
        return {};
    }

    return std::make_unique<VLenType>(vlen_name->content(), *actual_type);
}

std::optional<Types> Types::parse(Parser &parser)
{
    // types:
    //     ubyte enum enum_t {Clear = 0, Cumulonimbus = 1, Stratus = 2};
    //     opaque(11) opaque_t;
    //     int(*) vlen_t;
    Types types{};
    types.m_name = "types:";
    while (auto type = Type::parse(parser))
    {
        types.m_types.push_back(std::move(type));
    }
    if (types.m_types.empty())
    {
        return {};
    }
    return types;
}

std::string Group::description(int indent) const
{
    Description description(indent, false);
    description << "Group " + m_name + "\n";
    if (m_types)
    {
        description << m_types->description(indent + 1);
    }
    if (m_dimensions)
    {
        description << m_dimensions->description(indent + 1);
    }
    if (m_variables)
    {
        description << m_variables->description(indent + 1);
    }
    for (auto &group : m_groups)
    {
        description << group.description(indent + 1);
    }
    return description.description;
}

std::optional<Group> Group::parse(Parser &parser)
{

    auto group_name = parser.pop();
    auto left_bracket = parser.pop();
    if (!group_name || !left_bracket)
    {
        return {};
    }
    Group group{};
    group.m_name = group_name->content();

    while (auto content = parser.pop())
    {
        if (content->content() == "dimensions:")
        {
            std::cout << "parsing dimensions\n";
            group.m_dimensions = Dimensions::parse(parser);
        }
        else if (content->content() == "types:")
        {
            std::cout << "parsing types\n";
            group.m_types = Types::parse(parser);
        }
        else if (content->content() == "data:")
        {
            std::cout << "parsing data\n";
        }
        else if (content->content() == "variables:")
        {
            std::cout << "parsing variables\n";
            group.m_variables = Variables::parse(parser);
        }
        else if (content->content() == "group:")
        {
            std::cout << "parsing group\n";
            if (auto child_group = Group::parse(parser))
            {
                group.m_groups.push_back(std::move(*child_group));
            }
        }
        else if (content->content() == "}")
        {
            return group;
        }
    }
    return {};
}

std::optional<RootGroup> RootGroup::parse(Parser &parser)
{
    auto netcdf = parser.pop();

    if (!netcdf || netcdf->content() != "netcdf")
    {
        return {};
    }

    RootGroup root{};
    root.m_group = Group::parse(parser);

    return root;
}

} // namespace ncdlgen
