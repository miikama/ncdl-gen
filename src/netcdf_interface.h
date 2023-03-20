
#pragma once

#include <cassert>
#include <filesystem>
#include <string_view>

#include "netcdf.h"
#include <fmt/core.h>

#include "utils.h"
#include "vector_interface.h"

namespace ncdlgen
{

/**
 * Write and read data from netcdf.
 *
 * Currently support situation where the netcdf structure is defined in a cdl file
 * and an empty netcdf file is created by ncgen.
 *
 * Now we do not have to define all the dimensions for the variables
 */
class NetCDFInterface
{
  public:
    NetCDFInterface(std::string_view file_path) : path(file_path) {}

    virtual ~NetCDFInterface() = default;

    void open();
    void close();

    struct Path
    {
        int group_id{};
        int variable_id{};
    };

    /**
     * Resolve path e.g. /group/variable
     *
     * Get the parent group id and variable id
     */
    Path resolve_path(const std::string_view path);

    /**
     * Container for all the variable related fields
     *
     *  In the NetCDF representation, i.e. with id fields, not
     *  human readable fields.
     */
    struct VariableInfo
    {
        int32_t group_id{-1};
        int32_t variable_id{-1};
        std::vector<int32_t> dimension_ids{};
        std::vector<std::size_t> dimension_sizes{};
        int32_t nc_type{};
    };

    VariableInfo get_variable_info(const Path& path);

    /**
     * Main inteface for writing data to netcdf
     */
    template <typename ContainerType, typename ElementType, typename ContainerInterface>
    void write(const std::string_view full_path, const ContainerType& data)
    {
        auto path = resolve_path(full_path);

        // Get all information about the variable
        auto variable_info = get_variable_info(path);

        // TODO: Make sure resolved variable type and dimensions match

        if constexpr (std::is_arithmetic_v<ContainerType>)
        {
            if (auto ret = nc_put_var(path.group_id, path.variable_id, &data))
            {
                throw_error(fmt::format("nc_put_var ({})", full_path), ret);
            }
        }
        // 1D container
        else if constexpr (ContainerInterface::is_supported_ndarray(data))
        {
            std::vector<std::size_t> count = variable_info.dimension_sizes;
            std::vector<std::size_t> start(count.size(), 0);

            if (auto ret =
                    nc_put_vara(path.group_id, path.variable_id, start.data(), count.data(), data.data()))
            {
                throw_error(fmt::format("nc_put_var ({})", full_path), ret);
            }
        }
        else
        {
            static_assert(always_false_v<ContainerType>, "Unsupported type for writing to NetCDF");
        }
    }

    /**
     * Main inteface for reading data from netcdf
     */
    template <typename ContainerType, typename ElementType, typename ContainerInterface>
    ContainerType read(const std::string_view full_path)
    {
        auto path = resolve_path(full_path);

        ContainerType data;

        // Get all information about the variable
        auto variable_info = get_variable_info(path);

        // TODO: Make sure resolved variable type and dimensions match

        if constexpr (std::is_arithmetic_v<ContainerType>)
        {
            if (auto ret = nc_get_var(path.group_id, path.variable_id, &data))
            {
                throw_error(fmt::format("nc_get_var ({})", full_path), ret);
            }
        }
        else if constexpr (ContainerInterface::is_supported_ndarray(data))
        {
            std::vector<std::size_t> count = variable_info.dimension_sizes;
            std::vector<std::size_t> start(count.size(), 0);

            // see https://stackoverflow.com/a/613132
            // Let the compiler know that resize is a template
            ContainerInterface::template resize<ElementType>(data, variable_info.dimension_sizes);

            if (auto ret =
                    nc_get_vara(path.group_id, path.variable_id, start.data(), count.data(), data.data()))
            {
                throw_error(fmt::format("nc_get_vara ({})", full_path), ret);
            }
        }
        else
        {
            static_assert(always_false_v<ContainerType>, "Unsupported type for writing to NetCDF");
        }

        return data;
    }

  private:
    void assert_open();
    void throw_error(std::string_view message, int error_code);

    int get_group_id(const int parent_group_id, const std::string_view variable_name);
    int get_variable_id(const int group_id, std::string_view path);
    std::size_t get_dimension_size(const Path& path);

    std::filesystem::path path{};
    int root_id{-1};
};

} // namespace ncdlgen
