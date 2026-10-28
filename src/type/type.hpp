#ifndef WATS_TYPE_HPP
#define WATS_TYPE_HPP

#include <cstdint>
#include <string>

enum struct TypeKind : std::uint8_t{
    I32,
    I64,
    F32,
    F64,
    String,
    Var
};

class Type {
public:
   TypeKind kind;
   std::uint32_t id = 0;

   Type() = default;
   Type(TypeKind k): kind(k){}
   Type(TypeKind k, std::uint32_t id) : kind(k), id(id){}

   bool operator==(const Type&) const = default;

   std::string to_string();
   std::string to_string() const;
};

template <>
struct std::hash<Type> {
    std::size_t operator()(const Type& t) const noexcept {
        return std::hash<TypeKind>{}(t.kind);
    }
};

#endif
