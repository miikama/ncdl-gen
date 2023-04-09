#pragma once

#include <type_traits>
#include <vector>

#include "utils.h"

namespace ncdlgen
{

struct Interface
{
    using container_type_t = void;

    template <typename ElementType, typename ContainerType>
    static constexpr bool is_supported_ndarray(const ContainerType&)
    {
        return false;
    };

    template <typename ElementType, typename ContainerType>
    static constexpr std::size_t element_count(const ContainerType& data)
    {
        static_assert(always_false_v<ContainerType>, "The element_count interface not implemented.");
        return 0;
    }

    template <typename ElementType, typename ContainerType>
    static void resize(ContainerType& data, const std::vector<std::size_t>& dimension_sizes)
    {
        static_assert(always_false_v<ContainerType>, "The resize interface not implemented.");
    }
};

} // namespace ncdlgen
