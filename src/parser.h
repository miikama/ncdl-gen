
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ncdlgen {

struct Token {
    std::string_view content;
};

class Parser {

  public:
    Parser(const std::string &input) : m_input(input) {}

    std::vector<Token> tokenize();

    std::optional<char> double_peek() const;
    std::optional<char> peek() const;
    std::optional<char> pop();

  private:
    void pick_comment();
    void discard_word();
    void stop_word();
    void stop_word_pick_char();

    std::string_view m_input{};

    std::vector<Token> m_tokens{};

    size_t m_cursor{};
    size_t m_word_start{};
};

} // namespace ncdlgen
