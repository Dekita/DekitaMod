#include "Utils.hpp"

namespace DekitaRPG {
    /**
    * [public] IsValidUObject()
    * checks if the given uobject is valid or not :)
    */
    bool IsValidUObject(Unreal::UObject* Object) {
        //return Object && !Object->IsUnreachable(); 
        return Object && Unreal::UObjectArray::IsValid(Object->GetObjectItem(), false);
    }

};// namespace

