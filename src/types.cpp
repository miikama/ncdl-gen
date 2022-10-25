
#include <iostream>

#include "types.h"
#include "parser.h"

namespace ncdlgen {

std::string Types::description() const { return "Types"; }

std::string Group::description() const {
    std::string description = "  Group " + m_name;
    if (m_types) {
        description.append("\n");
        description.append(m_types->description());
    }
    return description;
}

std::string RootGroup::description() const {
    std::string description = "RootGroup " + m_name;
    if (m_group) {
        description.append("\n");
        description.append(m_group->description());
    }
    return description;
}

void RootGroup::print_tree(){

    std::cout << description() << "\n";
}

std::optional<Types> Types::parse(Parser &parser) {
    return {};
}

std::optional<Group> Group::parse(Parser &parser) {


    auto group_name = parser.pop();
    auto left_bracket = parser.pop();
    if(!group_name || !left_bracket)
    {
        return {};
    }
    m_name = group_name->content();

    while(auto content = parser.pop())
    {
        if (content->content() == "}")
        {
            return *this;
        }
    }
    return {};
}

std::optional<RootGroup> RootGroup::parse(Parser &parser) {
    auto netcdf = parser.pop();

    if (!netcdf || netcdf->content() != "netcdf") {
        return {};
    }

    m_group = Group();
    m_group = m_group->parse(parser);

    return *this;
}

} // namespace ncdlgen
