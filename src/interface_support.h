#pragma once

#include <type_traits>
#include <vector>

namespace ncdlgen
{

template <typename ElementType, typename ContainerType> constexpr std::false_type is_supported_ndarray;

// 1D stl vector are supported
template <typename ElementType>
constexpr std::true_type is_supported_ndarray<ElementType, std::vector<ElementType>>;

// Define a _v helper
template <typename ElementType, typename ContainerType>
inline constexpr bool is_supported_ndarray_v = is_supported_ndarray<ElementType, ContainerType>();

} // namespace ncdlgen
