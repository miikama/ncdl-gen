

#include "parser.h"

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

} // namespace ncdlgen