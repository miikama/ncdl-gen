
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
        [](auto&& arg) -> std::string_view
        {
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

std::optional<ComplexType> NetCDFType::as_complex_type() const
{
    if (!std::holds_alternative<ComplexType>(type))
    {
        return {};
    }
    else
    {
        return std::get<ComplexType>(type);
    }
}

std::string String::as_string() const { return value; }

std::string Number::as_string() const
{
    return std::visit([](auto&& arg) -> std::string { return fmt::format("{}", arg); }, value);
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
    for (auto& value : enum_values)
    {
        type_name += fmt::format(" {} = {} ", value.name, value.value);
    }
    return fmt::format("{} ]", type_name);
}
std::string VLenType::as_string() const
{
    return fmt::format("VLenType {} (*) {}", name_for_type(type), name);
}

std::string ArrayType::as_string() const
{
    return fmt::format("{} {} {}", name_for_type(type), name, dimensions.as_string());
}

void CompoundType::add_type(const std::string_view name, const ComplexType& type)
{
    types.push_back(type);
    type_names.push_back(std::string(name));
}

std::string CompoundType::as_string() const
{
    std::string type_name = fmt::format("CompoundType {}: [ ", name);
    if (types.size() == 1)
    {
        return fmt::format("{} ]", type_name);
    }
    for (std::size_t i = 0; i < types.size(); i++)
    {
        auto& child_type = types.at(i);
        auto& child_type_name = type_names.at(i);

        if (i == 0)
        {
            type_name += fmt::format("{} {}", child_type.name(), child_type_name);
        }
        else
        {
            type_name += fmt::format("; {} {}", child_type.name(), child_type_name);
        }
    }
    return fmt::format("{} ]", type_name);
}

std::optional<CompoundType> CompoundType::parse(Parser& parser)
{
    auto name = parser.pop_identifier();
    if (!name)
    {
        parser.log_parse_error("Could not find name for compound type.");
        return {};
    }

    CompoundType type{name->content()};
    auto start_bracket = parser.pop_specific({"{"});
    if (!start_bracket)
    {
        parser.log_parse_error(fmt::format("Could not find '{' for compound type '{}'.", name->content()));
        return {};
    }

    // Parse compound type child types
    while (true)
    {
        auto possible_type = parser.pop();
        if (!possible_type)
        {
            parser.log_parse_error("Could not find type name in compound type.");
            return {};
        }
        auto child_type = parser.resolve_type_for_name(possible_type->content());
        if (!child_type)
        {
            parser.log_parse_error(
                fmt::format("No type available to type name {}", possible_type->content()));
            return {};
        }
        auto child_name = parser.pop_identifier();
        if (!child_name)
        {
            parser.log_parse_error(fmt::format("No type name for type {} found.", possible_type->content()));
            return {};
        }
        auto stop_semicolon = parser.pop_specific({";"});
        if (!stop_semicolon)
        {
            parser.log_parse_error(
                fmt::format("Could not find ';' for compound type child type {}", child_name->content()));
            return {};
        }
        auto child_complex_type = child_type->as_complex_type();
        if (!child_complex_type)
        {
            parser.log_parse_error("Elementary types as part of compound types are not supported.");
            return {};
        }
        type.add_type(child_name->content(), *child_complex_type);

        auto close_bracket = parser.peek_specific({"}"});
        if (close_bracket)
        {
            parser.pop();
            break;
        }
    }

    return type;
}

std::string ComplexType::as_string() const
{
    return std::visit([](auto&& arg) -> std::string { return arg.as_string(); }, type);
}

std::string ComplexType::name() const
{
    return std::visit([](auto&& arg) -> std::string { return arg.name; }, type);
}

std::string Dimensions::description(int indent) const
{
    Description description(indent);
    description << "Dimensions";
    for (auto& dimension : dimensions)
    {
        description << dimension.description(indent + 1);
    }
    return description.description;
}

std::string Dimensions::as_string() const
{
    std::string description{"( "};
    for (auto& dimension : dimensions)
    {
        description += fmt::format("{} ", dimension.length);
    }
    return fmt::format("{})", description);
}

std::string Dimension::description(int indent) const
{
    Description description(indent, false);
    if (length == 0)
    {
        description << fmt::format("{} = unlimited", name);
    }
    else
    {
        description << fmt::format("{} = {}", name, length);
    }
    return description.description;
}

std::string RootGroup::description(int indent) const
{
    if (group)
    {
        Description group_description(indent, false);
        group_description << group->description(indent);
        return group_description.description;
    }
    return "";
}

void RootGroup::print_tree() { fmt::print(description(0)); }

std::optional<Dimension> Dimension::parse(Parser& parser)
{
    auto next_token = parser.peek();
    if (!next_token || is_group_end(next_token->content()))
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
    dim.name = name->content();
    if (value->content() == "UNLIMITED" || value->content() == "unlimited")
    {
        dim.length = 0;
    }
    else
    {
        dim.length = std::stoi(std::string(value->content()));
    }
    return dim;
}

std::optional<Dimensions> Dimensions::parse(Parser& parser)
{
    // dimensions:
    //     lat = 10, lon = 5, time = unlimited ;

    Dimensions dimensions{};
    while (auto dimension = Dimension::parse(parser))
    {
        dimensions.dimensions.push_back(*dimension);
    }
    if (dimensions.dimensions.empty())
    {
        return {};
    }
    return dimensions;
}

std::string VariableDimension::description(int indent) const { return m_name; }

std::optional<VariableDimension> VariableDimension::parse(Parser& parser)
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

NetCDFElementaryType Variable::basic_type() const
{
    if (std::holds_alternative<NetCDFElementaryType>(m_type.type))
    {
        return std::get<NetCDFElementaryType>(m_type.type);
    }
    fmt::print(stderr, "ERROR: Trying to retreive type from not basic type!\n");
    return NetCDFElementaryType::Default;
}

std::optional<Variable> Variable::parse(Parser& parser, NetCDFType existing_type)
{
    auto name = parser.pop();
    if (!name)
    {
        parser.log_parse_error("Could not find name for variable");
        return {};
    }
    auto line_end_or_open_bracket = parser.pop_specific({"(", ";"});
    if (!line_end_or_open_bracket)
    {
        parser.log_parse_error(fmt::format("Could not find '(' or ';' for variable {}", name->content()));
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
        parser.log_parse_error(fmt::format(
            "Could not find line end for variable definition for variable '{}'", name->content()));
        return {};
    }

    return var;
}

std::string Variables::description(int indent) const
{
    Description description(indent);
    description << "Variables";
    for (auto& variable : m_variables)
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
    for (auto& attribute : m_attributes)
    {
        description << attribute.description(indent);
    }
    return description.description;
}

void Variables::parse(Parser& parser, Group& group)
{
    std::optional<NetCDFType> previous_type = {};
    while (auto variable = VariableDeclaration::parse(parser, previous_type))
    {
        if (std::holds_alternative<Variable>(*variable))
        {
            group.variables().push_back(std::get<Variable>(*variable));

            // multiple variables in one line, continue to following
            if (parser.peek_specific({","}))
            {
                previous_type = group.variables().back().type();
                parser.pop();
            }
            else
            {
                previous_type = {};
            }
        }
        else if (std::holds_alternative<Attribute>(*variable))
        {
            group.attributes().push_back(std::get<Attribute>(*variable));
        }
        else
        {
            throw std::runtime_error("Incorrect type, this is bug.");
        }
    }
}

std::string Attribute::as_string() const
{
    // Use the visitor to go through all types
    return std::visit(
        [](auto&& arg) -> std::string
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>)
            {
                return arg;
            }
            else if constexpr (std::is_same_v<T, ValidRangeValue>)
            {
                return fmt::format("[{}, {}]", arg.start.as_string(), arg.end.as_string());
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

std::string Attribute::string_data() const
{
    return std::visit(
        [&](auto&& arg) -> std::string
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>)
            {
                return std::get<std::string>(m_value);
            }
            else
            {
                return arg.as_string();
            }
        },
        m_value);
}

std::optional<Attribute> Attribute::parse(Parser& parser, std::optional<NetCDFType> attribute_type)
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
        parser.log_parse_error(
            fmt::format("Could not find '=' for Attribute with parsed name {}", name->content()));
        return {};
    }

    auto split_str = split_string_at(name->content(), ':');
    if (split_str.second.empty())
    {
        parser.log_parse_error(fmt::format("Splitting attr name failed for Attribute {}", name->content()));
        return {};
    }
    // Global attribute
    bool is_global{split_str.first.empty()};

    Attribute attr{};
    attr.m_variable_name = split_str.first;
    attr.m_attribute_name = split_str.second;
    attr.m_type = attribute_type;

    // Try to find the variable this attribute corresponds to
    auto variable = parser.resolve_variable_for_name(*attr.m_variable_name);
    if (!variable && !is_global)
    {
        parser.log_parse_error(
            fmt::format("Could not find variable '{}' when parsing non-global attribute '{}'",
                        *attr.m_variable_name, attr.m_attribute_name));
        return {};
    }

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
    else if (attr.m_attribute_name == "_ChunkSizes" || attr.m_attribute_name == "_Storage" ||
             attr.m_attribute_name == "_Fletcher32" || attr.m_attribute_name == "_DeflateLevel" ||
             attr.m_attribute_name == "_Endianness" || attr.m_attribute_name == "_NoFill" ||
             attr.m_attribute_name == "_IsNetcdf4")
    {
        auto value = parser.pop();
        if (!value || value->content().empty())
        {
            return {};
        }
        // TODO: some of these are not just strings, like the chunksizes
        attr.m_type = NetCDFElementaryType::String;
        attr.m_value = std::string(value->content());
    }
    else if (attr.m_attribute_name == "_FillValue")
    {
        // Get the fill value type from the variable
        if (!variable)
        {
            parser.log_parse_error("No matching variable found when parsing attribute '_FillValue'");
            return {};
        }
        attr.m_type = variable->basic_type();
        auto fill_value = VariableData::parse(parser, *attr.m_type);
        if (!fill_value)
        {
            parser.log_parse_error("Could not parse value for attribute '_FillValue'");
            return {};
        }
        attr.m_value = *fill_value;
    }
    else if (attr.m_attribute_name == "_Shuffle")
    {
        // TODO: Shuffle attribute is always Bool, but only parsed as string
        auto value = parser.pop();
        if (!value || value->content().empty())
        {
            parser.log_parse_error("Could not parse value for attribute '_Shuffle'");
            return {};
        }
        attr.m_value = std::string(value->content());
    }
    else if (attr.m_attribute_name == "valid_range")
    {
        // Get the valid range value type from the variable
        if (!variable)
        {
            parser.log_parse_error("No matching variable found when parsing attribute 'valid_range'");
            return {};
        }
        attr.m_type = variable->basic_type();
        auto start = parser.parse_number(*attr.m_type);
        auto comma = parser.pop_specific({","});
        auto end = parser.parse_number(*attr.m_type);
        if (!start || !comma || !end)
        {
            parser.log_parse_error("Could not parse value for attribute 'valid_range'");
            return {};
        }
        attr.m_value = ValidRangeValue{*start, *end};
    }
    else if (is_global)
    {
        // untyped global attributes
        if (!attr.m_type.has_value())
        {
            // Note: maybe untyped global attributes should not have a type
            // It just seems that the content is 'typically' free string,
            // so we make the type string as well
            attr.m_type = NetCDFElementaryType::String;
        }

        // typed global attributes
        {
            auto& type = attr.m_type.value();
            auto data = VariableData::parse(parser, type);
            if (!data)
            {
                parser.log_parse_error(fmt::format("Parsing global attribute {} with type {} failed.",
                                                   split_str.second, type.name()));
                return {};
            }
            attr.m_value = *data;
        }
    }
    else
    {
        // TODO: For now, just parse everything thought to be attribute
        auto value = parser.pop();
        if (!value || value->content().empty())
        {
            return {};
        }
        // parser.log_parse_error(fmt::format("Unsupported attribute '{}'\n", attr.m_attribute_name));
        attr.m_value = std::string(value->content());
    }

    auto line_end = parser.pop_until_specific({";"});

    if (!line_end)
    {
        parser.log_parse_error(fmt::format(
            "Could not find line end for variable definition for attribute {}\n", attr.m_attribute_name));
        return {};
    }
    return attr;
}

std::string VariableData::as_string() const
{
    return std::visit([](auto&& arg) { return arg.as_string(); }, data);
}

std::optional<VariableData> VariableData::parse(Parser& parser, const NetCDFType& type)
{
    return std::visit(
        [&parser](auto&& arg) -> std::optional<VariableData>
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, NetCDFElementaryType>)
            {
                switch (arg)
                {
                case NetCDFElementaryType::String:
                case NetCDFElementaryType::Char:
                    return parser.parse_string(arg);
                default:
                    return parser.parse_number(arg);
                }
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
VariableDeclaration::parse(Parser& parser, std::optional<NetCDFType> existing_type)
{
    auto next_token = parser.peek();
    if (!next_token || is_group_end(next_token->content()))
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
        parser.log_parse_error("Did not find name for variable\n");
        return {};
    }

    // If the name has ':', it is attribute
    if (!name->content().empty() && name->content().find(':') != std::string::npos)
    {
        return Attribute::parse(parser, type);
    }

    return Variable::parse(parser, *type);
}

std::optional<EnumValue> EnumValue::parse(Parser& parser)
{
    auto next_token = parser.peek();
    if (!next_token || is_group_end(next_token->content()))
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

std::optional<ComplexType> ComplexType::parse(Parser& parser)
{

    auto next_token = parser.peek();
    if (!next_token || is_group_end(next_token->content()))
    {
        return {};
    }

    auto type_name = parser.pop();
    if (!type_name)
    {
        parser.log_parse_error("Did not find type name when parsing type");
        return {};
    }

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

        return ComplexType{
            OpaqueType(opaque_name->content(), std::stoi(std::string(opaque_size->content())))};
    }

    // compound cmpd_t { vlen_t f1; enum_t f2;};
    if (type_name->content() == "compound")
    {
        if (auto type = CompoundType::parse(parser))
        {
            return ComplexType(*type);
        }
        parser.log_parse_error("Failed to parse CompoundType");
        return {};
    }

    auto actual_type = type_for_token(*type_name);
    if (!actual_type)
        return {};

    if (parser.peek_specific({"enum"}))
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

void Types::parse(Parser& parser, std::vector<ComplexType>& types)
{
    // types:
    //     ubyte enum enum_t {Clear = 0, Cumulonimbus = 1, Stratus = 2};
    //     opaque(11) opaque_t;
    //     int(*) vlen_t;
    while (auto type = ComplexType::parse(parser))
    {
        parser.skip_extra_tokens();
        types.push_back(std::move(*type));
    }
}

void VariableSection::parse(Parser& parser, Group& group)
{
    while (auto next_token = parser.peek())
    {
        if (!next_token || is_group_end(next_token->content()))
        {
            break;
        }
        parser.skip_extra_tokens();

        auto name = parser.pop();
        auto* variable = parser.resolve_variable_for_name(name->content());
        if (!variable)
        {
            parser.log_parse_error(fmt::format("Could not resolve variable name {} in group {}\n",
                                               name->content(), group.name()));
            return;
        }
        auto equals = parser.pop_specific({"="});
        if (!equals)
        {
            parser.log_parse_error(
                fmt::format("Could not find equals for variable data for variable {}\n", name->content()));
            return;
        }
        auto data = parser.parse_data(variable->type());
        if (!data)
        {
            return;
        }
        auto line_end = parser.pop_specific({";"});
        if (!line_end)
        {
            parser.log_parse_error(
                fmt::format("Could not find line end for variable data for variable {}\n", name->content()));
            return;
        }
    }
}

std::string Group::description(int indent) const
{
    Description description(indent, false);
    description << "Group " + m_name + "\n";
    if (!m_types.empty())
    {
        description.push_indent();
        description.push_indent();
        description << "Types:\n";
        description.push_indent();
        for (auto& type : m_types)
        {
            description << fmt::format("{}\n", type.as_string());
        }
        description.pop_indent();
        description.pop_indent();
        description.pop_indent();
    }
    if (m_dimensions)
    {
        description << m_dimensions->description(indent + 1);
    }
    if (m_variables)
    {
        description << m_variables->description(indent + 1);
    }
    for (auto& group : m_groups)
    {
        description << group.description(indent + 1);
    }
    return description.description;
}

std::optional<Group> Group::parse(Parser& parser)
{

    auto group_name = parser.pop_identifier();
    if (!group_name)
    {
        parser.log_parse_error("Could not find group name when parsing group");
        return {};
    }
    auto left_bracket = parser.pop_specific({"{"});
    if (!left_bracket)
    {
        parser.log_parse_error(
            fmt::format("Could not find group starting brace when parsing group {}", group_name->content()));
        return {};
    }
    Group group{};
    group.m_name = group_name->content();
    parser.push_group_stack(group);

    while (auto content = parser.peek())
    {
        if (content->content() == "dimensions:")
        {
            parser.pop();
            group.m_dimensions = Dimensions::parse(parser);
        }
        else if (content->content() == "types:")
        {
            parser.pop();
            Types::parse(parser, group.m_types);
        }
        else if (content->content() == "data:")
        {
            parser.pop();
            VariableSection::parse(parser, group);
        }
        else if (content->content() == "variables:")
        {
            parser.pop();
            if (!group.m_variables)
            {
                group.m_variables = Variables{};
            }
            Variables::parse(parser, group);
        }
        else if (content->content() == "group:")
        {
            parser.pop();
            if (auto child_group = Group::parse(parser))
            {
                group.m_groups.push_back(std::move(*child_group));
            }
        }
        else if (content->content() == "}")
        {
            parser.pop();
            parser.pop_group_stack();
            return group;
        }
        // try to parse things as attributes
        else
        {
            if (!group.m_variables)
            {
                group.m_variables = Variables{};
            }
            Variables::parse(parser, group);
        }

        // Prepare a clean slate of tokens for the next iteration
        parser.skip_extra_tokens();
    }
    parser.pop_group_stack();
    return {};
}

std::optional<RootGroup> RootGroup::parse(Parser& parser)
{
    auto netcdf = parser.pop();

    if (!netcdf || netcdf->content() != "netcdf")
    {
        return {};
    }

    RootGroup root{};
    auto group = Group::parse(parser);
    if (!group)
    {
        return {};
    }

    root.group = std::make_unique<Group>(*group);

    return root;
}

const std::vector<Variable>& Group::variables() const
{
    static std::vector<Variable> variables{};
    if (!m_variables)
    {
        return variables;
    }
    return m_variables->variables();
}

std::vector<Variable>& Group::variables()
{
    static std::vector<Variable> variables{};
    if (!m_variables)
    {
        return variables;
    }
    return m_variables->variables();
}

const std::vector<Attribute>& Group::attributes() const
{
    static std::vector<Attribute> attributes{};
    if (!m_variables)
    {
        return attributes;
    }
    return m_variables->attributes();
}

std::vector<Attribute>& Group::attributes()
{
    static std::vector<Attribute> attributes{};
    if (!m_variables)
    {
        return attributes;
    }
    return m_variables->attributes();
}

const std::vector<Dimension>& Group::dimensions() const
{
    static std::vector<Dimension> dimensions{};
    if (!m_dimensions)
    {
        return dimensions;
    }
    return m_dimensions->dimensions;
}

std::vector<Dimension>& Group::dimensions()
{
    static std::vector<Dimension> dimensions{};
    if (!m_dimensions)
    {
        return dimensions;
    }
    return m_dimensions->dimensions;
}

} // namespace ncdlgen
