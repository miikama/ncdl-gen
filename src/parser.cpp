
#include <iostream>

#include "parser.h"

namespace ncdlgen {

std::vector<Token> Parser::tokenize() {

    while (auto character = peek()) {
        // std::cout << "Got characeter: " << *character << "\n";

        switch (*character) {
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
            stop_word_pick_char();
            continue;

        case '/':
            stop_word();
            // comment
            if (peek() && *peek() == '/') {
                pick_comment();
                continue;
            }

        // should we separate this from the previous word?
        // case ':':


        default:
            pop();
            continue;
        }
    }

    return m_tokens;
}

void Parser::discard_word() { m_word_start = m_cursor; }

void Parser::pick_comment()
{
    while (auto cur_char = pop()) {
        switch (*cur_char)
        {
        case '\n':
            discard_word();
            stop_word();
            return;
        
        default:
            continue;
        }
    }
}

void Parser::stop_word() {
    if (m_cursor > m_word_start) {
        m_tokens.push_back(
            {m_input.substr(m_word_start, m_cursor - m_word_start)});
    }
    pop();
    m_word_start = m_cursor;
}

void Parser::stop_word_pick_char() {
    if (m_cursor > m_word_start) {
        m_tokens.push_back(
            {m_input.substr(m_word_start, m_cursor - m_word_start)});
    }
    m_tokens.push_back({m_input.substr(m_cursor, 1)});
    pop();
    m_word_start = m_cursor;
}

std::optional<char> Parser::peek() const {
    if (m_cursor >= m_input.size()) {
        return {};
    }
    return m_input[m_cursor];
}

std::optional<char> Parser::pop() {
    if (m_cursor >= m_input.size()) {
        return {};
    }
    return m_input[m_cursor++];
}

} // namespace ncdlgen
