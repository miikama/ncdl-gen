
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "tokeniser.h"

namespace ncdlgen {

class Element {

  public:
    virtual void print() const = 0;
    const std::string_view name() const { return m_name; };

  protected:
    std::string m_name{};
};

class RootGroup : public Element {
  public:
    void print() const override {}
};

class Parser {

  public:
    std::optional<RootGroup> parse(const std::vector<Token> &tokens);

  private:
    size_t m_cursor{};
};

} // namespace ncdlgen
