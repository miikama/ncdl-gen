
#include <fmt/core.h>

#include "logging.h"
#include "parser.h"
#include "syntax.h"
#include "types.h"
#include "utils.h"

namespace ncdlgen
{

std::string_view NetCDFType::name() const
{
    return std::visit(
        [](auto &&arg) -> std::string_view {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, NetCDFElementaryType>)
            {
                return name_for_type(arg);
            }
            else if constexpr (std::is_same_v<T, ComplexType>)
            {
                return arg.name();
            }
            else
            {
                static_assert(always_false_v<T>, "Visiting unsupported type!");
            }
        },
        type);
}

std::string Number::as_string() const
{
    return std::visit([](auto &&arg) -> std::string { return fmt::format("{}", arg); }, value);
}

std::string Array::as_string() const
{
    if (data.empty())
    {
        return "[ ]";
    }
    std::string desc{"["};
    desc += data.front().as_string();
    for (size_t i = 1; i < data.size(); i++)
    {
        desc += ", " + data.at(i).as_string();
    }
    desc += "]";
    return desc;
}

std::string OpaqueType::as_string() const { return fmt::format("OpaqueType opaque({}) {}", length, name); }
std::string EnumType::as_string() const
{
    std::string type_name = fmt::format("EnumType {} {}: [ ", name, name_for_type(type));
    for (auto &value : enum_values)
    {
        type_name += fmt::format(" {} = {} ", value.name, value.value);
    }
    return fmt::format("{} ]", type_name);
}
std::string VLenType::as_string() const { return fmt::format("VLenType {} (*)", name_for_type(type)); }

std::string ComplexType::description() const
{
    return std::visit([](auto &&arg) -> std::string { return arg.as_string(); }, type);
}

std::string ComplexType::name() const
{
    return std::visit([](auto &&arg) -> std::string { return arg.name; }, type);
}

std::string Types::description(int indent) const
{
    Description description(indent, true);
    description << "Types";
    description.push_indent();
    description.push_indent();
    description.push_indent();
    for (auto &type : types)
    {
        description << type.description();
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

void RootGroup::print_tree() { fmt::print(description(0)); }

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
    description << fmt::format("{} {}", m_type.name(), m_name);

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

std::optional<Variable> Variable::parse(Parser &parser, NetCDFType existing_type)
{
    auto name = parser.pop();
    auto line_end_or_open_bracket = parser.pop_specific({"(", ";"});
    if (!name || !line_end_or_open_bracket)
    {
        return {};
    }

    Variable var{};
    var.m_name = name->content();
    var.m_type = existing_type;

    if (line_end_or_open_bracket->content() == ";")
    {
        return var;
    }

    while (auto dimension = VariableDimension::parse(parser))
    {
        var.m_dimensions.push_back(*dimension);
        if (parser.peek_specific({";"}))
        {
            break;
        }
        // new definiton on same line with same type
        if (parser.peek_specific({","}))
        {
            return var;
        }
    }
    auto line_end = parser.pop_specific({";"});

    if (!line_end)
    {
        fmt::print("Could not find line end for variable definition for variable {}\n", name->content());
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
    if (m_attributes.empty())
    {
        return description.description;
    }

    description.push_indent();
    description << "Attributes";
    description.push_indent();
    description.push_indent();
    for (auto &attribute : m_attributes)
    {
        description << attribute.description(indent);
    }
    return description.description;
}

std::optional<Variables> Variables::parse(Parser &parser)
{
    Variables variables{};
    variables.m_name = "variables:";

    std::optional<NetCDFType> previous_type = {};
    while (auto variable = VariableDeclaration::parse(parser, previous_type))
    {
        if (std::holds_alternative<Variable>(*variable))
        {
            variables.m_variables.push_back(std::get<Variable>(*variable));

            // multiple variables in one line, continue to following
            if (parser.peek_specific({","}))
            {
                previous_type = variables.m_variables.back().type();
                parser.pop();
            }
            else
            {
                previous_type = {};
            }
        }
        else if (std::holds_alternative<Attribute>(*variable))
        {
            variables.m_attributes.push_back(std::get<Attribute>(*variable));
        }
        else
        {
            throw std::runtime_error("Incorrect type, this is bug.");
        }
    }
    return variables;
}

std::string Attribute::as_string() const
{
    // Use the visitor to go through all types
    return std::visit(
        [](auto &&arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>)
            {
                return arg;
            }
            else if constexpr (std::is_same_v<T, ValidRangeValue>)
            {
                return fmt::format("{} - {}", arg.start.as_string(), arg.end.as_string());
            }
            else if constexpr (std::is_same_v<T, FillValueAttributeValue>)
            {
                return arg.as_string();
            }
            else if constexpr (std::is_same_v<T, VariableData>)
            {
                return arg.as_string();
            }
            else
            {
                static_assert(always_false_v<T>, "non-exhaustive visitor!");
            }
        },
        m_value);
}

std::string Attribute::description(int indent) const
{
    std::string type_part{};
    if (m_type)
    {
        type_part += fmt::format("{} ", m_type->name());
    }
    std::string name_part{};
    if (m_variable_name)
    {
        name_part += *m_variable_name;
    }
    return fmt::format("{}{}:{} = {}", type_part, name_part, m_attribute_name, as_string());
}

std::optional<Attribute> Attribute::parse(Parser &parser, std::optional<NetCDFType> attribute_type)
{
    // Allowed attribute grammar,
    // https://manpages.ubuntu.com/manpages/xenial/man1/ncgen.1.html
    // attrdecl:
    //          ':' ident '=' datalist
    //      | typeref type_var_ref ':' ident '=' datalist
    //      | type_var_ref ':' ident '=' datalist
    //      | type_var_ref ':' _FILLVALUE '=' datalist
    //      | typeref type_var_ref ':' _FILLVALUE '=' datalist
    //      | type_var_ref ':' _STORAGE '=' conststring
    //      | type_var_ref ':' _CHUNKSIZES '=' intlist
    //      | type_var_ref ':' _FLETCHER32 '=' constbool
    //      | type_var_ref ':' _DEFLATELEVEL '=' constint
    //      | type_var_ref ':' _SHUFFLE '=' constbool
    //      | type_var_ref ':' _ENDIANNESS '=' conststring
    //      | type_var_ref ':' _NOFILL '=' constbool
    //      | ':' _FORMAT '=' conststring
    //      ;

    auto name = parser.pop();
    if (!name)
    {
        return {};
    }

    auto equals = parser.pop_specific({"="});
    if (!equals)
    {
        fmt::print("Could not find '=' for Attribute with parsed name {}\n", name->content());
        return {};
    }

    auto split_str = split_string_at(name->content(), ':');
    if (split_str.second.empty())
    {
        fmt::print("Splitting attr name failed for Attribute {}\n", name->content());
        return {};
    }
    // Global attribute
    bool is_global{split_str.first.empty()};

    Attribute attr{};
    attr.m_variable_name = split_str.first;
    attr.m_attribute_name = split_str.second;
    attr.m_type = attribute_type;

    // Currently supported string attributes
    if (attr.m_attribute_name == "long_name" || attr.m_attribute_name == "units")
    {
        auto value = parser.pop();
        if (!value || value->content().empty())
        {
            return {};
        }
        attr.m_value = std::string(value->content());
    }
    else if (attr.m_attribute_name == "_FillValue")
    {
        // TODO: fetch type for untyped attributes from the corresponding variable
        auto fill_value = parser.parse_number(attr.m_type.value_or(NetCDFElementaryType::Default));
        if (!fill_value)
        {
            fmt::print("Could not parse value for attribute '_FillValue'\n");
            return {};
        }
        attr.m_value = *fill_value;
    }
    else if (attr.m_attribute_name == "valid_range")
    {
        // TODO: fetch type for untyped attributes from the corresponding variable
        auto start = parser.parse_number(attr.m_type.value_or(NetCDFElementaryType::Default));
        auto comma = parser.pop_specific({","});
        auto end = parser.parse_number(attr.m_type.value_or(NetCDFElementaryType::Default));
        if (!start || !comma || !end)
        {
            fmt::print("Could not parse value for attribute 'valid_range'\n");
            return {};
        }
        attr.m_value = ValidRangeValue{*start, *end};
    }
    else if (is_global)
    {
        auto data = VariableData::parse(parser, attr.m_type.value_or(NetCDFElementaryType::Default));
        if (!data)
        {
            fmt::print("Parsing global attribute {} with type {} failed.\n", split_str.second, attribute_type->name());
            return {};
        }
        attr.m_value = *data;
    }
    else
    {
        fmt::print("Unsupported attribute '{}'\n", attr.m_attribute_name);
        return {};
    }

    auto line_end = parser.pop_specific({";"});

    if (!line_end)
    {
        fmt::print("Could not find line end for variable definition for attribute {}\n", attr.m_attribute_name);
        return {};
    }
    return attr;
}

std::string VariableData::as_string() const
{
    return std::visit([](auto &&arg) { return arg.as_string(); }, data);
}

std::optional<VariableData> VariableData::parse(Parser &parser, const NetCDFType &type)
{
    return std::visit(
        [&parser](auto &&arg) -> std::optional<VariableData> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, NetCDFElementaryType>)
            {
                return parser.parse_number(arg);
            }
            else if constexpr (std::is_same_v<T, ComplexType>)
            {
                return parser.parse_complex_type_data(arg);
            }
            else
            {
                static_assert(always_false_v<T>, "Visiting unsupported type!");
            }
        },
        type.type);
}

std::optional<VariableDeclaration::VariableDeclarationType>
VariableDeclaration::parse(Parser &parser, std::optional<NetCDFType> existing_type)
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
    // variable attributes
    //    lat:long_name = "latitude";
    //    lat:units = "degrees_north";
    //    lon:long_name = "longitude";
    //    lon:units = "degrees_east";
    //    time:units = "seconds since 1992-1-1 00:00:00";

    // Not the first variable for this line
    if (existing_type)
    {
        return Variable::parse(parser, *existing_type);
    }

    auto type = parser.peek_type();

    // try to parse an attribute because variables have types
    if (!type)
    {
        return Attribute::parse(parser, {});
    }

    // pop the type
    parser.pop();

    // peek the variable/attribute name
    auto name = parser.peek();
    if (!name)
    {
        fmt::print("Did not find name for variable\n");
        return {};
    }

    // If the name has ':', it is attribute
    if (!name->content().empty() && name->content().find(':') != std::string::npos)
    {
        return Attribute::parse(parser, type);
    }

    return Variable::parse(parser, *type);
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

    EnumValue enum_value{.name = std::string(name->content()), .value = std::stoi(std::string(value->content()))};
    return enum_value;
}

std::optional<ComplexType> ComplexType::parse(Parser &parser)
{

    auto next_token = parser.peek();
    if (!next_token || is_keyword(next_token->content()))
    {
        return {};
    }

    auto type_name = parser.pop();
    if (!type_name)
        return {};

    if (type_name->content() == "opaque")
    {
        //     opaque(11) opaque_t;
        auto left_brace = parser.pop();
        auto opaque_size = parser.pop();
        auto right_brace = parser.pop();
        auto opaque_name = parser.pop();
        auto line_end = parser.pop();
        if (!left_brace || !opaque_size || !right_brace || !opaque_name || !line_end)
            return {};

        return ComplexType{OpaqueType(opaque_name->content(), std::stoi(std::string(opaque_size->content())))};
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

        auto enum_type = EnumType(enum_name->content(), *actual_type);

        while (auto enum_entry = EnumValue::parse(parser))
        {
            enum_type.enum_values.push_back(*enum_entry);
        }
        return ComplexType(enum_type);
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
    if (left_bracket->content() != "(" || star->content() != "*" || right_bracket->content() != ")")
    {
        return {};
    }

    return ComplexType{VLenType(vlen_name->content(), *actual_type)};
}

std::optional<Types> Types::parse(Parser &parser)
{
    // types:
    //     ubyte enum enum_t {Clear = 0, Cumulonimbus = 1, Stratus = 2};
    //     opaque(11) opaque_t;
    //     int(*) vlen_t;
    Types types{};
    while (auto type = ComplexType::parse(parser))
    {
        types.types.push_back(std::move(*type));
    }
    if (types.types.empty())
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
    parser.push_group_stack(group);

    while (auto content = parser.pop())
    {
        if (content->content() == "dimensions:")
        {
            fmt::print("parsing dimensions\n");
            group.m_dimensions = Dimensions::parse(parser);
        }
        else if (content->content() == "types:")
        {
            fmt::print("parsing types\n");
            group.m_types = Types::parse(parser);
        }
        else if (content->content() == "data:")
        {
            fmt::print("parsing data\n");
        }
        else if (content->content() == "variables:")
        {
            fmt::print("parsing variables\n");
            group.m_variables = Variables::parse(parser);
        }
        else if (content->content() == "group:")
        {
            fmt::print("parsing group\n");
            if (auto child_group = Group::parse(parser))
            {
                group.m_groups.push_back(std::move(*child_group));
            }
        }
        else if (content->content() == "}")
        {
            parser.pop_group_stack();
            return group;
        }
    }
    parser.pop_group_stack();
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

const std::vector<ComplexType> &Group::types() const
{
    static std::vector<ComplexType> empty_types{};
    if (!m_types)
    {
        return empty_types;
    }
    return m_types->types;
}

std::vector<Variable>& Group::variables()
{
    static std::vector<Variable> variables{};
    if(!m_variables)
    {
        return variables;
    }
    return m_variables->variables();
    
}

} // namespace ncdlgen
