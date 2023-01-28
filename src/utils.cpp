

#include "utils.h"

namespace ncdlgen
{

std::pair<std::string, std::string> split_string_at(std::string_view input, const char split_char)
{

    auto split_location = input.find(split_char);
    if (split_location == std::string::npos || split_location == input.size() - 1)
    {
        return {};
    }

    auto first_part = input.substr(0, split_location);
    auto second_part = input.substr(split_location + 1, input.size() - split_location - 1);
    return {std::string(first_part), std::string(second_part)};
}

std::vector<std::string_view> split_string(const std::string_view view, const char character)
{

    std::vector<std::string_view> split_components{};
    auto current{view};
    while (!current.empty())
    {
        auto first_split = current.find_first_of(character);
        if (first_split == std::string::npos)
        {
            split_components.push_back(current);
            return split_components;
        }
        auto split_comp{current.substr(0, first_split)};
        current = current.substr(first_split + 1, current.size());
        if (split_comp.empty())
        {
            continue;
        }
        split_components.push_back(split_comp);
    }
    return split_components;
}

} // namespace ncdlgen
