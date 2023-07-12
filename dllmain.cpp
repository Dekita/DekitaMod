/**
* system: dllmain.cpp
* reason: an example/test for ue4ss c++ mods
* author: dekitarpg@gmail.com
*/

/**
* Lots of includes just to make life easaier for testing <3
*/
#include <stdio.h>
#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <ExceptionHandling.hpp>

#pragma warning(disable: 4005)
#include <UE4SSProgram.hpp>
#include <GUI/Dumpers.hpp>
#include <USMapGenerator/Generator.hpp>
#include <Unreal/UnrealVersion.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/UnrealVersion.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/FProperty.hpp>
#include <Unreal/UPackage.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/UAssetRegistry.hpp>
#include <Unreal/UAssetRegistryHelpers.hpp>
#include <Unreal/UGameViewportClient.hpp>
#include <Unreal/PackageName.hpp>
#include <Unreal/TypeChecker.hpp>
#include <Unreal/FFrame.hpp>
#include <Unreal/FOutputDevice.hpp>
#include <Unreal/Property/NumericPropertyTypes.hpp>
#include <Unreal/Property/FObjectProperty.hpp>
#include <Unreal/Property/FWeakObjectProperty.hpp>
#include <Unreal/Property/FClassProperty.hpp>
#include <Unreal/Property/FArrayProperty.hpp>
#include <Unreal/Property/FMapProperty.hpp>
#include <Unreal/Property/FBoolProperty.hpp>
#include <Unreal/Property/FNameProperty.hpp>
#include <Unreal/Property/FStructProperty.hpp>
#include <Unreal/Property/FEnumProperty.hpp>
#include <Unreal/Property/FTextProperty.hpp>
#include <Unreal/Property/FStrProperty.hpp>
#include <UnrealCustom/CustomProperty.hpp>
#include <Unreal/PrimitiveTypes.hpp>
#include <Unreal/UKismetSystemLibrary.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/UScriptStruct.hpp>
#include <Unreal/UStruct.hpp>
#include <Unreal/UFunctionStructs.hpp>
#include <Unreal/FString.hpp>
#include <Unreal/AActor.hpp>
#pragma warning(default: 4005)

using namespace RC;

/**
* DekitaRPG
* main mod logic class
*/
class DekitaRPG {
public:
    /**
    * OnUnrealInitialized()
    * called when unreal engine initializes
    * able to use the 'Unreal' namespace in this function and anywhere else after this function has fired.
    */
    static void OnUnrealInitialized() {
        Unreal::Hook::RegisterBeginPlayPostCallback(OnPostBeginPlay);
        Unreal::Hook::RegisterProcessLocalScriptFunctionPostCallback(OnPostFunctionCall);
    }
    /**
    * OnProgramStarted()
    * called when the application properly begins
    */
    static void OnProgramStarted() {
        ScanForDekitaActor();
    }
    /**
    * OnUpdate()
    * called during each update
    */
    static void OnUpdate() {

    }
    /**
    * OnPostBeginPlay()
    * called after begin play runs for every AActor
    */
    static void OnPostBeginPlay([[maybe_unused]] Unreal::AActor* Context) {
        TRY([&] {
            Output::send<LogLevel::Verbose>(STR("BeginPlay: {}\n"), Context->GetNamePrivate().ToString());
        });
    }
    /**
    * OnPostFunctionCall()
    * called after each and every function call...
    * so make sure we only hook the functions we want.
    */
    static void OnPostFunctionCall([[maybe_unused]] Unreal::UObject* Context, Unreal::FFrame& Stack, [[maybe_unused]] void* RESULT_DECL) {
        TRY([&] {
            auto name = Stack.Node()->GetName();
            if (name == STR("PrintToModLoader")) {
                //OnPrintToModLoader(Context, Stack, RESULT_DECL);
            } 
            else if (name == STR("GetOrCreateWidget")) {
                OnGetOrCreateWidget(Context, Stack, RESULT_DECL);
            }
            else if (name == STR("DekCppTestReturnChange")) {
                OnDekCppTestReturnChange(Context, Stack, RESULT_DECL);
            } 
            else if (name == STR("DekCppTestStruct")) {
                OnDekCppTestStruct(Context, Stack, RESULT_DECL);
            }
        });
    }
    /**
    * OnPrintToModLoader()
    * called when PrintToModLoader is called
    * intercepts the message and reprints it to the console
    * just an example for reading the params of the hooked function
    */
    static void OnPrintToModLoader([[maybe_unused]] Unreal::UObject* Context, Unreal::FFrame& Stack, [[maybe_unused]] void* RESULT_DECL) {
        TRY([&] {
            auto node = Stack.Node();
            // expected function parameters
            struct FunctionParams {
                Unreal::FString Message;
            };
            // cast Stack.Locals() to expected params struct
            auto* params = reinterpret_cast<FunctionParams*>(Stack.Locals());
            // log info from params to make sure its being read properly
            Output::send<LogLevel::Verbose>(STR("\nThe Message?:: {}\n"), params->Message.GetCharArray());
        });
    }
    /**
    * OnGetOrCreateWidget()
    * called when GetOrCreateWidget is called. 
    * this is a function from my HogwartsLegacy UICore mod.
    * another example for reading input params of hooked function
    */
    static void OnGetOrCreateWidget([[maybe_unused]] Unreal::UObject* Context, Unreal::FFrame& Stack, [[maybe_unused]] void* RESULT_DECL) {
        TRY([&] {
            auto node = Stack.Node();
            // expected function parameters
            struct FunctionParams {
                Unreal::UWidgetBlueprintGeneratedClass& WidgetClass;
                bool TopLevelOnly;
            };
            // cast Stack.Locals() to expected params struct
            auto* params = reinterpret_cast<FunctionParams*>(Stack.Locals());
            // log info from params to make sure its being read properly
            Output::send<LogLevel::Verbose>(STR("Context->GetFullName(): {}\n"), Context->GetFullName());
            Output::send<LogLevel::Verbose>(STR("params->WidgetClass.GetName(): {}\n"), params->WidgetClass.GetName());
            Output::send<LogLevel::Verbose>(STR("params->WidgetClass.GetPathName(): {}\n"), params->WidgetClass.GetPathName());
            Output::send<LogLevel::Verbose>(STR("params->TopLevelOnly: {}\n"), params->TopLevelOnly);
        });
    }

    /**
    * OnDekCppTestReturnChange()
    * Called when the blueprint function DekCppTestReturnChange is called from test mod
    * an example for reading input and then editing the return parameters
    */
    static void OnDekCppTestReturnChange([[maybe_unused]] Unreal::UObject* Context, Unreal::FFrame& Stack, [[maybe_unused]] void* RESULT_DECL) {
        TRY([&] {
            auto node = Stack.Node();
            // check if this function expects a return value
            //auto expects_return = CheckExpectsReturn(node);

            // cast locals to functions expected input params
            struct FunctionParams {
                bool BoolIn1;
                bool BoolIn2;
                Unreal::FString StringInput;
            };
            auto* params = reinterpret_cast<FunctionParams*>(Stack.Locals());

            Output::send<LogLevel::Verbose>(STR("params->BoolIn1: {}\n"), params->BoolIn1);
            Output::send<LogLevel::Verbose>(STR("params->BoolIn2: {}\n"), params->BoolIn2);
            Output::send<LogLevel::Verbose>(STR("params->StringInput: {}\n"), params->StringInput.GetCharArray());

            // set output parameters based on name
            auto OutParms = Stack.OutParms();
            SetReturnParameter(OutParms, L"BoolOut1", true);
            SetReturnParameter(OutParms, L"BoolOut2", false);
            SetReturnParameter(OutParms, L"BoolOut3", true);
            SetReturnParameter(OutParms, L"StringOutput", Unreal::FString(L"I'm the altered string yo!"));

            Output::send<LogLevel::Verbose>(STR("altered return params!\n"));
        });
    }
    /**
    * OnDekCppTestStruct()
    * an example for reading input and then editing the return parameters - now with added structs!!
    */
    static void OnDekCppTestStruct([[maybe_unused]] Unreal::UObject* Context, Unreal::FFrame& Stack, [[maybe_unused]] void* RESULT_DECL) {
        TRY([&] {
            auto node = Stack.Node();

            // cast locals to functions expected input params
            struct StructParams {
                bool StructBool { false };
                int StructInt { 0 };
                Unreal::FString StructString;
            };
            struct FunctionParams {
                // Unreal::UScriptStruct StructInput;
                StructParams StructInput;
                bool BoolInput;
            };
            auto* params = reinterpret_cast<FunctionParams*>(Stack.Locals());
            
            Output::send<LogLevel::Verbose>(STR("params->StructInput.StructInt: {}\n"), params->StructInput.StructInt);
            Output::send<LogLevel::Verbose>(STR("params->StructInput.StructBool: {}\n"), params->StructInput.StructBool);
            Output::send<LogLevel::Verbose>(STR("params->StructInput.StructString: {}\n"), params->StructInput.StructString.GetCharArray());

            // create struct for sending back to ue
            StructParams StructOutput;
            StructOutput.StructBool = true;
            StructOutput.StructInt = 1;
            StructOutput.StructString = Unreal::FString(L"Im from c++");

            // set output parameters based on name
            auto OutParms = Stack.OutParms();
            SetReturnParameter(OutParms, L"BoolOutput", false); // default is true in ue
            SetReturnParameter(OutParms, L"StructOutput", StructOutput);
        });
    }
    /**
    * SetReturnParameter()
    * an example for how to set the return value for a hooked function
    */
    template <class T> static void SetReturnParameter(Unreal::FOutParmRec* OutParms, const wchar_t* ParamName, const T& NewValue) {
        // ensure FOutParmRec has expected properties
        struct FOutParmRecExtended : Unreal::FOutParmRec {
            Unreal::FProperty* Property;
            Unreal::uint8* PropAddr;
            Unreal::FOutParmRec* NextOutParm;
        };
        // iterate over outparms to set the deired param's pointer's value to newvalue
        auto CurrentOutParam = reinterpret_cast<FOutParmRecExtended*>(OutParms);
        while (CurrentOutParam && CurrentOutParam->Property) {
            // make sure that we only edit the correct property
            if (CurrentOutParam->Property->GetName() == ParamName) {
                // cast CurrentOutParam to NewValue's T and set the value
                *reinterpret_cast<T*>(CurrentOutParam->PropAddr) = NewValue;
            }
            // set CurrentOutParam for the next iteration / or stop the loop if nullptr
            CurrentOutParam = reinterpret_cast<FOutParmRecExtended*>(CurrentOutParam->NextOutParm);
        }
    }
    /**
    * ScanForDekitaUI()
    * scans for a specific instance of an actor and print some info about it
    */
    static void ScanForDekitaActor() {
        auto dekname = STR("/Game/DekitaRPG/UICore/UICoreLogic.UICoreLogic_C");
        auto Object = Unreal::UObjectGlobals::StaticFindObject(nullptr, nullptr, dekname);
        if (Object) {
            Output::send<LogLevel::Verbose>(STR("UICoreLogic.GetName(): {}\n"), Object->GetName());
            Output::send<LogLevel::Verbose>(STR("UICoreLogic.GetFullName(): {}\n"), Object->GetFullName());
            Output::send<LogLevel::Verbose>(STR("UICoreLogic.GetPathName(): {}\n"), Object->GetPathName());
        }
    }
    /**
    * CheckExpectsReturn()
    * checks if a UFunction has the OutParms flag
    */
    static bool CheckExpectsReturn(Unreal::UFunction* node) {
        auto return_flag = Unreal::EFunctionFlags::FUNC_HasOutParms;
        return (node->GetFunctionFlags() & return_flag) != 0;
    }
};

/**
* DekitaMod:
* UE4SS c++ mod class defintion
*/
class DekitaMod : public RC::CppUserModBase {
public:
    // constructor
    DekitaMod() : CppUserModBase() {
        ModName = STR("DekitaRPG");
        ModVersion = STR("0.01");
        ModDescription = STR("Example mod to learn UE4SS C++ modding basics");
        ModAuthors = STR("DekitaRPG@gmail.com");
        Output::send<LogLevel::Verbose>(STR("created {}!\n"), ModName);
    }
    // destructor
    ~DekitaMod() { 
        // fill when required
    }
    // You are allowed to use the 'Unreal' namespace in this function and anywhere else after this function has fired.
    auto on_unreal_init() -> void override {
        Output::send<LogLevel::Verbose>(STR("{} unreal initialized!\n"), ModName);
        DekitaRPG::OnUnrealInitialized();
    }
    // called after everything has fully initialized
    auto on_program_start() -> void override {
        Output::send<LogLevel::Verbose>(STR("{} program started!\n"), ModName);
        DekitaRPG::OnProgramStarted();
    }
    // called each update
    auto on_update() -> void override {
        DekitaRPG::OnUpdate();
    }
};

/**
* export the start_mod() and uninstall_mod() functions to 
* be used by the core ue4ss system to load in our dll mod
*/
#define DEKITA_MOD_API __declspec(dllexport) 
extern "C" {
    DEKITA_MOD_API RC::CppUserModBase* start_mod() { return new DekitaMod(); }
    DEKITA_MOD_API void uninstall_mod(RC::CppUserModBase* mod) { delete mod; }
}