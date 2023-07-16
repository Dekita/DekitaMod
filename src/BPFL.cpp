#include "BPFL.hpp"

namespace DekitaRPG {
    /**
    * [public] Hook()
    * not a hooked bp function, but the main hook to delegate to said functions:
    * called after each and every function call, so only hook needed functions.
    */
    DefineUnrealScriptFunction(DekitaRPG::BPFL::Hook) {
        TRY([&] {
            // ignore functions not from the correct bpfunc lib
            if (!IsValidFunctionClass(Context)) return;
            auto name = Stack.Node()->GetName();
            if (name == STR("LogToUE4SS")) CallUnrealScriptFunction(OnLogToUE4SS);
            else if (name == STR("TryOpenURL")) CallUnrealScriptFunction(OnOpenLink); 
            else if (name == STR("TestReturnBasic")) CallUnrealScriptFunction(OnTestReturnBasic);
            else if (name == STR("TestReturnStruct")) CallUnrealScriptFunction(OnTestReturnStruct);
            else if (name == STR("GetOrCreateWidget")) CallUnrealScriptFunction(OnGetOrCreateWidget); // from DekUICore
        });
    }

    /**
    * [private] IsValidFunctionClass()
    * checks to make sure the given uobject is any of the valid bp func lib classes
    */
    bool DekitaRPG::BPFL::IsValidFunctionClass(Unreal::UObject* Context) {
        auto ACCEPTED_CLASSES = {
            L"Default__BPFL_UICore_C",
            L"Default__DekCpp_BPFL_C",
        };
        auto name = Context->GetName();
        for (decltype(auto) acceptable : ACCEPTED_CLASSES) {
            if (name == acceptable) return true;
        }
        return false;
    }

    /**
    * [private] OnLogToUE4SS()
    * logs a message to ue4ss, allows for log, info, warning and error log levels
    */
    DefineUnrealScriptFunction(DekitaRPG::BPFL::OnLogToUE4SS) {
        auto node = Stack.Node();
        // expected function parameters
        struct FunctionParams {
            Unreal::FString Message;
            int LogLevel;
        };
        // cast Stack.Locals() to expected params struct
        auto params = GetInputParameters<FunctionParams>(Stack);
        auto message = params->Message.GetCharArray();
        switch (params->LogLevel) {
            case 1: INFO("{}", message); break;
            case 2: WARNING("{}", message); break;
            case 3: ERROR("{}", message); break;
            default:LOG("{}", message); break;
        }
    }

    /**
    * [private] OnOpenLink()
    * attempts to open a url in the users preffered browser
    */
    DefineUnrealScriptFunction(DekitaRPG::BPFL::OnOpenLink) {
        if (!PLATFORM_ID) return;
        //CallUnrealScriptFunction(LogFunctionInformation);
        auto node = Stack.Node();
        struct FunctionParams { Unreal::FString URL; };
        auto params = GetInputParameters<FunctionParams>(Stack);
        std::string url = to_string(params->URL.GetCharArray());
        std::array<std::string, 3> commands = {"explorer", "xdg-open", "open"};
        std::string syscom = commands[PLATFORM_ID -1];
        if (syscom.empty() || url.empty()) return;
        std::string command = syscom + " " + url;
        std::system(command.data());
        LOG("OnOpenLink {}", params->URL.GetCharArray());
    }

    /**
    * [private] OnGetOrCreateWidget()
    * called when GetOrCreateWidget is called.
    * this is a function from my HogwartsLegacy UICore mod.
    * another example for reading input params of hooked function
    */
    DefineUnrealScriptFunction(DekitaRPG::BPFL::OnGetOrCreateWidget) {
        TRY([&] {
            auto node = Stack.Node();
            // expected function parameters
            struct FunctionParams {
                Unreal::UWidgetBlueprintGeneratedClass& WidgetClass;
                bool TopLevelOnly;
            };
            // cast Stack.Locals() to expected params struct
            auto params = GetInputParameters<FunctionParams>(Stack);
            // log info from params to make sure its being read properly
            LOG("DekitaRPG::BPFL::OnGetOrCreateWidget");
            CallUnrealScriptFunction(LogFunctionInformation);
            LOG_EXPR(params->WidgetClass.GetName());
            LOG_EXPR(params->WidgetClass.GetPathName());
            LOG_EXPR(params->TopLevelOnly);
            //DekitaRPG::IFace::PropogateTestFunction();
        });
    }

    /**
    * [private] LogFunctionInformation()
    * logs some basic information about a script function, useful for debugging
    */
    DefineUnrealScriptFunction(DekitaRPG::BPFL::LogFunctionInformation) {
        auto node = Stack.Node();
        LOG_EXPR(Context->GetName());
        LOG_EXPR(Context->GetPathName());
        LOG_EXPR(Context->GetFullName());
        LOG_EXPR(Context->GetExporterName().ToString());
        LOG_EXPR(Context->GetClassPrivate()->GetFullName());
        //LOG_EXPR(Context->GetOuter()->GetName());
        LOG_EXPR(node->GetClassPrivate()->GetName());
        LOG_EXPR(node->GetFullName());
        LOG_EXPR(node->GetName());
        LOG("properties:");
        for (decltype(auto) param : node->ForEachProperty()) {
            LOG("{} {}", param->GetClass().GetName(), param->GetName());
        };
        decltype(auto) ret = node->GetReturnProperty();
        if (ret == nullptr) return;
        LOG("RETURN {}", ret->GetClass().GetName());
    }

    /**
    * [private] OnTestReturnBasic()
    * Called when the blueprint function TestReturnBasic is called from test mod
    * an example for reading input and then editing the return parameters
    */
    DefineUnrealScriptFunction(DekitaRPG::BPFL::OnTestReturnBasic) {
        TRY([&] {
            LOG("DekitaRPG::BPFL::OnTestReturnBasic");
            CallUnrealScriptFunction(LogFunctionInformation);
            auto node = Stack.Node();
            // check if this function expects a return value
            //auto expects_return = CheckExpectsReturn(node);

            // cast locals to functions expected input params
            struct FunctionParams {
                bool BoolIn1;
                bool BoolIn2;
                Unreal::FString StringInput;
            };
            auto params = GetInputParameters<FunctionParams>(Stack);

            LOG_EXPR(params->BoolIn1);
            LOG_EXPR(params->BoolIn2);
            LOG_EXPR(params->StringInput.GetCharArray());

            // set output parameters based on name
            auto OutParms = Stack.OutParms();
            SetReturnParameter(OutParms, L"BoolOut1", true);
            SetReturnParameter(OutParms, L"BoolOut2", false);
            SetReturnParameter(OutParms, L"BoolOut3", true);
            SetReturnParameter(OutParms, L"StringOutput", Unreal::FString(L"I'm the altered string yo!"));
            LOG("{}", Context->GetName());

            Output::send<LogLevel::Verbose>(STR("altered return params!\n"));
        });
    }

    /**
    * [private] OnTestReturnStruct()
    * an example for reading input and then editing the return parameters - now with added structs!!
    */
    DefineUnrealScriptFunction(DekitaRPG::BPFL::OnTestReturnStruct) {
        TRY([&] {
            LOG("DekitaRPG::BPFL::OnTestReturnStruct");
            CallUnrealScriptFunction(LogFunctionInformation);
            auto node = Stack.Node();

            // cast locals to functions expected input params
            struct StructParams {
                bool StructBool{ false };
                int StructInt{ 0 };
                Unreal::FString StructString;
            };
            struct FunctionParams {
                // Unreal::UScriptStruct StructInput;
                StructParams StructInput;
                bool BoolInput;
            };
            auto params = GetInputParameters<FunctionParams>(Stack);

            LOG_EXPR(params->StructInput.StructInt);
            LOG_EXPR(params->StructInput.StructBool);
            LOG_EXPR(params->StructInput.StructString.GetCharArray());

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
    * [private] SetReturnParameter()
    * sets a parameters return value for a hooked script function
    */
    template <class T> static void DekitaRPG::BPFL::SetReturnParameter(Unreal::FOutParmRec* OutParms, const wchar_t* ParamName, const T& NewValue) {
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
    * [private] GetInputParameters<T>()
    * casts the given stack.locals() into T to get access to the input params
    */
    template <class T> static T* DekitaRPG::BPFL::GetInputParameters(Unreal::FFrame& Stack) {
        return reinterpret_cast<T*>(Stack.Locals());
    }

    /**
    * [private] ScanForDekitaUI()
    * scans for a specific instance of an actor and print some info about it
    */
    void DekitaRPG::BPFL::ScanForDekitaActor() {
        auto dekname = STR("/Game/DekitaRPG/UICore/UICoreLogic.UICoreLogic_C");
        auto Object = Unreal::UObjectGlobals::StaticFindObject(nullptr, nullptr, dekname);
        if (Object) {
            LOG("DekitaRPG::BPFL::ScanForDekitaActor");
            LOG_EXPR(Object->GetName());
            LOG_EXPR(Object->GetPathName());
            LOG_EXPR(Object->GetFullName());
            LOG_EXPR(Object->GetExporterName().ToString());
            LOG_EXPR(Object->GetClassPrivate()->GetFullName());
            //LOG_EXPR(Object->GetOuter()->GetName());
        }
    }

    /**
    * [private] CheckExpectsReturn()
    * checks if a UFunction has the OutParms flag
    */
    bool DekitaRPG::BPFL::CheckExpectsReturn(Unreal::UFunction* node) {
        return (node->GetFunctionFlags() & Unreal::EFunctionFlags::FUNC_HasOutParms) != 0;
    }

};// namespace
