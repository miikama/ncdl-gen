
#pragma once

#include <string_view>

#include "types.h"

namespace ncdlgen
{

bool is_keyword(const std::string_view word);

std::optional<NetCDFType> type_for_token(const Token &token);

const std::string_view name_for_type(const NetCDFType &type);

// clang-format off
template <NetCDFType>
struct internal_type {};
template <> struct internal_type<NetCDFType::Char>      { using type = int8_t; };
template <> struct internal_type<NetCDFType::Byte>      { using type = int8_t; };
template <> struct internal_type<NetCDFType::Ubyte>     { using type = uint8_t; };
template <> struct internal_type<NetCDFType::Short>     { using type = int16_t; };
template <> struct internal_type<NetCDFType::Ushort>    { using type = uint16_t; };
template <> struct internal_type<NetCDFType::Int>       { using type = int32_t; };
template <> struct internal_type<NetCDFType::Uint>      { using type = uint32_t; };
template <> struct internal_type<NetCDFType::Long>      { using type = uint32_t; };
template <> struct internal_type<NetCDFType::Int64>     { using type = int64_t; };
template <> struct internal_type<NetCDFType::Uint64>    { using type = uint64_t; };
template <> struct internal_type<NetCDFType::Float>     { using type = float; };
template <> struct internal_type<NetCDFType::Real>      { using type = float; };
template <> struct internal_type<NetCDFType::Double>    { using type = double; };
template <> struct internal_type<NetCDFType::String>    { using type = std::string; };
template <> struct internal_type<NetCDFType::Default>   { };
// clang-format on

} // namespace ncdlgen