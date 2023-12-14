
#pragma once

#include <functional>
#include <string>
#include <string_view>

#include "syntax.h"

namespace ncdlgen
{

struct DefaultCustomisation
{
    static std::string container_for_dimensions(const std::string_view& element_type_name,
                                                const std::vector<ncdlgen::VariableDimension>& dimensions);
};

class Generator
{
  public:
    enum class GenerateTarget
    {
        Header,
        Source,
    };

    struct Options
    {
        GenerateTarget target{GenerateTarget::Header};
        std::string class_name{};
        std::string serialisation_interface{"NetCDFInterface"};
        std::string array_interface{"VectorInterface"};
        std::vector<std::string> base_headers{"stdint.h", "netcdf_interface.h"};
        std::vector<std::string> library_headers{"<vector>"};
        std::vector<std::string> interface_headers{"vector_interface.h"};
        std::function<std::string(const std::string_view&, const std::vector<ncdlgen::VariableDimension>&)>
            container_for_dimensions{DefaultCustomisation::container_for_dimensions};
    };

    Generator(Options options) : options(std::move(options)) {}

    void generate(const std::string_view input_cdl);

  private:
    // header
    void dump_header(const ncdlgen::Group& group, int indent);
    void dump_header_reading(const ncdlgen::Group& group, const std::string_view fully_qualified_struct_name);
    void dump_header_writing(const ncdlgen::Group& group, const std::string_view fully_qualified_struct_name);
    void dump_header_namespace(const ncdlgen::Group& group);

    // source
    void dump_source_read_group(const ncdlgen::Group& group, const std::string_view group_path,
                                const std::string_view name_space_name);
    void dump_source_write_group(const ncdlgen::Group& group, const std::string_view group_path,
                                 const std::string_view name_space_name);
    void dump_source(const ncdlgen::Group& group, const std::string_view group_path);
    void dump_source_headers(const ncdlgen::Group& group);

    // options
    Options options{};
};

} // namespace ncdlgen
