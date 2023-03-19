#pragma once

#include <type_traits>
#include <vector>

#include "utils.h"

namespace ncdlgen
{

template <typename ElementType, typename ContainerType> constexpr std::false_type is_supported_ndarray;

// Define a _v helper
template <typename ElementType, typename ContainerType>
inline constexpr bool is_supported_ndarray_v = is_supported_ndarray<ElementType, ContainerType>();

namespace interface
{

template <typename ElementType, typename ContainerType>
void resize(ContainerType& data, const std::vector<std::size_t>& dimension_sizes)
{
    auto func = [](int a) { return 0; };
    func(data);
    static_assert(always_false_v<ContainerType>, "The resize interface not implemented.");
}

} // namespace interface

} // namespace ncdlgen
