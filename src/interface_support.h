#pragma once

#include <type_traits>
#include <vector>

namespace ncdlgen
{

template <typename ElementType, typename ContainerType> constexpr std::false_type is_supported_ndarray;

// Define a _v helper
template <typename ElementType, typename ContainerType>
inline constexpr bool is_supported_ndarray_v = is_supported_ndarray<ElementType, ContainerType>();

} // namespace ncdlgen
