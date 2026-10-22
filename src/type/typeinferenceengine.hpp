#ifndef WATS_TYPE_INFERENCE_ENGINE
#define WATS_TYPE_INFERENCE_ENGINE

#include <cstdint>
#include <unordered_map>
#include <optional>

#include "type.hpp"

enum struct UnifyError{
    Mismatch,
    OccurCheck    
};

class TypeInferenceEngine {
private:
    std::unordered_map<std::uint32_t, Type> id2Type; 
    std::uint32_t id = 0;
public:
    Type freshVar();
    Type resolve(Type& t);
    std::optional<UnifyError> unify(Type&, Type&);
};

#endif
