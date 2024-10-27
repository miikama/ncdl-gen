#include "vector_interface.h"

namespace ncdlgen
{

std::size_t VectorOperations::number_of_elements(const std::vector<std::size_t>& dimension_sizes)
{
    if (dimension_sizes.empty())
    {
        return 0;
    }

    std::size_t number_of_elements{1};
    for (auto& dimension_size : dimension_sizes)
    {
        number_of_elements *= dimension_size;
    }
    return number_of_elements;
}

} // namespace ncdlgen
