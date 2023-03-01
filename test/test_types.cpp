#include <gtest/gtest.h>
#include <string>

#include "equality.h"

using namespace ncdlgen;

TEST(types, opaque)
{
    auto ref_type = OpaqueType("test_opaque", 1);
    auto wrong = OpaqueType("test_opaqu", 1);
    auto wrong2 = OpaqueType("test_opaque", 2);
    auto correct_type = OpaqueType("test_opaque", 1);

    ASSERT_EQ(ref_type, correct_type);
    ASSERT_NE(ref_type, wrong);
    ASSERT_NE(ref_type, wrong2);
}

TEST(types, vlen)
{
    auto ref_type = VLenType("test_vlen", NetCDFElementaryType::Int64);
    auto wrong = VLenType("test_vl", NetCDFElementaryType::Int64);
    auto wrong2 = VLenType("test_vlen", NetCDFElementaryType::Char);
    auto correct_type = VLenType("test_vlen", NetCDFElementaryType::Int64);

    ASSERT_EQ(ref_type, correct_type);
    ASSERT_NE(ref_type, wrong);
    ASSERT_NE(ref_type, wrong2);
}

TEST(types, complex)
{
    auto vlen_type = VLenType("vlen", NetCDFElementaryType::Int64);
    auto opaque_type = OpaqueType("opaque", 1);
    auto wrong_vlen = VLenType("vlen_wrong", NetCDFElementaryType::Char);
    auto wrong_opaque = VLenType("opaque_wrong", NetCDFElementaryType::Char);

    auto vlen_correct = ComplexType(vlen_type);
    auto opaque_correct = ComplexType(opaque_type);

    // comparing vlen_type to vlen_type
    ASSERT_EQ(ComplexType(vlen_type), ComplexType(vlen_type));
    ASSERT_EQ(NetCDFType(ComplexType(vlen_type)), NetCDFType(ComplexType(vlen_type)));

    // comparing vlen_type to wrong_vlen
    ASSERT_NE(ComplexType(vlen_type), ComplexType(wrong_vlen));
    ASSERT_NE(NetCDFType(ComplexType(vlen_type)), NetCDFType(ComplexType(wrong_vlen)));

    // comparing opauqe_type to opauqe_type
    ASSERT_EQ(ComplexType(opaque_type), ComplexType(opaque_type));
    ASSERT_EQ(NetCDFType(ComplexType(opaque_type)), NetCDFType(ComplexType(opaque_type)));

    // comparing opaque_type to wrong_opaque
    ASSERT_NE(ComplexType(opaque_type), ComplexType(wrong_opaque));
    ASSERT_NE(NetCDFType(ComplexType(opaque_type)), NetCDFType(ComplexType(wrong_opaque)));

    // Comparing opaque to vlen
    ASSERT_NE(ComplexType(opaque_type), ComplexType(vlen_type));
    ASSERT_NE(NetCDFType(ComplexType(opaque_type)), NetCDFType(ComplexType(vlen_type)));
    ASSERT_NE(ComplexType(vlen_type), ComplexType(opaque_type));
    ASSERT_NE(NetCDFType(ComplexType(vlen_type)), NetCDFType(ComplexType(opaque_type)));
}

// TODO: tests for enum, compound, array types missing
