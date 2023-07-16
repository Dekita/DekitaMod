#pragma once

#include "Utils.hpp"
#include "IFace.hpp"

namespace DekitaRPG {
    /**
    * BPFL (Blueprint Function Library Function Implementation)
    */
    class BPFL {

    public:
        // hook function for RegisterProcessLocalScriptFunctionPostCallback
        static DefineUnrealScriptFunction(Hook);

    private:
        // actual bp functions
        static DefineUnrealScriptFunction(OnLogToUE4SS);
        static DefineUnrealScriptFunction(OnOpenLink);
        // test functions
        static DefineUnrealScriptFunction(OnGetOrCreateWidget);
        static DefineUnrealScriptFunction(OnTestReturnBasic);
        static DefineUnrealScriptFunction(OnTestReturnStruct);
        // helper function to log information about the current node and context
        static DefineUnrealScriptFunction(LogFunctionInformation);

        // helper functions to get input/set output parameters
        template <class T> static T* GetInputParameters(Unreal::FFrame&);
        template <class T> static void SetReturnParameter(Unreal::FOutParmRec*, const wchar_t*, const T&);

        // additional helper functions
        static bool IsValidFunctionClass(Unreal::UObject*);
        static bool CheckExpectsReturn(Unreal::UFunction*);
        static void ScanForDekitaActor();

    };// class 
} // namespace 
