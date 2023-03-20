#pragma once

#include <vector>

#include "interface.h"

namespace ncdlgen
{

// 1D stl vector are supported
template <typename ElementType>
constexpr std::true_type is_supported_ndarray<ElementType, std::vector<ElementType>>;

namespace interface
{

template <typename ElementType> constexpr std::size_t element_count(const std::vector<ElementType>& data)
{
    return data.size();
}

template <typename ElementType>
void resize(std::vector<ElementType>& data, const std::vector<std::size_t>& dimension_sizes)
{
    std::size_t number_of_elements{1};
    for (auto& dimension_size : dimension_sizes)
    {
        number_of_elements *= dimension_size;
    }

    data.resize(number_of_elements);
}

} // namespace interface

} // namespace ncdlgen
