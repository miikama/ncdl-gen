#pragma once

#include <vector>

#include <fmt/core.h>

#include "interface.h"

namespace ncdlgen
{

namespace VectorOperations
{

/**
 * is vector
 */
template <typename T, typename U = void> struct is_vector : std::false_type
{
};

template <typename T>
struct is_vector<T, std::enable_if_t<std::is_same_v<T, std::vector<typename T::value_type>>>>
    : public std::true_type
{
};
template <typename T> inline constexpr bool is_vector_v = is_vector<T>::value;

static_assert(is_vector_v<std::vector<int>>);
static_assert(is_vector_v<std::vector<short unsigned int>>);
static_assert(is_vector_v<std::vector<std::vector<int>>>);
static_assert(!is_vector_v<int>);

/**
 * dimension count
 */

template <typename T, typename _ = void> struct dimension_count
{
    static constexpr size_t value = 0;
};

template <typename T> struct dimension_count<T, std::enable_if_t<is_vector_v<T>>>
{
    static constexpr size_t value = dimension_count<typename T::value_type>::value + 1;
};

template <typename T> inline constexpr size_t dimension_count_v = dimension_count<T>::value;

static_assert(dimension_count_v<int> == 0, "Scalar has 0 dimension");
static_assert(dimension_count_v<std::vector<int>> == 1, "vector has 1 dimension");
static_assert(dimension_count_v<std::vector<std::vector<int>>> == 2, "2d vector has 2 dimensions");

/**
 * Helper for total element size as the factor of input dimensions.
 */
std::size_t number_of_elements(const std::vector<std::size_t>& dimension_sizes);

/**
 * Resize the input Container (vector of vectors) based on input dimension sizes
 */
template <typename ContainerType>
void resize(ContainerType& container, const std::vector<std::size_t>& dimension_sizes)
{
    if (dimension_count_v<ContainerType> != dimension_sizes.size())
    {
        throw std::runtime_error(
            fmt::format("Error with resizing container with {} dimensions with {} target dimensions.",
                        dimension_count_v<ContainerType>, dimension_sizes.size()));
    }
    if constexpr (dimension_count_v<ContainerType> == 0)
    {
        return;
    }
    else
    {
        container.resize(dimension_sizes.front(), {});
        for (auto& element : container)
        {
            std::vector<std::size_t> reduced_sizes(dimension_sizes.begin() + 1, dimension_sizes.end());
            resize<typename ContainerType::value_type>(element, reduced_sizes);
        }
    }
};

/**
 * Assign to input Container (vector of vectors) from a flat vector that has the
 * correct amount of elements.
 */
template <typename ElementType, typename ContainerType>
void assign(ContainerType& container, const std::vector<ElementType>& data, std::size_t& flat_index)
{
    // When reaching scalar, assign from flat index and increase the index
    if constexpr (dimension_count_v<ContainerType> == 0)
    {
        if (flat_index >= data.size())
        {
            throw std::runtime_error(
                fmt::format("Trying to assign to Container at index {}, when data available for {} entries.",
                            flat_index, data.size()));
        }

        container = data[flat_index];
        flat_index++;
        return;
    }
    // When going through the vector, call recursively into subvectors/scalars
    else
    {
        for (auto& element : container)
        {
            assign<ElementType, typename ContainerType::value_type>(element, data, flat_index);
        }
    }
};

/**
 * Assign to input Container (vector of vectors) from a flat vector that has the
 * correct amount of elements.
 *
 * Main interface for users, set up the flat index counter
 */
template <typename ElementType, typename ContainerType>
void assign(ContainerType& container, const std::vector<ElementType>& data)
{
    size_t flat_index{};
    assign(container, data, flat_index);
};

}; // namespace VectorOperations

struct VectorInterface
{
    // ND stl vector are supported
    template <typename ElementType, typename ContainerType,
              std::enable_if_t<VectorOperations::is_vector_v<ContainerType>, bool> = true>
    static constexpr bool is_supported_ndarray()
    {
        return true;
    };

    template <typename ElementType, typename ContainerType>
    static Data<ElementType> prepare(const std::vector<std::size_t>& dimension_sizes)
    {
        Data<ElementType> data{};
        data.dimension_sizes = dimension_sizes;
        data.data.resize(VectorOperations::number_of_elements(dimension_sizes));
        return data;
    }

    template <typename ElementType, typename ContainerType>
    static void finalise(ContainerType& output, const Data<ElementType>& data)
    {
        VectorOperations::resize(output, data.dimension_sizes);
        VectorOperations::assign(output, data.data);
    }

    template <typename ElementType>
    static constexpr void resize(std::vector<ElementType>& data,
                                 const std::vector<std::size_t>& dimension_sizes)
    {
        data.resize(VectorOperations::number_of_elements(dimension_sizes));
    }
};

} // namespace ncdlgen
