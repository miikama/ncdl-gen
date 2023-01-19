
#pragma once

#include <string>

namespace ncdlgen
{

struct Description
{

    Description(const int indent, bool add_new_line = true) : indent(indent), new_line(add_new_line) {}

    void operator<<(const std::string& input)
    {
        std::string line{};
        for (size_t i = 0; i < indent * 2; i++)
        {
            line += " ";
        }
        line += input;
        if (new_line)
        {
            line += "\n";
        }
        description += line;
    }

    void push_indent() { indent++; }
    void pop_indent()
    {
        if (indent > 0)
        {
            indent--;
        }
    }

    std::string description{};
    size_t indent{};
    bool new_line{};
};

} // namespace ncdlgen