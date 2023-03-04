
#include <cassert>
#include <exception>
#include <fmt/core.h>

#include "netcdf.h"

#include "netcdf_interface.h"
#include "utils.h"

namespace ncdlgen
{

void NetCDFInterface::throw_error(std::string_view message, int error_code)
{
    throw std::runtime_error(
        fmt::format("Error with NetCDF function {}: '{}'.", message, nc_strerror(error_code)));
}

void NetCDFInterface::assert_open()
{
    if (root_id < 0)
    {
        throw std::runtime_error("Trying to acces netcdf file that is not open.");
    }
}

void NetCDFInterface::open()
{
    if (auto res = nc_open(path.c_str(), NC_WRITE, &root_id))
    {
        throw_error("nc_create", res);
    }
}

void NetCDFInterface::close()
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

NetCDFInterface::Path NetCDFInterface::resolve_path(const std::string_view path)
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

int NetCDFInterface::get_group_id(const int parent_group_id, const std::string_view group_name)
{
    assert_open();
    int group_id{-1};

    std::string current_path{group_name};
    if (auto ret = nc_inq_grp_ncid(parent_group_id, current_path.c_str(), &group_id))
    {
        throw_error("nc_inq_grp_ncid", ret);
    }
    return group_id;
}

int NetCDFInterface::get_variable_id(const int group_id, std::string_view path)
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

} // namespace ncdlgen