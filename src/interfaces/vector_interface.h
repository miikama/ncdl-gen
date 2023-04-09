#pragma once

#include <vector>

#include "interface.h"

namespace ncdlgen
{

struct VectorInterface
{
    template <typename ElementType> using container_type_t = std::vector<ElementType>;

    // 1D stl vector are supported
    template <typename ElementType>
    static constexpr bool is_supported_ndarray(const std::vector<ElementType>&)
    {
        return true;
    };

    template <typename ElementType>
    static constexpr std::size_t element_count(const std::vector<ElementType>& data)
    {
        return data.size();
    }

    template <typename ElementType>
    static constexpr void resize(std::vector<ElementType>& data,
                                 const std::vector<std::size_t>& dimension_sizes)
    {
        std::size_t number_of_elements{1};
        for (auto& dimension_size : dimension_sizes)
        {
            number_of_elements *= dimension_size;
        }

        data.resize(number_of_elements);
    }
};

} // namespace ncdlgen
