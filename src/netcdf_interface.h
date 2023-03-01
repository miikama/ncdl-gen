
#pragma once

#include <string_view>

class NetCDFInterface
{
  public:
    template <typename T> void write(std::string_view path, const T& data) {}
    template <typename T> T read(std::string_view path) { return T{}; }
};
