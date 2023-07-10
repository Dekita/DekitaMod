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
#pragma warning(default: 4005)

using namespace RC;

/**
*
*
*/
class DekitaRPG {
public:
    /**
    *
    *
    */
    static void OnUnrealInitialized() {
        SetupMainBindings();
        ScanForDekitaUI();
    }
    /**
    *
    *
    */
    static void OnProgramStarted() {
        ScanForDekitaUI();
    }
    /**
    *
    *
    */
    static void OnUpdate() {

    }
    /**
    *
    *
    */
    static void SetupMainBindings(){
        Unreal::Hook::RegisterBeginPlayPostCallback(&OnPostBeginPlay);
        Unreal::Hook::RegisterProcessLocalScriptFunctionPostCallback(&OnPostFunctionCall);

        auto funkname = STR("DekCppTestingFunction");

        // Unreal::UnrealScriptFunctionCallable
        //auto prehook = [](Unreal::UnrealScriptFunctionCallableContext& Context, void* CustomData) {
        //    Output::send<LogLevel::Verbose>(STR("prehook\n"));
        //};
        //auto posthook = [](Unreal::UnrealScriptFunctionCallableContext& Context, void* CustomData) {
        //    Output::send<LogLevel::Verbose>(STR("posthook\n"));
        //};

        //Unreal::UObjectGlobals::RegisterHook(funkname, prehook, posthook, nullptr);
    }
    /**
    *
    *
    */
    static void OnPostBeginPlay([[maybe_unused]] Unreal::AActor* Context) {
        TRY([&] {
            // Output::send<LogLevel::Verbose>(STR("Context->GetFullName: {}\n"), Context->GetFullName());
        });
    }
    /**
    *
    *
    */
    static void ScanForDekitaUI() {
        auto Object = Unreal::UObjectGlobals::StaticFindObject(nullptr, nullptr, STR("/Game/DekitaRPG/UICore/UICoreLogic.UICoreLogic_C"));
        Output::send<LogLevel::Verbose>(STR("UICoreLogic Object Name: {}\n"), Object->GetFullName());
    }
    /**
    *
    *
    */
    static void OnPostFunctionCall([[maybe_unused]] Unreal::UObject* Context, Unreal::FFrame& Stack, [[maybe_unused]] void* RESULT_DECL) {
        TRY([&] {
            auto node = Stack.Node();
            auto name = node->GetName();
            if (name == STR("PrintToModLoader")) {
                OnPrintToModLoader(Context, Stack, RESULT_DECL);
            } else if (name == STR("GetOrCreateWidget")) {
                OnGetOrCreateWidget(Context, Stack, RESULT_DECL);
            } else if (name == STR("DekCppTestingFunction")) {
                OnDekCppTestingFunction(Context, Stack, RESULT_DECL);
            }
        });
    }
    /**
    *
    *
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
            //Output::send<LogLevel::Verbose>(STR("\nThe Message?:: {}\n"), params->Message.GetCharArray());
            // get return value offset: 
            auto return_value_offset = node->GetReturnValueOffset();
            auto has_return_value = return_value_offset != 0xFFFF;
            // check if this function expects a return value
            auto expects_return = CheckExpectsReturn(node);
            //Output::send<LogLevel::Verbose>(STR("expects_return: {} -- return_offset: {} - {}\n"), expects_return, has_return_value, return_value_offset);
        });
    }

    static bool CheckExpectsReturn(Unreal::UFunction* node) {
        auto return_flag = Unreal::EFunctionFlags::FUNC_HasOutParms;
        return (node->GetFunctionFlags() & return_flag) != 0;
    }


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
            Output::send<LogLevel::Verbose>(STR("OnGetOrCreateWidget->GetFullName(): {}\n"), Context->GetFullName());
            Output::send<LogLevel::Verbose>(STR("TopLevelOnly: {}\n"), params->TopLevelOnly);
            Output::send<LogLevel::Verbose>(STR("params->ObjectInput.GetName(): {}\n"), params->WidgetClass.GetName());
            Output::send<LogLevel::Verbose>(STR("params->ObjectInput.GetPathName(): {}\n"), params->WidgetClass.GetPathName());

            // get return value offset: 
            auto return_value_offset = node->GetReturnValueOffset();
            auto has_return_value = return_value_offset != 0xFFFF;
            // check if this function expects a return value
            auto expects_return = CheckExpectsReturn(node);
            Output::send<LogLevel::Verbose>(STR("expects_return: {} -- return_offset: {} - {}\n\n"), expects_return, has_return_value, return_value_offset);
        });
    }

    static void OnDekCppTestingFunction([[maybe_unused]] Unreal::UObject* Context, Unreal::FFrame& Stack, [[maybe_unused]] void* RESULT_DECL) {
        TRY([&] {
            auto node = Stack.Node();
            /*
            * Log the full nameof the function
            */
            Output::send<LogLevel::Verbose>(STR("DekCppTestingFunction->GetFullName(): {}\n"), Context->GetFullName());

            /*
            * Read `Stack.Locals()` into `FunctionParams` struct to later access the data.
            */
            struct FunctionParams {
                bool BoolInput;
                Unreal::UScriptStruct &StructInput;
                Unreal::UObject &ObjectInput;
            };

            auto* params = reinterpret_cast<FunctionParams*>(Stack.Locals());
            Output::send<LogLevel::Verbose>(STR("params->BoolInput: {}\n"), params->BoolInput);


            // struct for expected return params
            struct OutputParams {
                bool BoolOutput;
                Unreal::UScriptStruct &StructOutput;
                Unreal::UObject &ObjectOutput;
            };
            auto* outparams = reinterpret_cast<OutputParams*>(RESULT_DECL);
            Output::send<LogLevel::Verbose>(STR("b-outparams->BoolOutput: {}\n"), outparams->BoolOutput);
            outparams->BoolOutput = params->BoolInput;
            //outparams->StructOutput = params->StructInput;
            //outparams->ObjectOutput = params->ObjectInput;
            Output::send<LogLevel::Verbose>(STR("a-outparams->BoolOutput: {}\n"), outparams->BoolOutput);


            auto return_value_offset = node->GetReturnValueOffset();
            auto has_return_value = return_value_offset != 0xFFFF;
            auto expects_return = CheckExpectsReturn(node);
            Output::send<LogLevel::Verbose>(STR("expects_return: {} -- return_offset: {} - {}\n\n"), expects_return, has_return_value, return_value_offset);


            //Output::send<LogLevel::Verbose>(STR("params->ObjectInput.GetName(): {}\n"), params->ObjectInput.GetName());
            //Output::send<LogLevel::Verbose>(STR("params->StructInput.GetName(): {}\n"), params->StructInput.GetName());
            //Output::send<LogLevel::Verbose>(STR("params->ObjectInput.GetPathName(): {}\n"), params->ObjectInput.GetPathName());
            //Output::send<LogLevel::Verbose>(STR("params->StructInput.GetPathName(): {}\n"), params->StructInput.GetPathName());


            //auto thing = static_cast<Unreal::UObject*>(RESULT_DECL);
            //thing->GetFunctionByName(L"");
            
            //auto widgie = static_cast<Unreal::UObject*>(params->WidgetClass);

            //params->WidgetClass.



            //if (RESULT_DECL) {
            //    Output::send<LogLevel::Verbose>(STR("whatever RESULT_DECL is\n"));
            //}
            
            // call function within node? node is what? 
            // UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack
            // node->CallRemoteFunction(nullptr, nullptr, nullptr, nullptr);

            auto return_property = node->GetReturnProperty();
            if (return_property) {
                Output::send<LogLevel::Verbose>(STR("return_property is a thing!\n"));
            }



            return;


            /*
            * Iterate over each of the functions parameters
            */
            auto outparms = Stack.OutParms();

            auto num_unreal_params = node->GetNumParms();
            if (has_return_value && num_unreal_params > 0) { --num_unreal_params; }
            if (has_return_value || num_unreal_params > 0) {
                for (Unreal::FProperty* param : node->ForEachProperty()) {
                    if (!param->HasAnyPropertyFlags(Unreal::EPropertyFlags::CPF_Parm)) { continue; }
                    if (has_return_value && param->GetOffset_Internal() == return_value_offset) { continue; }
                    auto param_type = param->GetClass().GetFName();
                    auto param_type_comparison_index = param_type.GetComparisonIndex();
                    Output::send<LogLevel::Verbose>(STR("{}: {}\n"), param_type.ToString(), param->GetName());
                    // get function param data? 
                    // unrequired due to above `FunctionParams` cast
                    // auto data = param->ContainerPtrToValuePtr<void>(Stack.Locals()); 
                    if (!param->HasAllPropertyFlags(Unreal::EPropertyFlags::CPF_ReturnParm | Unreal::EPropertyFlags::CPF_OutParm)) {
                        Output::send<LogLevel::Verbose>(STR("{}: is a return property!\n"), param->GetName());
                    }
                }
            }

            bool return_value_handled{};
            if (has_return_value && RESULT_DECL) {
                auto return_property = node->GetReturnProperty();
                if (!return_property) return;
                auto return_property_type = return_property->GetClass().GetFName();
                auto return_property_type_comparison_index = return_property_type.GetComparisonIndex();
                // return_value_handled = true;
                std::wstring return_property_type_name = return_property_type.ToString();
                std::wstring return_property_name = return_property->GetName();
                Output::send(STR("Tried altering return value of a custom BP function without a registered handler for return type Return property '{}' of type '{}' not supported."), return_property_name, return_property_type_name);
            }
        });
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



// Unreal::Hook::RegisterProcessEventPreCallback
// Unreal::Hook::AddRequiredObject(STR("/Script/CoreUObject.DynamicClass"));

// auto clazz = Context->GetClassPrivate();
// Stack.Node()->GetFullName();
// Output::send<LogLevel::Verbose>(STR("DekitaMOD RegisterBeginPlayPreCallback()\n"));


//static auto test_function_hook_pre(Unreal::UnrealScriptFunctionCallableContext context, void* custom_data) -> void {
//    Output::send<LogLevel::Verbose>(STR("DekitaMOD test_function_hook_pre!\n"));
//}
//
//static auto modp_function_hook_pre(Unreal::UnrealScriptFunctionCallableContext context, void* custom_data) -> void {
//    Output::send<LogLevel::Verbose>(STR("DekitaMOD modp_function_hook_pre!\n"));
//}

//Output::send<LogLevel::Verbose>(STR("DekitaMOD updating!\n"));
//if (!hasHooked) {

//    Unreal::UFunction* begin_play = Unreal::UObjectGlobals::StaticFindObject<Unreal::UFunction *>(nullptr, nullptr, STR("/Game/DekitaRPG/UICore/UICoreLogic.UICoreLogic_C:ReceiveBeginPlay"));
//    if (begin_play) {
//        begin_play->RegisterPreHook(&test_function_hook_pre, nullptr);
//    }

//    Unreal::UFunction* mod_print = Unreal::UObjectGlobals::StaticFindObject<Unreal::UFunction*>(nullptr, nullptr, STR("/Game/DekitaRPG/UICore/UICoreLogic.UICoreLogic_C:PrintToModLoader"));
//    if (mod_print) {
//        mod_print->RegisterPreHook(&modp_function_hook_pre, nullptr);
//    }
//    hasHooked = true;
//}



#define DEKITA_MOD_API __declspec(dllexport) 
extern "C" {
    DEKITA_MOD_API RC::CppUserModBase* start_mod() { return new DekitaMod(); }
    DEKITA_MOD_API void uninstall_mod(RC::CppUserModBase* mod) { delete mod; }
}