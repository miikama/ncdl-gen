
#pragma once

#include <string>
#include <string_view>

namespace ncdlgen
{

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
        const std::string class_name{};
    };

    Generator(Options options) : options(std::move(options)) {}

    void generate(const std::string_view input_cdl);

  private:
    Options options{};
};

} // namespace ncdlgen
