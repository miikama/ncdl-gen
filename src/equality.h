
#pragma once

#include "types.h"

namespace ncdlgen
{

bool operator==(const EnumValue& value, const EnumValue& other_value);
bool operator!=(const EnumValue& value, const EnumValue& other_value);
bool operator==(const Dimension& value, const Dimension& other_value);
bool operator!=(const Dimension& value, const Dimension& other_value);

bool operator==(const NetCDFType& type, const NetCDFType& other_type);
bool operator!=(const NetCDFType& type, const NetCDFType& other_type);
bool operator==(const ComplexType& type, const ComplexType& other_type);
bool operator!=(const ComplexType& type, const ComplexType& other_type);
bool operator==(const OpaqueType& type, const OpaqueType& other_type);
bool operator!=(const OpaqueType& type, const OpaqueType& other_type);
bool operator==(const VLenType& type, const VLenType& other_type);
bool operator!=(const VLenType& type, const VLenType& other_type);
bool operator==(const EnumType& type, const EnumType& other_type);
bool operator!=(const EnumType& type, const EnumType& other_type);
bool operator==(const ArrayType& type, const ArrayType& other_type);
bool operator!=(const ArrayType& type, const ArrayType& other_type);
bool operator==(const CompoundType& type, const CompoundType& other_type);
bool operator!=(const CompoundType& type, const CompoundType& other_type);

} // namespace ncdlgen
