
#include <cassert>
#include <exception>
#include <fmt/core.h>

#include "netcdf.h"

#include "netcdf_pipe.h"
#include "utils.h"

namespace ncdlgen
{

void NetCDFPipe::throw_error(std::string_view message, int error_code)
{
    throw std::runtime_error(
        fmt::format("Error with NetCDF function {}: '{}'.", message, nc_strerror(error_code)));
}

void NetCDFPipe::assert_open()
{
    if (root_id < 0)
    {
        throw std::runtime_error("Trying to acces netcdf file that is not open.");
    }
}

void NetCDFPipe::open()
{
    if (auto res = nc_open(path.c_str(), NC_WRITE, &root_id))
    {
        throw_error("nc_create", res);
    }
}

void NetCDFPipe::close()
{
    if (root_id < 0)
    {
        return;
    }

    if (auto res = nc_close(root_id))
    {
        throw_error("nc_close", res);
    }
}

NetCDFPipe::Path NetCDFPipe::resolve_path(const std::string_view path)
{
    assert_open();

    auto split_path = split_string(path, '/');
    assert(split_path.size() > 0);

    int parent_group_id{root_id};

    // Resolve groups
    for (size_t i = 0; i < split_path.size() - 1; i++)
    {
        parent_group_id = get_group_id(parent_group_id, split_path.at(i));
    }

    // resolve variable
    int variable_id = get_variable_id(parent_group_id, split_path.back());

    return Path{.group_id = parent_group_id, .variable_id = variable_id};
}

NetCDFPipe::VariableInfo NetCDFPipe::get_variable_info(const Path& path)
{
    assert_open();

    // variable type
    int variable_type{-1};
    if (auto ret = nc_inq_vartype(path.group_id, path.variable_id, &variable_type))
    {
        throw_error("nc_inq_vartype", ret);
    }

    // N dimensions
    int n_dimensions{-1};
    if (auto ret = nc_inq_varndims(path.group_id, path.variable_id, &n_dimensions))
    {
        throw_error("nc_inq_varndims", ret);
    }
    assert(n_dimensions < NC_MAX_DIMS && n_dimensions >= 0);

    // Dimension IDS
    std::vector<int> dimension_ids(n_dimensions, -1);
    if (auto ret = nc_inq_vardimid(path.group_id, path.variable_id, dimension_ids.data()))
    {
        throw_error("nc_inq_vardimid", ret);
    }

    // Size for each dimension

    std::vector<std::size_t> dimension_sizes{};
    for (auto& dimension_id : dimension_ids)
    {
        Path dimension_path{path.group_id, dimension_id};
        dimension_sizes.emplace_back(get_dimension_size(dimension_path));
    }

    return VariableInfo{.group_id = path.group_id,
                        .variable_id = path.variable_id,
                        .dimension_ids = dimension_ids,
                        .dimension_sizes = dimension_sizes,
                        .nc_type = variable_type};
}

int NetCDFPipe::get_group_id(const int parent_group_id, const std::string_view group_name)
{
    assert_open();
    int group_id{-1};

    std::string current_path{group_name};
    if (auto ret = nc_inq_grp_ncid(parent_group_id, current_path.c_str(), &group_id))
    {
        throw_error(fmt::format("nc_inq_grp_ncid('{}')", group_name), ret);
    }
    return group_id;
}

int NetCDFPipe::get_variable_id(const int group_id, std::string_view path)
{
    assert_open();
    std::string variable_name{path};
    int variable_id{-1};
    if (auto ret = nc_inq_varid(group_id, variable_name.c_str(), &variable_id))
    {
        throw_error(fmt::format("nc_inq_varid({})", path), ret);
    }
    return variable_id;
}

std::size_t NetCDFPipe::get_dimension_size(const Path& path)
{
    assert_open();
    std::size_t dimension_length{};
    if (auto ret = nc_inq_dimlen(path.group_id, path.variable_id, &dimension_length))
    {
        throw_error("nc_inq_dimlen", ret);
    }
    return dimension_length;
}

} // namespace ncdlgen