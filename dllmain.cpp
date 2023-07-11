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
        //auto funkname = STR("DekCppTestingFunction");
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
            // ScanForDekitaUI();
        });
    }
    /**
    *
    *
    */
    static void ScanForDekitaUI() {
        auto dekname = STR("/Game/DekitaRPG/UICore/UICoreLogic.UICoreLogic_C");
        auto Object = Unreal::UObjectGlobals::StaticFindObject(nullptr, nullptr, dekname);
        if (Object) {
            Output::send<LogLevel::Verbose>(STR("UICoreLogic.GetName(): {}\n"), Object->GetName());
            Output::send<LogLevel::Verbose>(STR("UICoreLogic.GetFullName(): {}\n"), Object->GetFullName());
            Output::send<LogLevel::Verbose>(STR("UICoreLogic.GetPathName(): {}\n"), Object->GetPathName());
        }
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
                return OnPrintToModLoader(Context, Stack, RESULT_DECL);
            } 
            else if (name == STR("GetOrCreateWidget")) {
                return OnGetOrCreateWidget(Context, Stack, RESULT_DECL);
            }
            else if (name == STR("DekCppTestReturnChange")) {
                return OnDekCppTestReturnChange(Context, Stack, RESULT_DECL);
            } 
            else if (name == STR("DekCppTestStruct")) {
                return OnDekCppTestStruct(Context, Stack, RESULT_DECL);
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
        });
    }

    static void AlterStringProperty(Unreal::FProperty* param, const wchar_t* rawstring, [[maybe_unused]] void* RESULT_DECL) {
        Unreal::FString* string = static_cast<Unreal::FString*>(RESULT_DECL);
        if (!string) {
            Output::send<LogLevel::Verbose>(STR("string pointer is nullptr\n"));
        } else {
            auto newstring = Unreal::FString(rawstring);
            string->SetCharArray(newstring.GetCharTArray());
            Output::send<LogLevel::Verbose>(STR("output should be set to: {}\n"), newstring.GetCharArray());
        }
    }

    static void AlterBoolProperty(Unreal::FProperty* param, bool newbool, [[maybe_unused]] void* RESULT_DECL) {
        uint8_t* bitfield_ptr = static_cast<uint8_t*>(RESULT_DECL);
        if (!bitfield_ptr) {
            Output::send<LogLevel::Verbose>(STR("bitfield_ptr is nullptr\n"));
        } else {
            Unreal::FBoolProperty* bp = static_cast<Unreal::FBoolProperty*>(param);
            uint8_t field_mask = bp->GetFieldMask();
            uint8_t byte_offset = bp->GetByteOffset();
            uint8_t* byte_value = bitfield_ptr + byte_offset;
            uint8_t byte_mask = bp->GetByteMask();
            *byte_value = ((*byte_value) & ~field_mask) | (newbool ? byte_mask : 0);
            //*byte_value = (byte)newbool;
            Output::send<LogLevel::Verbose>(STR("output should be set to: {}\n"), *byte_value);
        }

    }

    
    static void OnDekCppTestReturnChange([[maybe_unused]] Unreal::UObject* Context, Unreal::FFrame& Stack, [[maybe_unused]] void* RESULT_DECL) {
        TRY([&] {
            auto node = Stack.Node();
            // check if this function expects a return value
            //auto expects_return = CheckExpectsReturn(node);

            for (Unreal::FProperty* param : node->ForEachProperty()) {
                if (!param->HasAnyPropertyFlags(Unreal::EPropertyFlags::CPF_Parm)) { continue; }
                auto param_type = param->GetClass().GetFName();
                auto param_type_comparison_index = param_type.GetComparisonIndex();
                Output::send<LogLevel::Verbose>(STR("{}: {}\n"), param_type.ToString(), param->GetName());

                // read current params value into data  
                auto data = param->Unreal::FProperty::ContainerPtrToValuePtr<void>(Stack.Locals());

                //if (Unreal::FName(L"StrProperty").GetComparisonIndex() == param_type_comparison_index) {
                if (L"StringOutput" == param->GetName()) {
                    auto string = L"I'm a string that was made in c++ and send to ue before printing: woah!";
                    Output::send<LogLevel::Verbose>(STR("trying to edit StringOutput\n"));
                    AlterStringProperty(param, string, RESULT_DECL);
                }
                if (L"BoolOutput" == param->GetName()) {
                    Output::send<LogLevel::Verbose>(STR("trying to edit BoolOutput\n"));
                    AlterBoolProperty(param, true, RESULT_DECL);
                }
            }

        });
    }




    static void OnDekCppTestStruct([[maybe_unused]] Unreal::UObject* Context, Unreal::FFrame& Stack, [[maybe_unused]] void* RESULT_DECL) {
        TRY([&] {
            auto node = Stack.Node();

            Output::send<LogLevel::Verbose>(STR("\nDekCppTestingFunction->GetFullName(): {}\n"), Context->GetFullName());

            struct StructParams {
                bool StructBool;
                int StructInt;
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

            // struct for expected return params
            struct OutputParams {
                StructParams StructOutput;
                bool BoolOutput;
            };
            // cast output params 
            auto* outparams = reinterpret_cast<OutputParams*>(RESULT_DECL);
            Output::send<LogLevel::Verbose>(STR("b-outparams->BoolOutput: {}\n"), outparams->BoolOutput);
            
            outparams->BoolOutput = false; // default is true in ue func call
            outparams->StructOutput.StructInt = 1;
            outparams->StructOutput.StructBool = true;

            Output::send<LogLevel::Verbose>(STR("a-outparams->BoolOutput: {}\n"), outparams->BoolOutput);

            // call function within node? node is what? 
            // UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack
            // node->CallRemoteFunction(nullptr, nullptr, nullptr, nullptr);

            auto return_property = node->GetReturnProperty();
            if (return_property) {
                Output::send<LogLevel::Verbose>(STR("return_property is a thing!\n"));
            }

            auto return_value_offset = node->GetReturnValueOffset();
            auto has_return_value = return_value_offset != 0xFFFF;
            auto expects_return = CheckExpectsReturn(node);
            Output::send<LogLevel::Verbose>(STR("expects_return: {} -- return_offset: {} - {}\n\n"), expects_return, has_return_value, return_value_offset);
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


///*
//* Iterate over each of the functions parameters
//*/
//auto outparms = Stack.OutParms();
//
//auto num_unreal_params = node->GetNumParms();
//if (has_return_value && num_unreal_params > 0) { --num_unreal_params; }
//if (has_return_value || num_unreal_params > 0) {
//    for (Unreal::FProperty* param : node->ForEachProperty()) {
//        if (!param->HasAnyPropertyFlags(Unreal::EPropertyFlags::CPF_Parm)) { continue; }
//        if (has_return_value && param->GetOffset_Internal() == return_value_offset) { continue; }
//        auto param_type = param->GetClass().GetFName();
//        auto param_type_comparison_index = param_type.GetComparisonIndex();
//        Output::send<LogLevel::Verbose>(STR("{}: {}\n"), param_type.ToString(), param->GetName());
//        // get function param data? 
//        // unrequired due to above `FunctionParams` cast
//        // auto data = param->ContainerPtrToValuePtr<void>(Stack.Locals()); 
//        if (!param->HasAllPropertyFlags(Unreal::EPropertyFlags::CPF_ReturnParm | Unreal::EPropertyFlags::CPF_OutParm)) {
//            Output::send<LogLevel::Verbose>(STR("{}: is a return property!\n"), param->GetName());
//        }
//    }
//}
//
//bool return_value_handled{};
//if (has_return_value && RESULT_DECL) {
//    auto return_property = node->GetReturnProperty();
//    if (!return_property) return;
//    auto return_property_type = return_property->GetClass().GetFName();
//    auto return_property_type_comparison_index = return_property_type.GetComparisonIndex();
//    // return_value_handled = true;
//    std::wstring return_property_type_name = return_property_type.ToString();
//    std::wstring return_property_name = return_property->GetName();
//    Output::send(STR("Tried altering return value of a custom BP function without a registered handler for return type Return property '{}' of type '{}' not supported."), return_property_name, return_property_type_name);
//}
//
//Output::send<LogLevel::Verbose>(STR("params->StructInput.GetName(): {}\n"), structname);
//            });


//struct FunctionParams {
//    bool BoolIn2;
//    Unreal::UScriptStruct StructInput;
//    bool BoolInput;
//    // Unreal::UClass
//};
//auto* params = reinterpret_cast<FunctionParams*>(Stack.Locals());

//Output::send<LogLevel::Verbose>(STR("params->BoolInput: {}\n"), params->BoolInput);
//Output::send<LogLevel::Verbose>(STR("params->BoolIn2: {}\n"), params->BoolIn2);

//Output::send<LogLevel::Verbose>(STR("Iterating StructInput;..\n"));

//for (Unreal::FProperty* param : params->StructInput.ForEachProperty()) {
//    if (!param->HasAnyPropertyFlags(Unreal::EPropertyFlags::CPF_Parm)) { continue; }
//    auto param_type = param->GetClass().GetFName();
//    auto param_type_comparison_index = param_type.GetComparisonIndex();
//    Output::send<LogLevel::Verbose>(STR("{}: {}\n"), param_type.ToString(), param->GetName());
//}

//Output::send<LogLevel::Verbose>(STR("Iterated StructInput;..\n"));

//auto structname = params->StructInput.GetName();
//Output::send<LogLevel::Verbose>(STR("{}\n"), structname);

//auto* structparams = reinterpret_cast<StructParams*>(params->StructInput);




//struct InputParams {
//    bool BoolInput;
//    Unreal::UObject __WorldContext;
//    bool BoolOutput;
//    Unreal::FString StringOutput;
//};
//auto* inputparams = reinterpret_cast<InputParams*>(Stack.Locals());

//struct OutputParams {
//    bool BoolOutput;
//    Unreal::FString StringOutput;
//};

//auto* outputparams = static_cast<Unreal::UStruct*>(RESULT_DECL);
////auto* outputparams = reinterpret_cast<OutputParams*>(RESULT_DECL);

//auto BoolOutput = outputparams->GetValuePtrByPropertyName(L"BoolOutput");
//if (BoolOutput) {
//    // BoolOutput = true;
//}

//// log info from params to make sure its being read properly
//Output::send<LogLevel::Verbose>(STR("\nOnDekCppTestReturnChange->GetFullName(): {}\n"), Context->GetFullName());
//Output::send<LogLevel::Verbose>(STR("inputparams->BoolInput: {}\n"), inputparams->BoolInput);
//Output::send<LogLevel::Verbose>(STR("outputparams->BoolOutput: {}\n"), BoolOutput);
////Output::send<LogLevel::Verbose>(STR("outputparams->StringOutput: {}\n"), outputparams->StringOutput.GetCharArray());

//Output::send<LogLevel::Verbose>(STR("AfterChange:\n"));
//auto nustr = Unreal::FString(STR("I'm a string that was made in c++ and send to ue before printing: woah!"));

//struct OutputParams {
//    bool BoolOutput;
//    Unreal::FString StringOutput;
//};
//auto* outputparams = reinterpret_cast<OutputParams*>(RESULT_DECL);

//outputparams->BoolOutput = true;
//outputparams->StringOutput = nustr;


//auto base = static_cast<Unreal::UObject*>(RESULT_DECL);
//auto baseptr = base->GetValuePtrByPropertyName(L"BoolOutput");

//base->GetPropertyByName(L"");

//inputparams->BoolOutput = true;
//inputparams->StringOutput = nustr;

//outputparams->StringOutput->SetCharArray(nustr.GetCharTArray());

//Output::send<LogLevel::Verbose>(STR("outputparams->StringOutput: {}\n"), outputparams->StringOutput.GetCharArray());
//Output::send<LogLevel::Verbose>(STR("outputparams->BoolOutput: {}\n"), outputparams->BoolOutput);



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