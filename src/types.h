

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "tokeniser.h"

namespace ncdlgen
{

class Parser;

class Element
{

  public:
    Element() = default;
    Element(const std::string_view &name) : m_name(name) {}
    virtual ~Element() = default;

    virtual std::string description(int indent) const = 0;
    const std::string_view name() const { return m_name; };

  protected:
    std::string m_name{};
};

enum class NetCDFType
{
    Char,
    Byte,
    Ubyte,
    Short,
    Ushort,
    Int,
    Uint,
    Long,
    Int64,
    Uint64,
    Float,
    Real,
    Double,
    String,
    Default,
};

class Type : public Element
{
  public:
    Type(const std::string_view &name) : Element(name) {}

    virtual std::string description(int indent) const = 0;

    static std::unique_ptr<Type> parse(Parser &);

    virtual const bool is_opaque() const { return false; }
    virtual const bool is_vlen() const { return false; }
    virtual const bool is_enum() const { return false; }
};

using DimensionLength = size_t;

class OpaqueType : public Type
{
  public:
    OpaqueType(const std::string_view &name, const size_t &length)
        : m_length(length), Type(name)
    {
    }

    std::string description(int indent) const override;

    const bool is_opaque() const override { return true; }

  private:
    DimensionLength m_length{};
};

struct EnumValue
{
    std::string name{};
    int value{};

    static std::optional<EnumValue> parse(Parser &);
};

class EnumType : public Type
{
    friend class Type;

  public:
    EnumType(const std::string_view &name, const NetCDFType type)
        : m_type(type), Type(name)
    {
    }

    std::string description(int indent) const override;

    const bool is_enum() const override { return true; }

  private:
    NetCDFType m_type{NetCDFType::Default};
    std::vector<EnumValue> m_values{};
};

class VLenType : public Type
{
  public:
    VLenType(const std::string_view &name, const NetCDFType type)
        : m_type(type), Type(name)
    {
    }

    std::string description(int indent) const override;

    const bool is_vlen() const override { return true; }

  private:
    NetCDFType m_type{NetCDFType::Default};
};

class Types : public Element
{
  public:
    std::string description(int indent) const override;

    static std::optional<Types> parse(Parser &);

  private:
    std::vector<std::unique_ptr<Type>> m_types{};
};

class Dimension : public Element
{
  public:
    std::string description(int indent) const override;

    static std::optional<Dimension> parse(Parser &);

  private:
    DimensionLength m_length{};
};

class Dimensions : public Element
{
  public:
    std::string description(int indent) const override;

    static std::optional<Dimensions> parse(Parser &);

  private:
    std::vector<Dimension> m_dimensions{};
};

class Variable : public Element
{
  public:
    std::string description(int indent) const override;

    static std::optional<Variable> parse(Parser &);

  private:
    double m_value{};
    NetCDFType m_type { NetCDFType::Default };
};

class Variables : public Element
{
  public:
    std::string description(int indent) const override;

    static std::optional<Variables> parse(Parser &);

  private:
    std::vector<Variable> m_variables{};
};

class Group : public Element
{
  public:

    /**
     * Group contains types which have unique_ptrs. Therefore, the
     * groups cannot be trivially copied aroud and must be moved.
     * 
     * See also reason for noexcept
     * https://stackoverflow.com/questions/64758775/result-type-must-be-constructible-from-value-type-of-input-range-when-trying-t
    */
    Group() = default;
    Group(Group&& group) noexcept {
        m_name = std::move(group.m_name);
        m_types = std::move(group.m_types);
        m_dimensions = std::move(group.m_dimensions);
        m_groups = std::move(group.m_groups);
        m_variables = std::move(group.m_variables);
    }
    Group& operator= (Group&& group) noexcept {
        m_name = std::move(group.m_name);
        m_types = std::move(group.m_types);
        m_dimensions = std::move(group.m_dimensions);
        m_groups = std::move(group.m_groups);
        m_variables = std::move(group.m_variables);
        return *this;
    }

    std::string description(int indent) const override;

    static std::optional<Group> parse(Parser &);

  private:
    std::optional<Types> m_types{};
    std::optional<Dimensions> m_dimensions{};
    std::optional<Variables> m_variables{};
    std::vector<Group> m_groups{};
};

class RootGroup : public Element
{
  public:
    void print_tree();

    std::string description(int indent) const override;

    static std::optional<RootGroup> parse(Parser &);

  private:
    std::optional<Group> m_group{};
};

} // namespace ncdlgen
