
#include <iostream>

#include "parser.h"
#include "types.h"
#include "unordered_set"

namespace ncdlgen {

bool is_keyword(const std::string_view word) {
    static std::unordered_set<std::string_view> keywords{};
    keywords.insert("variables:");
    keywords.insert("dimensions:");
    keywords.insert("types:");
    keywords.insert("data:");
    keywords.insert("group:");
    return keywords.find(word) != keywords.end();
}

std::string Types::description() const { return "Types"; }

std::string Group::description() const {
    std::string description = "  Group " + m_name;
    if (m_types) {
        description.append("\n");
        description.append(m_types->description());
    }
    if (m_dimensions) {
        description.append("\n");
        description.append(m_dimensions->description());
    }
    return description;
}

std::string Dimensions::description() const {
    std::string description = "  Dimensions";
    for (auto &dimension : m_dimensions) {
        description.append("\n");
        description.append(dimension.description());
    }
    description.append("\n");
    return description;
}

std::string Dimension::description() const {
    if (m_length == 0) {
        return "    " + m_name + " = unlimited";
    } else {
        return "    " + m_name + " = " + std::to_string(m_length);
    }
}

std::string RootGroup::description() const {
    std::string description = "RootGroup " + m_name;
    if (m_group) {
        description.append("\n");
        description.append(m_group->description());
    }
    return description;
}

void RootGroup::print_tree() { std::cout << description() << "\n"; }

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
