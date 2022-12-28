
#include <unordered_set>

#include "syntax.h"

namespace ncdlgen
{

bool is_keyword(const std::string_view word)
{
    static std::unordered_set<std::string_view> keywords{};
    keywords.insert("variables:");
    keywords.insert("dimensions:");
    keywords.insert("types:");
    keywords.insert("data:");
    keywords.insert("group:");
    return keywords.find(word) != keywords.end();
}

std::optional<NetCDFElementaryType> type_for_token(const Token& token)
{
    if (token.content() == "char")
    {
        return NetCDFElementaryType::Char;
    }
    else if (token.content() == "byte")
    {
        return NetCDFElementaryType::Byte;
    }
    else if (token.content() == "ubyte")
    {
        return NetCDFElementaryType::Ubyte;
    }
    else if (token.content() == "short")
    {
        return NetCDFElementaryType::Short;
    }
    else if (token.content() == "ushort")
    {
        return NetCDFElementaryType::Ushort;
    }
    else if (token.content() == "int")
    {
        return NetCDFElementaryType::Int;
    }
    else if (token.content() == "uint")
    {
        return NetCDFElementaryType::Uint;
    }
    else if (token.content() == "long")
    {
        return NetCDFElementaryType::Long;
    }
    else if (token.content() == "int64")
    {
        return NetCDFElementaryType::Int64;
    }
    else if (token.content() == "uint64")
    {
        return NetCDFElementaryType::Uint64;
    }
    else if (token.content() == "float")
    {
        return NetCDFElementaryType::Float;
    }
    else if (token.content() == "real")
    {
        return NetCDFElementaryType::Real;
    }
    else if (token.content() == "double")
    {
        return NetCDFElementaryType::Double;
    }
    else if (token.content() == "string")
    {
        return NetCDFElementaryType::String;
    }
    else
    {
        return NetCDFElementaryType::Default;
    }
}

const std::string_view name_for_type(const NetCDFElementaryType& type)
{
    switch (type)
    {
    case NetCDFElementaryType::Char:
        return "char";
    case NetCDFElementaryType::Byte:
        return "byte";
    case NetCDFElementaryType::Ubyte:
        return "ubyte";
    case NetCDFElementaryType::Short:
        return "short";
    case NetCDFElementaryType::Ushort:
        return "ushort";
    case NetCDFElementaryType::Int:
        return "int";
    case NetCDFElementaryType::Uint:
        return "uint";
    case NetCDFElementaryType::Long:
        return "long";
    case NetCDFElementaryType::Int64:
        return "int64";
    case NetCDFElementaryType::Uint64:
        return "uint64";
    case NetCDFElementaryType::Float:
        return "float";
    case NetCDFElementaryType::Real:
        return "real";
    case NetCDFElementaryType::Double:
        return "double";
    case NetCDFElementaryType::String:
        return "string";
    default:
        return "unknown";
    }
}

} // namespace ncdlgen