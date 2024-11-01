

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

// Forward declaration
class Parser;
class Group;
struct ComplexType;
struct NetCDFType;

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

using DimensionLength = size_t;

struct Dimension
{
    std::string description(int indent) const;

    static std::optional<Dimension> parse(Parser&);

    DimensionLength length{};
    std::string name{};
};

struct Dimensions
{
  public:
    std::string description(int indent) const;
    std::string as_string() const;

    static std::optional<Dimensions> parse(Parser&);

    std::vector<Dimension> dimensions{};
};

struct String
{
    String(std::string value, NetCDFElementaryType type) : value(std::move(value)), netcdf_type(type) {}

    std::string as_string() const;

    std::string value{};
    NetCDFElementaryType netcdf_type{NetCDFElementaryType::Default};
};

struct Number
{

    template <typename InternalType>
    Number(InternalType value, NetCDFElementaryType type) : value(value), netcdf_type(type)
    {
    }

    std::string as_string() const;

    std::variant<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double>
        value{};
    NetCDFElementaryType netcdf_type{NetCDFElementaryType::Default};
};

struct Array
{
    Array() {}

    std::string as_string() const;

    std::vector<Number> data;
};

struct VariableData
{
    static std::optional<VariableData> parse(Parser&, const NetCDFType& type);

    VariableData(const Number& number) : data(number) {}
    VariableData(const String& data) : data(data) {}
    VariableData(const Array& array) : data(array) {}

    std::string as_string() const;

    std::variant<Number, String, Array> data;
};

class Element
{

  public:
    Element() = default;
    Element(const std::string_view& name) : m_name(name) {}
    virtual ~Element() = default;

    virtual std::string description(int indent) const = 0;
    const std::string_view name() const { return m_name; };

  protected:
    std::string m_name{};
};

struct OpaqueType
{
    OpaqueType(const std::string_view& name, const size_t& length) : length(length), name(name) {}

    std::string as_string() const;

    DimensionLength length{};
    std::string name{};
};

struct EnumValue
{
    std::string name{};
    int value{};

    static std::optional<EnumValue> parse(Parser&);
};

struct EnumType
{
    EnumType(const std::string_view& name, const NetCDFElementaryType type) : type(type), name(name) {}

    std::string as_string() const;

    NetCDFElementaryType type{NetCDFElementaryType::Default};
    std::vector<EnumValue> enum_values{};
    std::string name{};
};

struct VLenType
{
    VLenType(const std::string_view& name, const NetCDFElementaryType type) : type(type), name(name) {}

    std::string as_string() const;

    NetCDFElementaryType type{NetCDFElementaryType::Default};
    std::string name{};
};

struct CompoundType
{
    CompoundType(const std::string_view name) : name(name) {}
    std::string as_string() const;

    static std::optional<CompoundType> parse(Parser&);

    void add_type(const std::string_view name, const ComplexType& type);

    std::vector<ComplexType> types;
    std::vector<std::string> type_names;
    std::string name{};
};

struct ArrayType
{
    std::string as_string() const;

    NetCDFElementaryType type{NetCDFElementaryType::Default};
    std::string name{};
    Dimensions dimensions{};
};

struct ComplexType
{
    explicit ComplexType(OpaqueType type) : type(type) {}
    explicit ComplexType(VLenType type) : type(type) {}
    explicit ComplexType(EnumType type) : type(type) {}
    explicit ComplexType(ArrayType type) : type(type) {}
    explicit ComplexType(CompoundType type) : type(type) {}

    std::string as_string() const;
    std::string name() const;

    static std::optional<ComplexType> parse(Parser&);

    std::variant<OpaqueType, EnumType, VLenType, ArrayType, CompoundType> type;
};

struct NetCDFType
{
    constexpr NetCDFType(const NetCDFElementaryType& type) : type(type) {}
    constexpr NetCDFType(const ComplexType& type) : type(type) {}

    std::variant<NetCDFElementaryType, ComplexType> type;

    std::string_view name() const;

    std::optional<ComplexType> as_complex_type() const;
};

struct Types
{
    static void parse(Parser&, std::vector<ComplexType>& types);
};

struct ValidRangeValue
{
    Number start;
    Number end;

    std::string as_string() const { return "[" + start.as_string() + ", " + end.as_string() + "]"; }
};
using FillValueAttributeValue = Number;

class Attribute : public Element
{
  public:
    std::string description(int indent) const override;

    static std::optional<Attribute> parse(Parser&, std::optional<NetCDFType> attribute_type);

    // Get string representation of the contained value
    std::string as_string() const;

    std::optional<NetCDFType> type() { return m_type; }
    const std::string_view name() const { return m_attribute_name; }
    std::string string_data() const;

  private:
    std::optional<NetCDFType> m_type{};
    std::optional<std::string> m_variable_name{};
    std::string m_attribute_name{};

    // Either this is a variant of all different attribute value types
    // or this is handled via inheritance and this is a unique_ptr
    // to AttributeValue
    std::variant<std::string, ValidRangeValue, FillValueAttributeValue, VariableData> m_value{};
};

class VariableDimension : public Element
{
  public:
    VariableDimension(const std::string_view name) : Element(name) {}
    std::string description(int indent) const override;
    static std::optional<VariableDimension> parse(Parser&);
};

class Variable : public Element
{
  public:
    std::string description(int indent) const override;

    static std::optional<Variable> parse(Parser&, NetCDFType existing_type);

    NetCDFType type() const { return m_type; }
    NetCDFElementaryType basic_type() const;
    const std::vector<VariableDimension>& dimensions() const { return m_dimensions; };
    bool is_scalar() const
    {
        // TODO: also scalar when we have only one dimension that has size 1
        return m_dimensions.empty();
    }

  private:
    std::optional<VariableData> m_value;
    NetCDFType m_type{NetCDFElementaryType::Default};
    std::vector<VariableDimension> m_dimensions{};
};

struct VariableDeclaration
{
    using VariableDeclarationType = std::variant<Variable, Attribute>;

    std::string description(int indent) const;

    static std::optional<VariableDeclarationType> parse(Parser&, std::optional<NetCDFType> existing_type);
};

class Variables : public Element
{
  public:
    std::string description(int indent) const override;

    static void parse(Parser&, Group&);

    std::vector<Variable>& variables() { return m_variables; }
    const std::vector<Variable>& variables() const { return m_variables; }
    std::vector<Attribute>& attributes() { return m_attributes; }
    const std::vector<Attribute>& attributes() const { return m_attributes; }

  private:
    std::vector<Variable> m_variables{};
    std::vector<Attribute> m_attributes{};
};

struct VariableSection
{
    static void parse(Parser& parser, Group& group);
};

class Group : public Element
{
  public:
    std::string description(int indent) const override;

    static std::optional<Group> parse(Parser&);

    const std::vector<ComplexType>& types() const { return m_types; }
    std::vector<Variable>& variables();
    const std::vector<Variable>& variables() const;
    std::vector<Attribute>& attributes();
    const std::vector<Attribute>& attributes() const;
    const std::vector<Dimension>& dimensions() const;
    std::vector<Dimension>& dimensions();
    const std::vector<Group>& groups() const { return m_groups; };
    std::vector<Group>& groups() { return m_groups; };

  private:
    std::vector<ComplexType> m_types{};
    std::optional<Dimensions> m_dimensions{};
    std::optional<Variables> m_variables{};
    std::vector<Group> m_groups{};
};

struct RootGroup
{
    void print_tree();

    std::string description(int indent) const;

    static std::optional<RootGroup> parse(Parser&);

    std::unique_ptr<Group> group{};
};

} // namespace ncdlgen
