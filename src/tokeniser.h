
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ncdlgen
{

struct SourceLocation
{
    size_t line{};
    size_t column{};
};

struct Token
{
    const std::string_view& content() const { return m_content; }
    std::string_view m_content{};
    SourceLocation source_location{};
};

class Tokeniser
{

  public:
    Tokeniser(const std::string& input) : m_input(input) {}

    std::vector<Token> tokenise();

    std::optional<char> double_peek() const;
    std::optional<char> peek() const;
    std::optional<char> pop();

    static void print_tokens(const std::vector<Token>& tokens);

  private:
    void pick_comment();
    void pick_string();
    void discard_word();
    void stop_word();
    void stop_word_leave_char();

    std::string_view m_input{};

    std::vector<Token> m_tokens{};

    size_t m_cursor{};
    size_t m_word_start{};

    SourceLocation source_location() { return {m_line, m_column}; }

    size_t m_line{};
    size_t m_column{};
};

} // namespace ncdlgen
