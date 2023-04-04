
#pragma once

#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace ncdlgen
{

// helper constant for the static asserts in template
template <class> inline constexpr bool always_false_v = false;

std::pair<std::string, std::string> split_string_at(std::string_view input, const char split_char);

std::vector<std::string_view> split_string(const std::string_view view, const char character);

std::string read_file(std::string_view file_name);

} // namespace ncdlgen
