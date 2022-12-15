

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "tokeniser.h"

namespace ncdlgen
{

/**
 * This type business needs rethinking at some point
 */
struct UserType
{
    std::string name{};
};

enum class NetCDFElementaryType
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

struct NetCDFType
{
    constexpr NetCDFType(const NetCDFElementaryType &type) : type(type) {}
    constexpr NetCDFType(const UserType &type) : type(type) {}

    std::variant<NetCDFElementaryType, UserType> type;

    std::string_view name() const;
};

// Forward declaration
class Parser;

struct Number
{

    template <typename InternalType>
    Number(InternalType value, NetCDFElementaryType type) : value(value), netcdf_type(type)
    {
    }

    std::string as_string() const;

    std::variant<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double> value{};
    NetCDFElementaryType netcdf_type{NetCDFElementaryType::Default};
};

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
using DimensionLength = size_t;

struct OpaqueType
{
    OpaqueType(const std::string_view &name, const size_t &length) : length(length), name(name) {}

    std::string as_string() const;

    DimensionLength length{};
    std::string name{};
};

struct EnumValue
{
    std::string name{};
    int value{};

    static std::optional<EnumValue> parse(Parser &);
};

struct EnumType
{
    EnumType(const std::string_view &name, const NetCDFElementaryType type) : type(type), name(name) {}

    std::string as_string() const;

    NetCDFElementaryType type{NetCDFElementaryType::Default};
    std::vector<EnumValue> enum_values{};
    std::string name{};
};

struct VLenType
{
    VLenType(const std::string_view &name, const NetCDFElementaryType type) : type(type), name(name) {}

    std::string as_string() const;

    NetCDFElementaryType type{NetCDFElementaryType::Default};
    std::string name{};
};

struct ComplexType
{
    explicit ComplexType(OpaqueType type) : type(type) {}
    explicit ComplexType(VLenType type) : type(type) {}
    explicit ComplexType(EnumType type) : type(type) {}

    std::string description() const;
    std::string name() const;

    static std::optional<ComplexType> parse(Parser &);

    std::variant<OpaqueType, EnumType, VLenType> type;
};

struct Types
{
    std::string description(int indent) const;
    static std::optional<Types> parse(Parser &);
    std::vector<ComplexType> types{};
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

struct ValidRangeValue
{
    Number start;
    Number end;
};
using FillValueAttributeValue = Number;

class Attribute : public Element
{
  public:
    std::string description(int indent) const override;

    static std::optional<Attribute> parse(Parser &, std::optional<NetCDFType> attribute_type);

    // Get string representation of the contained value
    std::string as_string() const;

  private:
    std::optional<NetCDFType> m_type{};
    std::optional<std::string> m_variable_name{};
    std::string m_attribute_name{};

    // Either this is a variant of all different attribute value types
    // or this is handled via inheritance and this is a unique_ptr
    // to AttributeValue
    std::variant<std::string, ValidRangeValue, FillValueAttributeValue> m_value{};
};

class VariableDimension : public Element
{
  public:
    VariableDimension(const std::string_view name) : Element(name) {}
    std::string description(int indent) const override;
    static std::optional<VariableDimension> parse(Parser &);
};

class Variable : public Element
{
  public:
    std::string description(int indent) const override;

    static std::optional<Variable> parse(Parser &, NetCDFType existing_type);

    const NetCDFType type() const { return m_type; }

  private:
    double m_value{};
    NetCDFType m_type{NetCDFElementaryType::Default};
    std::vector<VariableDimension> m_dimensions{};
};

struct VariableDeclaration
{
    using VariableDeclarationType = std::variant<Variable, Attribute>;

    std::string description(int indent) const;

    static std::optional<VariableDeclarationType> parse(Parser &, std::optional<NetCDFType> existing_type);
};

class Variables : public Element
{
  public:
    std::string description(int indent) const override;

    static std::optional<Variables> parse(Parser &);

  private:
    std::vector<Variable> m_variables{};
    std::vector<Attribute> m_attributes{};
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
    Group(Group &&group) noexcept
    {
        m_name = std::move(group.m_name);
        m_types = std::move(group.m_types);
        m_dimensions = std::move(group.m_dimensions);
        m_groups = std::move(group.m_groups);
        m_variables = std::move(group.m_variables);
    }
    Group &operator=(Group &&group) noexcept
    {
        m_name = std::move(group.m_name);
        m_types = std::move(group.m_types);
        m_dimensions = std::move(group.m_dimensions);
        m_groups = std::move(group.m_groups);
        m_variables = std::move(group.m_variables);
        return *this;
    }

    std::string description(int indent) const override;

    static std::optional<Group> parse(Parser &);

    const std::vector<ComplexType> &types() const;

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
