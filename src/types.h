

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "tokeniser.h"

namespace ncdlgen {

class Parser;

class Element {

  public:
    virtual std::string description() const = 0;
    const std::string_view name() const { return m_name; };

  protected:
    std::string m_name{};
};

class Types : public Element {
  public:
    std::string description() const override;

    static std::optional<Types> parse(Parser &);
};

using DimensionLength = size_t;

class Dimension : public Element {
  public:
    std::string description() const override;

    static std::optional<Dimension> parse(Parser &);

  private:
    DimensionLength m_length{};
};

class Dimensions : public Element {
  public:
    std::string description() const override;

    static std::optional<Dimensions> parse(Parser &);

  private:
    std::vector<Dimension> m_dimensions{};
};

class Group : public Element {
  public:
    std::string description() const override;

    static std::optional<Group> parse(Parser &);

  private:
    std::optional<Types> m_types{};
    std::optional<Dimensions> m_dimensions{};
};

class RootGroup : public Element {
  public:
    void print_tree();

    std::string description() const override;

    static std::optional<RootGroup> parse(Parser &);

  private:
    std::optional<Group> m_group{};
};

} // namespace ncdlgen
