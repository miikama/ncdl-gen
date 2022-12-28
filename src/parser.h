
#pragma once

#include <list>
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

    std::optional<const Token> peek_specific(const std::vector<std::string> &possible_tokens);
    std::optional<const Token> pop_specific(const std::vector<std::string> &possible_tokens);
    std::optional<NetCDFType> peek_type();

    std::optional<Number> parse_number(const NetCDFType&);
    std::optional<Array> parse_array(const NetCDFElementaryType& type);
    std::optional<Array> parse_complex_type_data(const ComplexType&);

    void push_group_stack(Group &group) { group_stack.push_back(&group); }
    void pop_group_stack() { group_stack.pop_back(); }

    Variable* resolve_variable_for_name(const std::string_view var_name);

  private:
    /**
     * During parsing, resolve typenames to types. Typically
     * these are the basic NetCDF types, but for user defined
     * types, resolve typename to the actual type
     */
    std::optional<NetCDFType> resolve_type_for_name(const std::string_view type_name);

    size_t m_cursor{};
    const std::vector<Token> &m_tokens;

    // stack of groups for parsing
    std::list<Group *> group_stack{};
};

} // namespace ncdlgen
