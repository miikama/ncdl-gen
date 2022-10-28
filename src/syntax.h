
#pragma once

#include <string_view>

#include "types.h"

namespace ncdlgen
{

bool is_keyword(const std::string_view word);

std::optional<NetCDFType> type_for_token(const Token &token);

std::string name_for_type(const NetCDFType &type);

} // namespace ncdlgen