
#include <iostream>

#include "parser.h"
#include "types.h"
#include "unordered_set"

namespace ncdlgen
{

struct Description
{

    Description(const int indent, bool add_new_line = true)
        : indent(indent), new_line(add_new_line)
    {
    }

    void operator<<(const std::string &input)
    {
        std::string line{};
        for (size_t i = 0; i < indent * 2; i++)
        {
            line += " ";
        }
        line += input;
        if (new_line)
        {
            line += "\n";
        }
        description += line;
    }

    void push_indent() { indent++; }

    std::string description{};
    size_t indent{};
    bool new_line{};
};

bool is_keyword(const std::string_view word)
{
    static std::unordered_set<std::string_view> keywords{};
    keywords.insert("variables:");
    keywords.insert("dimensions:");
    keywords.insert("types:");
    keywords.insert("data:");
    keywords.insert("group:");
    return keywords.find(word) != keywords.end();
}

std::optional<NetCDFType> type_for_token(const Token &token)
{
    if (token.content() == "char")
    {
        return NetCDFType::Char;
    }
    else if (token.content() == "byte")
    {
        return NetCDFType::Byte;
    }
    else if (token.content() == "ubyte")
    {
        return NetCDFType::Ubyte;
    }
    else if (token.content() == "short")
    {
        return NetCDFType::Short;
    }
    else if (token.content() == "ushort")
    {
        return NetCDFType::Ushort;
    }
    else if (token.content() == "int")
    {
        return NetCDFType::Int;
    }
    else if (token.content() == "uint")
    {
        return NetCDFType::Uint;
    }
    else if (token.content() == "long")
    {
        return NetCDFType::Long;
    }
    else if (token.content() == "int64")
    {
        return NetCDFType::Int64;
    }
    else if (token.content() == "uint64")
    {
        return NetCDFType::Uint64;
    }
    else if (token.content() == "float")
    {
        return NetCDFType::Float;
    }
    else if (token.content() == "real")
    {
        return NetCDFType::Real;
    }
    else if (token.content() == "double")
    {
        return NetCDFType::Double;
    }
    else if (token.content() == "string")
    {
        return NetCDFType::String;
    }
    else
    {
        return {};
    }
}

std::string name_for_type(const NetCDFType &type)
{
    switch (type)
    {
    case NetCDFType::Char:
        return "char";
    case NetCDFType::Byte:
        return "byte";
    case NetCDFType::Ubyte:
        return "ubyte";
    case NetCDFType::Short:
        return "short";
    case NetCDFType::Ushort:
        return "ushort";
    case NetCDFType::Int:
        return "int";
    case NetCDFType::Uint:
        return "uint";
    case NetCDFType::Long:
        return "long";
    case NetCDFType::Int64:
        return "int64";
    case NetCDFType::Uint64:
        return "uint64";
    case NetCDFType::Float:
        return "float";
    case NetCDFType::Real:
        return "real";
    case NetCDFType::Double:
        return "double";
    case NetCDFType::String:
        return "string";
    default:
        return "unknown";
    }
}

std::string OpaqueType::description(int indent) const
{
    Description description(indent);
    description << std::string("OpaqueType ") + "opaque(" +
                       std::to_string(m_length) + ") " + Element::m_name;
    return description.description;
}
std::string EnumType::description(int indent) const
{
    Description description(indent);
    description << "EnumType " + m_name + " " + name_for_type(m_type);
    description.push_indent();
    description.push_indent();
    description.push_indent();
    for (auto &value : m_values)
    {
        description << value.name + "=" + std::to_string(value.value);
    }
    return description.description;
}
std::string VLenType::description(int indent) const
{
    Description description(indent);
    description << "VLenType " + name_for_type(m_type) + "(*)";
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
    for (auto &group : m_groups)
    {
        description << group.description(indent + 1);
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
        description << m_name + " = unlimited";
    }
    else
    {
        description << m_name + " = " + std::to_string(m_length);
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
    auto equals = parser.pop();
    auto value = parser.pop();
    auto line_end_or_comma = parser.pop();
    if (!name || !equals || !value || !line_end_or_comma)
    {
        return {};
    }
    bool is_line_or_comma = line_end_or_comma->content() == "," ||
                            line_end_or_comma->content() == ";";
    if (equals->content() != "=" || !is_line_or_comma)
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

std::optional<EnumValue> EnumValue::parse(Parser &parser)
{
    // TODO: almost Dimension::parse
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
    auto equals = parser.pop();
    auto value = parser.pop();
    auto bracket_or_comma = parser.pop();
    if (!name || !equals || !value || !bracket_or_comma)
    {
        return {};
    }
    bool is_bracket_or_comma = bracket_or_comma->content() == "," ||
                               bracket_or_comma->content() == "}";
    if (equals->content() != "=" || !is_bracket_or_comma)
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
            group.m_types = Types::parse(parser);
            std::cout << "parsing data\n";
        }
        else if (content->content() == "variables:")
        {
            std::cout << "parsing variables\n";
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
