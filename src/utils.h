
#pragma once

#include <string>

std::pair<std::string, std::string> split_string_at(std::string_view input,
                                                    const char split_char)
{

    auto split_location = input.find(split_char);
    if (split_location == std::string::npos ||
        split_location == input.size() - 1)
    {
        return {};
    }

    auto first_part = input.substr(0, split_location);
    auto second_part =
        input.substr(split_location + 1, input.size() - split_location - 1);
    return {std::string(first_part), std::string(second_part)};
}
