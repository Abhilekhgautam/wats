#include "type.hpp"

std::string Type::to_string(){
    switch(kind){
        case TypeKind::I32    :  {return "i32";}
        case TypeKind::I64    :  {return "i64";}
        case TypeKind::F32    :  {return "f32";}
        case TypeKind::F64    :  {return "f64";}
        case TypeKind::String :  {return "string";}
        case TypeKind::Var    :  {return "unknown";}
    }
}

std::string Type::to_string() const {
    switch(kind){
        case TypeKind::I32    :  {return "i32";}
        case TypeKind::I64    :  {return "i64";}
        case TypeKind::F32    :  {return "f32";}
        case TypeKind::F64    :  {return "f64";}
        case TypeKind::String :  {return "string";}
        case TypeKind::Var    :  {return "unknown";}
    }
}
