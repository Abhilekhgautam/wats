// created by void at 2026-10-23 12:51:11
#include "typetable.hpp"
#include "type.hpp"

TypeTable::TypeTable(){
    names["i32"] = TypeKind::I32;
    names["i64"] = TypeKind::I64;
    names["f32"] = TypeKind::F32;
    names["f64"] = TypeKind::F64;
}

std::optional<Type> TypeTable::resolve(const std::string& name){
    if(auto it = names.find(name); it != names.end()){
        return it->second; 
    }
    
    return {};
}
