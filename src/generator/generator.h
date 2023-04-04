
#pragma once

#include <string>
#include <string_view>

namespace ncdlgen
{

class Generator
{
  public:
    struct Options
    {
        const std::string class_name{};
    };

    Generator(Options options) : options(std::move(options)) {}

    void generate(const std::string_view input_cdl);

  private:
    Options options{};
};

} // namespace ncdlgen
