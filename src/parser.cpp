

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

std::optional<const Token> Parser::peek_specific(const std::vector<std::string> &possible_tokens)
{
    auto token = peek();
    if (!token)
    {
        return {};
    }
    for(auto& possible : possible_tokens)
    {
        if(token->content() == possible)
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
    for(auto& possible : possible_tokens)
    {
        if(token->content() == possible)
        {
            return token;
        }
    }
    return {};
}

std::optional<NetCDFType> Parser::peek_type()
{
    auto token = peek();
    if ( !token) {
        return {};
    }

    auto type = type_for_token(*token);
    if(type == NetCDFType::Default)
    {
        return {};
    }
    return type;
}

std::optional<const Token> Parser::pop_type()
{
    auto token = pop();
    if ( !token) {
        return {};
    }

    if(type_for_token(*token) == NetCDFType::Default)
    {
        return {};
    }
    return token;
}

} // namespace ncdlgen