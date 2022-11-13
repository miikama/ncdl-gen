
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

std::optional<NetCDFType> Parser::peek_type()
{
    auto token = peek();
    if (!token)
    {
        return {};
    }

    return resolve_type_for_name(token->content());
}

std::optional<NetCDFType>
Parser::resolve_type_for_name(const std::string_view type_name)
{
    // Basic type
    auto type = type_for_token({type_name});
    if (type != NetCDFType::Default)
    {
        return type;
    }

    return {};
}

std::optional<Number> Parser::parse_number(NetCDFType type)
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
        case NetCDFType::Char:
        {
            return Number(std::stoi(number_string), NetCDFType::Char);
        }
        case NetCDFType::Byte:
        {
            return Number(std::stoi(number_string), NetCDFType::Byte);
        }
        case NetCDFType::Ubyte:
        {
            return Number(std::stoul(number_string), NetCDFType::Ubyte);
        }
        case NetCDFType::Short:
        {
            return Number(std::stoi(number_string), NetCDFType::Short);
        }
        case NetCDFType::Ushort:
        {
            return Number(std::stoul(number_string), NetCDFType::Ushort);
        }
        case NetCDFType::Int:
        {
            return Number(std::stoi(number_string), NetCDFType::Int);
        }
        case NetCDFType::Uint:
        {
            return Number(std::stoul(number_string), NetCDFType::Uint);
        }
        // Same as Int
        case NetCDFType::Long:
        {
            return Number(std::stoi(number_string), NetCDFType::Long);
        }
        case NetCDFType::Int64:
        {
            return Number(std::stol(number_string), NetCDFType::Float);
        }
        case NetCDFType::Uint64:
        {
            return Number(std::stoul(number_string), NetCDFType::Float);
        }
        case NetCDFType::Float:
        {
            return Number(std::stof(number_string), NetCDFType::Float);
        }
        case NetCDFType::Double:
        {
            return Number(std::stof(number_string), NetCDFType::Double);
        }
        // same as float
        case NetCDFType::Real:
        {
            return Number(std::stof(number_string), NetCDFType::Real);
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