
#include <iostream>

#include "tokeniser.h"

namespace ncdlgen
{

std::vector<Token> Tokeniser::tokenise()
{

    while (auto character = peek())
    {
        // std::cout << "Got characeter: " << *character << "\n";

        switch (*character)
        {
        case ' ':
        case '\t':
        case '\n':
            stop_word();
            continue;

        case '{':
        case '}':
        case '(':
        case ')':
        case '*':
        case ';':
        case ',':
            stop_word_leave_char();
            m_tokens.push_back({m_input.substr(m_cursor, 1)});
            pop();
            m_word_start = m_cursor;
            continue;

        case '"':
            stop_word_leave_char();
            pop();
            pick_string();
            continue;

        case '/':
            // this is comment
            if (double_peek() && *double_peek() == '/')
            {
                stop_word();
                pick_comment();
                continue;
            }
            // otherwise, this is part of group name and will be kept as single
            // string

            // This is not separated this from the previous word
            // case ':':

        default:
            pop();
            continue;
        }
    }

    return m_tokens;
}

void Tokeniser::discard_word() { m_word_start = m_cursor; }

void Tokeniser::pick_comment()
{
    while (auto cur_char = pop())
    {
        switch (*cur_char)
        {
        case '\n':
            discard_word();
            return;

        default:
            continue;
        }
    }
}

void Tokeniser::pick_string()
{
    while (auto cur_char = pop())
    {
        switch (*cur_char)
        {
        case '"':
            stop_word_leave_char();
            return;

        default:
            continue;
        }
    }
}

void Tokeniser::stop_word()
{
    if (m_cursor > m_word_start)
    {
        m_tokens.push_back(
            {m_input.substr(m_word_start, m_cursor - m_word_start)});
    }
    pop();
    m_word_start = m_cursor;
}

void Tokeniser::stop_word_leave_char()
{
    if (m_cursor > m_word_start)
    {
        m_tokens.push_back(
            {m_input.substr(m_word_start, m_cursor - m_word_start)});
    }
    m_word_start = m_cursor;
}

std::optional<char> Tokeniser::double_peek() const
{
    if (m_cursor + 1 >= m_input.size())
    {
        return {};
    }
    return m_input[m_cursor + 1];
}

std::optional<char> Tokeniser::peek() const
{
    if (m_cursor >= m_input.size())
    {
        return {};
    }
    return m_input[m_cursor];
}

std::optional<char> Tokeniser::pop()
{
    if (m_cursor >= m_input.size())
    {
        return {};
    }
    return m_input[m_cursor++];
}

} // namespace ncdlgen
