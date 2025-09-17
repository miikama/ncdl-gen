#pragma once

#include <type_traits>
#include <vector>

#include "utils.h"

namespace ncdlgen
{

template <typename ElementType> struct Data
{
    std::vector<ElementType> data{};
    std::vector<std::size_t> dimension_sizes{};
};

struct Interface
{

    template <typename ElementType, typename ContainerType, typename Enable = void>
    static constexpr bool is_supported_ndarray()
    {
        return false;
    };

    template <typename ElementType, typename ContainerType>
    static Data<ElementType> prepare(const std::vector<std::size_t>& dimension_sizes)
    {
        static_assert(always_false_v<ContainerType>, "The prepare interface not implemented.");
    }

    template <typename ElementType, typename ContainerType>
    static void finalise(ContainerType& output, Data<ElementType>& data)
    {
        static_assert(always_false_v<ContainerType>, "The finalise interface not implemented.");
    }
};

} // namespace ncdlgen
