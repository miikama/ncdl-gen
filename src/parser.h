
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "tokeniser.h"
#include "types.h"

namespace ncdlgen
{

class Parser
{

  public:
    Parser(const std::vector<Token> &tokens) : m_tokens(tokens) {}
    std::optional<RootGroup> parse();

    std::optional<const Token> pop();
    std::optional<const Token> peek();

    std::optional<const Token>
    peek_specific(const std::vector<std::string> &possible_tokens);
    std::optional<const Token>
    pop_specific(const std::vector<std::string> &possible_tokens);
    std::optional<NetCDFType> peek_type();
    std::optional<const Token> pop_type();

    std::optional<Number> parse_number(NetCDFType);

  private:
    std::optional<float> parse_float();

    size_t m_cursor{};
    const std::vector<Token> &m_tokens;
};

} // namespace ncdlgen
