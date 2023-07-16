#pragma once

#include "Utils.hpp"

namespace DekitaRPG {

    /*
    * IFaceUFunkMapItem 
    * simple struct to store function and context for storing in function map
    */
    struct IFaceUFunkMapItem {
        Unreal::UFunction* Function;
        Unreal::AActor* Context;
    };

    /**
    * IFaceUFunkMap
    * defines the custom map of string to IFaceUFunkMapItem vector
    */
    typedef std::map<std::wstring, std::vector<IFaceUFunkMapItem>> IFaceUFunkMap;

    /**
    * IFace
    * class to handle hooking actors that implement a specific bp interface
    */
    class IFace {
        public:
            // hook function for RegisterBeginPlayPostCallback 
            static void Hook(Unreal::AActor*);

            // an example to propogate a function to all actors 'hooked'
            static void PropogateTestFunction();

        private:
            // IFaceUFunkMap for all hooked functions
            static IFaceUFunkMap g_ContextItemMap;

            // add, remove, and iterate the context items 
            static void AddContextItem(Unreal::UFunction*, Unreal::AActor*);
            static void RemoveContextItems(Unreal::AActor*);
            static void Iterate(std::wstring, void*);

            // EndPlay hooks for any actor that is 'hooked'
            static void OnPreEndPlayHook(Unreal::UnrealScriptFunctionCallableContext&, void*);
            static void OnPostEndPlayHook(Unreal::UnrealScriptFunctionCallableContext&, void*);

    };// class
} // namespace