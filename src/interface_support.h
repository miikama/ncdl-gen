#pragma once

#include <type_traits>

namespace ncdlgen
{

template <typename ContainerType, typename Enable = void> struct is_supported_ndarray : public std::false_type
{
};

// 1D stl containers are supported, marked by type trait value_type
template <typename ContainerType>
struct is_supported_ndarray<
    ContainerType, typename std::enable_if<std::is_arithmetic_v<typename ContainerType::value_type>>::type>
    : public std::true_type
{
};

template <typename ContainerType>
inline constexpr bool is_supported_ndarray_v = is_supported_ndarray<ContainerType>::value;

} // namespace ncdlgen
