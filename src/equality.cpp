

#include "equality.h"
#include "utils.h"

#include <fmt/core.h>

namespace ncdlgen
{

bool operator==(const EnumValue& value, const EnumValue& other_value)
{
    return value.name == other_value.name && value.value == other_value.value;
}

bool operator==(const Dimension& value, const Dimension& other_value)
{
    return value.name == other_value.name && value.length == other_value.length;
}

bool operator==(const ComplexType& type, const ComplexType& other_type)
{
    // Iterate through all contained types of both argument types
    return std::visit(
        [&other_type](auto&& own_type) -> bool {
            return std::visit(
                [&own_type](auto&& other_type) {
                    using OwnType = std::decay_t<decltype(own_type)>;
                    using OtherType = std::decay_t<decltype(other_type)>;

                    // If the underlying types do not match, the argument types do not match
                    if constexpr (!std::is_same_v<OwnType, OtherType>)
                    {
                        return false;
                    }
                    else
                    {
                        // Note: This requires that the underlying types have
                        // a corresponding operator==() defined
                        return own_type == other_type;
                    }
                },
                other_type.type);
        },
        type.type);
}

bool operator==(const NetCDFType& type, const NetCDFType& other_type)
{
    // Iterate through all contained types of both argument types
    return std::visit(
        [&other_type](auto&& own_type) -> bool {
            return std::visit(
                [&own_type](auto&& other_type) {
                    using OwnType = std::decay_t<decltype(own_type)>;
                    using OtherType = std::decay_t<decltype(other_type)>;

                    // If the underlying types do not match, the argument types do not match
                    if constexpr (!std::is_same_v<OwnType, OtherType>)
                    {
                        return false;
                    }
                    else
                    {
                        // Note: This requires that the underlying types have
                        // a corresponding operator==() defined
                        return own_type == other_type;
                    }
                },
                other_type.type);
        },
        type.type);
}

bool operator==(const OpaqueType& type, const OpaqueType& other_type)
{
    return type.length == other_type.length && type.name == other_type.name;
}

bool operator==(const VLenType& type, const VLenType& other_type)
{
    return type.type == other_type.type && type.name == other_type.name;
}

bool operator==(const EnumType& type, const EnumType& other_type)
{
    if (type.enum_values.size() != other_type.enum_values.size())
    {
        return false;
    }
    bool values_equal{true};
    for (size_t i = 0; i < type.enum_values.size(); i++)
    {
        values_equal &= type.enum_values[i] == other_type.enum_values[i];
    }
    return values_equal && type.type == other_type.type && type.name == other_type.name;
}

bool operator==(const ArrayType& type, const ArrayType& other_type)
{
    if (!(type.type == other_type.type && type.name == other_type.name &&
          type.dimensions.dimensions.size() == other_type.dimensions.dimensions.size()))
    {
        return false;
    }
    for (size_t i = 0; i < type.dimensions.dimensions.size(); i++)
    {
        if (type.dimensions.dimensions[i] == other_type.dimensions.dimensions[i])
        {
            continue;
        }
        return false;
    }
    return true;
}

bool operator==(const CompoundType& type, const CompoundType& other_type)
{
    bool field_ok = type.name == other_type.name && type.type_names.size() == other_type.type_names.size();
    if (!field_ok)
    {
        return false;
    }
    bool types_ok{true};
    for (size_t i = 0; i < type.type_names.size(); i++)
    {
        types_ok &= type.type_names[i] == other_type.type_names[i];
        types_ok &= type.types[i] == other_type.types[i];
    }
    return types_ok;
}

bool operator!=(const NetCDFType& type, const NetCDFType& other_type) { return !(type == other_type); }
bool operator!=(const ComplexType& type, const ComplexType& other_type) { return !(type == other_type); }
bool operator!=(const EnumValue& value, const EnumValue& other_value) { return !(value == other_value); }
bool operator!=(const Dimension& value, const Dimension& other_value) { return !(value == other_value); }
bool operator!=(const OpaqueType& type, const OpaqueType& other_type) { return !(type == other_type); }
bool operator!=(const VLenType& type, const VLenType& other_type) { return !(type == other_type); }
bool operator!=(const EnumType& type, const EnumType& other_type) { return !(type == other_type); }
bool operator!=(const ArrayType& type, const ArrayType& other_type) { return !(type == other_type); }
bool operator!=(const CompoundType& type, const CompoundType& other_type) { return !(type == other_type); }

} // namespace ncdlgen
