
#pragma once

#include <string_view>

#include "types.h"

namespace ncdlgen
{

bool is_keyword(const std::string_view word);

// any keyword stops variable declaration part
// } ends variable declaration part because it ends the group
bool is_group_end(const std::string_view word);

std::optional<NetCDFElementaryType> type_for_token(const Token& token);

const std::string_view name_for_type(const NetCDFElementaryType& type);
const std::string_view cpp_name_for_type(const NetCDFElementaryType& type);

// clang-format off
template <NetCDFElementaryType>
struct internal_type {};
template <> struct internal_type<NetCDFElementaryType::Char>      { using type = int8_t; };
template <> struct internal_type<NetCDFElementaryType::Byte>      { using type = int8_t; };
template <> struct internal_type<NetCDFElementaryType::Ubyte>     { using type = uint8_t; };
template <> struct internal_type<NetCDFElementaryType::Short>     { using type = int16_t; };
template <> struct internal_type<NetCDFElementaryType::Ushort>    { using type = uint16_t; };
template <> struct internal_type<NetCDFElementaryType::Int>       { using type = int32_t; };
template <> struct internal_type<NetCDFElementaryType::Uint>      { using type = uint32_t; };
template <> struct internal_type<NetCDFElementaryType::Long>      { using type = uint32_t; };
template <> struct internal_type<NetCDFElementaryType::Int64>     { using type = int64_t; };
template <> struct internal_type<NetCDFElementaryType::Uint64>    { using type = uint64_t; };
template <> struct internal_type<NetCDFElementaryType::Float>     { using type = float; };
template <> struct internal_type<NetCDFElementaryType::Real>      { using type = float; };
template <> struct internal_type<NetCDFElementaryType::Double>    { using type = double; };
template <> struct internal_type<NetCDFElementaryType::String>    { using type = std::string; };
template <> struct internal_type<NetCDFElementaryType::Default>   { };
// clang-format on

} // namespace ncdlgen