
#include <iostream>

#include "parser.h"
#include "types.h"
#include "unordered_set"

namespace ncdlgen {

struct Description{

    Description(const int indent, bool add_new_line = true) : indent(indent), new_line(add_new_line) {}

    void operator<<(const std::string& input) { 
        std::string line {};
        for (size_t i = 0; i < indent * 2; i++)
        {
            line += " ";
        }
        line += input;
        if ( new_line)
        {
            line += "\n";
        } 
        description += line;        
    }

    void push_indent() {  indent++; }

    std::string description {};
    size_t indent {};
    bool new_line {};
};

bool is_keyword(const std::string_view word) {
    static std::unordered_set<std::string_view> keywords{};
    keywords.insert("variables:");
    keywords.insert("dimensions:");
    keywords.insert("types:");
    keywords.insert("data:");
    keywords.insert("group:");
    return keywords.find(word) != keywords.end();
}

std::string Types::description(int indent) const { return "Types"; }

std::string Group::description(int indent) const {
    Description description(indent);
    description << "Group " + m_name;
    if (m_types) {
        description << m_types->description(indent + 1);
    }
    if (m_dimensions) {
        description << m_dimensions->description(indent + 1);
    }
    return description.description;
}

std::string Dimensions::description(int indent) const {
    Description description(indent);
    description << "Dimensions";
    for (auto &dimension : m_dimensions) {
        description << dimension.description(indent + 1);
    }
    return description.description;
}

std::string Dimension::description(int indent) const {
    Description description(indent, false);
    if (m_length == 0) {
        description << m_name + " = unlimited";
    } else {
        description << m_name + " = " + std::to_string(m_length);
    }
    return description.description;
}

std::string RootGroup::description(int indent) const {
    Description description(indent);
    description  << "RootGroup " + m_name;
    if (m_group) {
        description << m_group->description(indent + 1);
    }
    return description.description;
}

void RootGroup::print_tree() { std::cout << description(0) << "\n"; }

std::optional<Dimension> Dimension::parse(Parser &parser) {

    auto next_token = parser.peek();
    if (!next_token || is_keyword(next_token->content())) {
        return {};
    }
    auto name = parser.pop();
    auto equals = parser.pop();
    auto value = parser.pop();
    auto line_end_or_comma = parser.pop();
    if (!name || !equals || !value || !line_end_or_comma) {
        return {};
    }
    bool is_line_or_comma = line_end_or_comma->content() == "," ||
                            line_end_or_comma->content() == ";";
    if (equals->content() != "=" || !is_line_or_comma) {
        return {};
    }

    Dimension dim{};
    dim.m_name = name->content();
    if (value->content() == "UNLIMITED" || value->content() == "unlimited") {
        dim.m_length = 0;
    } else {
        dim.m_length = std::stoi(std::string(value->content()));
    }
    return dim;
}

std::optional<Dimensions> Dimensions::parse(Parser &parser) {

    Dimensions dimensions{};
    dimensions.m_name = "dimensions:";
    while (auto dimension = Dimension::parse(parser)) {
        dimensions.m_dimensions.push_back(*dimension);
    }
    return dimensions;
}

std::optional<Types> Types::parse(Parser &parser) { return {}; }

std::optional<Group> Group::parse(Parser &parser) {

    auto group_name = parser.pop();
    auto left_bracket = parser.pop();
    if (!group_name || !left_bracket) {
        return {};
    }
    Group group{};
    group.m_name = group_name->content();

    while (auto content = parser.pop()) {
        if (content->content() == "dimensions:") {
            group.m_dimensions = Dimensions::parse(parser);
        } else if (content->content() == "}") {
            return group;
        }
    }
    return {};
}

std::optional<RootGroup> RootGroup::parse(Parser &parser) {
    auto netcdf = parser.pop();

    if (!netcdf || netcdf->content() != "netcdf") {
        return {};
    }

    RootGroup root{};
    root.m_group = Group::parse(parser);

    return root;
}

} // namespace ncdlgen
