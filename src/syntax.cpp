
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

std::optional<NetCDFType> type_for_token(const Token &token)
{
    if (token.content() == "char")
    {
        return NetCDFType::Char;
    }
    else if (token.content() == "byte")
    {
        return NetCDFType::Byte;
    }
    else if (token.content() == "ubyte")
    {
        return NetCDFType::Ubyte;
    }
    else if (token.content() == "short")
    {
        return NetCDFType::Short;
    }
    else if (token.content() == "ushort")
    {
        return NetCDFType::Ushort;
    }
    else if (token.content() == "int")
    {
        return NetCDFType::Int;
    }
    else if (token.content() == "uint")
    {
        return NetCDFType::Uint;
    }
    else if (token.content() == "long")
    {
        return NetCDFType::Long;
    }
    else if (token.content() == "int64")
    {
        return NetCDFType::Int64;
    }
    else if (token.content() == "uint64")
    {
        return NetCDFType::Uint64;
    }
    else if (token.content() == "float")
    {
        return NetCDFType::Float;
    }
    else if (token.content() == "real")
    {
        return NetCDFType::Real;
    }
    else if (token.content() == "double")
    {
        return NetCDFType::Double;
    }
    else if (token.content() == "string")
    {
        return NetCDFType::String;
    }
    else
    {
        return {};
    }
}

const std::string_view name_for_type(const NetCDFType &type)
{
    switch (type)
    {
    case NetCDFType::Char:
        return "char";
    case NetCDFType::Byte:
        return "byte";
    case NetCDFType::Ubyte:
        return "ubyte";
    case NetCDFType::Short:
        return "short";
    case NetCDFType::Ushort:
        return "ushort";
    case NetCDFType::Int:
        return "int";
    case NetCDFType::Uint:
        return "uint";
    case NetCDFType::Long:
        return "long";
    case NetCDFType::Int64:
        return "int64";
    case NetCDFType::Uint64:
        return "uint64";
    case NetCDFType::Float:
        return "float";
    case NetCDFType::Real:
        return "real";
    case NetCDFType::Double:
        return "double";
    case NetCDFType::String:
        return "string";
    default:
        return "unknown";
    }
}

} // namespace ncdlgen