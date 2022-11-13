
#include <fmt/core.h>

#include "parser.h"
#include "syntax.h"

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

std::optional<const Token>
Parser::peek_specific(const std::vector<std::string> &possible_tokens)
{
    auto token = peek();
    if (!token)
    {
        return {};
    }
    for (auto &possible : possible_tokens)
    {
        if (token->content() == possible)
        {
            return token;
        }
    }
    return {};
}

std::optional<const Token>
Parser::pop_specific(const std::vector<std::string> &possible_tokens)
{
    auto token = pop();
    if (!token)
    {
        return {};
    }
    for (auto &possible : possible_tokens)
    {
        if (token->content() == possible)
        {
            return token;
        }
    }
    return {};
}

std::optional<NetCDFElementaryType> Parser::peek_type()
{
    auto token = peek();
    if (!token)
    {
        return {};
    }

    return resolve_type_for_name(token->content());
}

std::optional<NetCDFElementaryType>
Parser::resolve_type_for_name(const std::string_view type_name)
{
    // Basic type
    auto type = type_for_token({type_name});
    if (type != NetCDFElementaryType::Default)
    {
        return type;
    }

    return {};
}

std::optional<Number> Parser::parse_number(NetCDFElementaryType type)
{
    auto number_token = pop();
    if (!number_token)
    {
        return {};
    }
    auto number_string = std::string(number_token->content());

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
        switch (type)
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
            return Number(std::stoul(number_string),
                          NetCDFElementaryType::Ubyte);
        }
        case NetCDFElementaryType::Short:
        {
            return Number(std::stoi(number_string),
                          NetCDFElementaryType::Short);
        }
        case NetCDFElementaryType::Ushort:
        {
            return Number(std::stoul(number_string),
                          NetCDFElementaryType::Ushort);
        }
        case NetCDFElementaryType::Int:
        {
            return Number(std::stoi(number_string), NetCDFElementaryType::Int);
        }
        case NetCDFElementaryType::Uint:
        {
            return Number(std::stoul(number_string),
                          NetCDFElementaryType::Uint);
        }
        // Same as Int
        case NetCDFElementaryType::Long:
        {
            return Number(std::stoi(number_string), NetCDFElementaryType::Long);
        }
        case NetCDFElementaryType::Int64:
        {
            return Number(std::stol(number_string),
                          NetCDFElementaryType::Float);
        }
        case NetCDFElementaryType::Uint64:
        {
            return Number(std::stoul(number_string),
                          NetCDFElementaryType::Float);
        }
        case NetCDFElementaryType::Float:
        {
            return Number(std::stof(number_string),
                          NetCDFElementaryType::Float);
        }
        case NetCDFElementaryType::Double:
        {
            return Number(std::stof(number_string),
                          NetCDFElementaryType::Double);
        }
        // same as float
        case NetCDFElementaryType::Real:
        {
            return Number(std::stof(number_string), NetCDFElementaryType::Real);
        }

        default:
            fmt::print("Parsing number of NetCDF type '{}' is not supported\n",
                       name_for_type(type));
            return {};
        }
    }
    catch (std::invalid_argument)
    {
        fmt::print("Could not parse string '{}' as NetCDF type '{}'.\n",
                   number_string, name_for_type(type));
        return {};
    }
}

} // namespace ncdlgen