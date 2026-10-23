// created by void at 2026-10-22 23:04:51
#include "typeinferenceengine.hpp"
#include "type.hpp"

Type TypeInferenceEngine::freshVar(){
    id += 1;

    Type newType = Type{TypeKind::Var, id};

    return newType;
}

Type TypeInferenceEngine::resolve(Type& t){
    Type type = t;
    while(type.kind == TypeKind::Var){
        if(auto id = id2Type.find(type.id); id != id2Type.end()){
           type = id2Type[id->first];
        }
        else break;
    }

    return type;
}

std::optional<UnifyError> TypeInferenceEngine::unify(Type& first, Type& second){
    Type first_resolved  = resolve(first);
    Type second_resolved = resolve(second);

    if (first_resolved == second_resolved) return {};

    if (first_resolved.kind == TypeKind::Var){
        id2Type[first.id] = second;
        return {};
    }
    
    if (second_resolved.kind == TypeKind::Var){
        id2Type[second.id] = second;
        return {};
    }
    
    return UnifyError::Mismatch;
}
