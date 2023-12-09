
#include <fmt/core.h>

#include "parser.h"
#include "syntax.h"
#include "utils.h"

namespace ncdlgen
{

std::optional<RootGroup> Parser::parse()
{

    RootGroup root{};

    return root.parse(*this);
}

std::optional<const Token> Parser::pop()
{
    if (m_cursor >= m_tokens.size())
    {
        return {};
    }
    return m_tokens[m_cursor++];
}

std::optional<const Token> Parser::peek()
{
    if (m_cursor >= m_tokens.size())
    {
        return {};
    }
    return m_tokens[m_cursor];
}

static std::vector<std::string> forbidden_identifiers{
    "{", "}", "(", ")", "*", ":", ";", ",",
};

std::optional<const Token> Parser::pop_identifier()
{
    if (peek_specific(forbidden_identifiers))
    {
        pop();
        return {};
    }
    return pop();
}

std::optional<const Token> Parser::peek_specific(const std::vector<std::string>& possible_tokens)
{
    auto token = peek();
    if (!token)
    {
        return {};
    }
    for (auto& possible : possible_tokens)
    {
        if (token->content() == possible)
        {
            return token;
        }
    }
    return {};
}

std::optional<const Token> Parser::pop_specific(const std::vector<std::string>& possible_tokens)
{
    auto token = pop();
    if (!token)
    {
        return {};
    }
    for (auto& possible : possible_tokens)
    {
        if (token->content() == possible)
        {
            return token;
        }
    }
    return {};
}

std::optional<NetCDFType> Parser::peek_type()
{
    auto token = peek();
    if (!token)
    {
        return {};
    }

    return resolve_type_for_name(token->content());
}

SourceLocation Parser::current_cursor_location() const
{
    if (m_cursor >= m_tokens.size())
    {
        return {0, 0};
    }
    return m_tokens[m_cursor].source_location;
}

void Parser::log_parse_error(const std::string& message)
{
    auto cursor_location = current_cursor_location();
    fmt::print("Parser Error at line {} and column {}:\n    {}\n", cursor_location.line + 1,
               cursor_location.column + 1, message);
}

Group* Parser::resolve_group_from_path(const std::string_view group_path)
{
    if (group_path.empty())
    {
        return nullptr;
    }

    auto path_components{split_string(group_path, '/')};

    Group* group{group_stack.front()};

    // The first group is root group. We do not support paths
    // that reference the root group by name, the paths should
    // start one level lower
    for (std::size_t i = 0; i < path_components.size(); i++)
    {
        auto component{path_components[i]};
        bool found_group{false};
        for (auto& possible_group : group->groups())
        {
            if (possible_group.name() == path_components[i])
            {
                group = &possible_group;
                found_group = true;
                break;
            }
        }

        if (!found_group)
        {
            log_parse_error(
                fmt::format("Could not associate group path component '{}' to any group", component));
            return nullptr;
        }
    }

    return group;
}

std::optional<NetCDFType> Parser::resolve_type_for_name(const std::string_view type_name)
{
    // Basic type
    auto type = type_for_token({type_name});
    if (type != NetCDFElementaryType::Default)
    {
        return *type;
    }

    // the types can be defined with absolute paths with group names
    // e.g. /g/type_name
    if (type_name.find("/") != std::string::npos)
    {
        auto path{type_name.substr(0, type_name.find_last_of("/") + 1)};
        auto* group = resolve_group_from_path(path);
        if (!group)
        {
            log_parse_error(fmt::format("Could not resolve group of group type {}", type_name));
            return {};
        }

        auto type_name_without_path{type_name.substr(type_name.find_last_of("/") + 1, type_name.size())};
        for (auto& type : group->types())
        {
            if (type.name() == type_name_without_path)
            {
                return type;
            }
        }
        return {};
    }

    // Possibly complex user defined type
    for (auto it = group_stack.rbegin(); it != group_stack.rend(); ++it)
    {
        const Group* group = *it;
        for (auto& type : group->types())
        {
            if (type.name() == type_name)
            {
                return type;
            }
        }
    }

    return {};
}

Variable* Parser::resolve_variable_for_name(const std::string_view var_name)
{
    if (group_stack.empty())
    {
        return nullptr;
    }

    // We only search for variables in the current group
    auto& current_group = *group_stack.back();

    for (auto& variable : current_group.variables())
    {
        if (variable.name() == var_name)
        {
            return &variable;
        }
    }
    return nullptr;
}

std::optional<Number> Parser::parse_number(const NetCDFType& type)
{
    auto number_token = pop();
    if (!number_token)
    {
        return {};
    }
    auto number_string = std::string(number_token->content());

    if (!std::holds_alternative<NetCDFElementaryType>(type.type))
    {
        fmt::print("Parsing number for user defined complex type '{}' is not supported\n", type.name());
        return {};
    }
    auto& basic_type = std::get<NetCDFElementaryType>(type.type);

    /**
     * Here we delegate the conversions to std::stoT. This is
     * not the best for error cases, because e.g. when parsing
     * a float the following inputs all result in same output
     * WITHOUT errors
     *
     *  50
     *  50.
     *  50sssss24124
     *  50--1231231
     *  50+12321sss
     *
     *  -> result in 50 without error
     */
    try
    {
        switch (basic_type)
        {
        case NetCDFElementaryType::Char:
        {
            return Number(std::stoi(number_string), NetCDFElementaryType::Char);
        }
        case NetCDFElementaryType::Byte:
        {
            return Number(std::stoi(number_string), NetCDFElementaryType::Byte);
        }
        case NetCDFElementaryType::Ubyte:
        {
            return Number(std::stoul(number_string), NetCDFElementaryType::Ubyte);
        }
        case NetCDFElementaryType::Short:
        {
            return Number(std::stoi(number_string), NetCDFElementaryType::Short);
        }
        case NetCDFElementaryType::Ushort:
        {
            return Number(std::stoul(number_string), NetCDFElementaryType::Ushort);
        }
        case NetCDFElementaryType::Int:
        {
            return Number(std::stoi(number_string), NetCDFElementaryType::Int);
        }
        case NetCDFElementaryType::Uint:
        {
            return Number(std::stoul(number_string), NetCDFElementaryType::Uint);
        }
        // Same as Int
        case NetCDFElementaryType::Long:
        {
            return Number(std::stoi(number_string), NetCDFElementaryType::Long);
        }
        case NetCDFElementaryType::Int64:
        {
            return Number(std::stol(number_string), NetCDFElementaryType::Float);
        }
        case NetCDFElementaryType::Uint64:
        {
            return Number(std::stoul(number_string), NetCDFElementaryType::Float);
        }
        case NetCDFElementaryType::Float:
        {
            return Number(std::stof(number_string), NetCDFElementaryType::Float);
        }
        case NetCDFElementaryType::Double:
        {
            return Number(std::stof(number_string), NetCDFElementaryType::Double);
        }
        // same as float
        case NetCDFElementaryType::Real:
        {
            return Number(std::stof(number_string), NetCDFElementaryType::Real);
        }

        default:
            fmt::print("Parsing number of NetCDF type '{}' is not supported\n", name_for_type(basic_type));
            return {};
        }
    }
    catch (std::invalid_argument)
    {
        fmt::print("Could not parse string '{}' as NetCDF type '{}'.\n", number_string,
                   name_for_type(basic_type));
        return {};
    }
}

std::optional<Array> Parser::parse_data(const NetCDFType& type)
{

    return std::visit(
        [&](auto&& arg) -> std::optional<Array> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, NetCDFElementaryType>)
            {
                return parse_array(arg);
            }
            else if constexpr (std::is_same_v<T, ComplexType>)
            {
                return parse_complex_type_data(arg);
            }
            else
            {

                static_assert(always_false_v<T>, "Visiting unsupported type.");
            }
        },
        type.type);
}

std::optional<Array> Parser::parse_array(const NetCDFElementaryType& type)
{
    // Parsing 17, 18, 19
    Array array{};
    while (auto entry = parse_number(type))
    {
        array.data.push_back(*entry);
        if (auto found_comma = peek_specific({","}))
        {
            pop();
        }
        else
        {
            break;
        }
    }
    return array;
}

std::optional<Array> Parser::parse_complex_type_data(const ComplexType& type)
{
    return std::visit(
        [this](auto&& arg) -> std::optional<Array> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, OpaqueType>)
            {
                fmt::print("TODO: parsing OpaqueType unsupported!\n");
                return {};
            }
            else if constexpr (std::is_same_v<T, EnumType>)
            {
                auto enum_token = pop();
                if (!enum_token || enum_token->content().empty())
                {
                    log_parse_error("No token for parsing EnumType data.");
                    return {};
                }
                // e.g. enum_t.Stratus
                auto enum_string{enum_token->content()};
                if (enum_string.find('.') != std::string::npos)
                {
                    auto split = split_string(enum_string, '.');
                    if (split.size() != 2)
                    {
                        log_parse_error("Incorrect number of '.' in EnumType data entry.");
                        return {};
                    }
                    auto enum_type = resolve_type_for_name(split[0]);
                    if (!enum_type)
                    {
                        log_parse_error(fmt::format(
                            "Could not find type '' when parsing EnumType data entry.", split[0]));
                        return {};
                    }
                    // TODO: validate the enum variable entry name ('Stratus' part)
                }
                fmt::print("Succesfully parsed enum type but not returning data. Now at token {}\n",
                           peek()->content());
                // TODO: return actual EnumType data
                return Array();
            }
            else if constexpr (std::is_same_v<T, VLenType>)
            {
                // Parsing {17, 18, 19}
                auto start_bracket = pop_specific({"{"});
                if (!start_bracket)
                {
                    fmt::print("Did not find start bracket when parsing type {}\n", arg.name);
                    return {};
                }
                Array array{};
                while (auto entry = parse_number(arg.type))
                {
                    array.data.push_back(*entry);
                    if (auto found_comma = peek_specific({","}))
                    {
                        pop();
                    }
                    else
                    {
                        break;
                    }
                }
                auto end_bracket = pop_specific({"}"});
                if (!end_bracket)
                {
                    log_parse_error(fmt::format("Did not find end bracket when parsing type {}", arg.name));
                    return {};
                }
                return array;
            }
            else if constexpr (std::is_same_v<T, ArrayType>)
            {
                return parse_array(arg.type);
            }
            else if constexpr (std::is_same_v<T, CompoundType>)
            {
                auto start_bracket = pop_specific({"{"});
                if (!start_bracket)
                {
                    log_parse_error(fmt::format(
                        "Could not find start bracket for CompoundType '{}' definition", arg.name));
                    return {};
                }
                for (auto& contained_type : arg.types)
                {
                    auto contained_data = parse_complex_type_data(ComplexType(contained_type));
                    if (!contained_data)
                    {
                        log_parse_error(fmt::format("Could not parse variable {} contents for type {}",
                                                    arg.name, arg.name));
                        return {};
                    }
                    auto comma_or_end_bracket = pop_specific({",", "}"});
                    if (!comma_or_end_bracket)
                    {
                        log_parse_error(fmt::format("Could not find ',' or '}' to separate or end "
                                                    "compound variable '{}' data section.",
                                                    arg.name));
                    }
                    if (comma_or_end_bracket->content() == "}")
                    {
                        break;
                    }
                }
                fmt::print("Succesfully parsed compound type but not returning data. Now at token {}\n",
                           peek()->content());
                // TODO: return actual CompoundType data
                return Array();
            }
            else
            {
                static_assert(always_false_v<T>, "Visiting unsupported type!");
            }
        },
        type.type);
}

void Parser::skip_extra_tokens()
{
    while (auto next_token = peek())
    {
        if (next_token->content() == ";")
        {
            pop();
        }
        else
        {
            return;
        }
    }
}

} // namespace ncdlgen