#include "IFace.hpp"

namespace DekitaRPG {

    //initialize IFaceUFunkMap g_ContextItemMap;
    IFaceUFunkMap IFace::g_ContextItemMap;

    /**
    * [public] DekitaRPG::IFace::Hook()
    * called after begin play runs for every AActor
    * ensures that any actor who implements the desired blueprint interface class
    * will properly receive the events when the system fires them.
    */
    void IFace::Hook(Unreal::AActor* Context) {
        TRY([&] {
            // for hooking endplay event on desired actors
            Unreal::UFunction* pEndPlayFunction = nullptr;
            decltype(auto) bHasImplementedInterface = false;
            // check for a specific class of aactor (bp biped player from hogwarts legacy)
            if (Context->GetName().starts_with(STR("BP_Biped_Player_C"))) {
                INFO("FOUND BP_Biped_Player_C!");
            }
            // iterate over actors available interfaces
            decltype(auto) actorclass = Context->GetClassPrivate();
            for (decltype(auto) iface : actorclass->GetInterfaces()) {
                // ensure only hook into desired interface class
                if (iface.Class->GetName() != L"DekCpp_IFace_C") continue;
                if (!bHasImplementedInterface) { // only check once, cuz optimize
                    bHasImplementedInterface = true;
                    // set EndPlay UFunction* here to ensure we only grab it once for actors that include interface
                    pEndPlayFunction = Context->GetFunctionByName(STR("ReceiveEndPlay"));
                    //auto endplay = Context->GetFunctionByNameInChain(STR("ReceiveEndPlay"));
                    if (pEndPlayFunction) {
                        LOG("FOUND EndPlay for blueprint with DekCppInterface!");
                    } else {
                        LOG("No default EndPlay event implemented in blueprint with DekCppInterface!!");
                        return; // hard return as no point hooking interface event functions
                    }
                }
                // iterate over each available function within this interface and add as context item
                for (decltype(auto) pIFaceFunk : iface.Class->ForEachFunction()) {
                    auto id = pIFaceFunk->GetName();
                    auto func = Context->GetFunctionByName(id.c_str());
                    if (!func) LOG("AddContextItem error: bad hook pointer {}", id.c_str());
                    AddContextItem(func, Context);
                    //LOG("Test Calling InterfaceHookTest: {}", id);
                    //Context->ProcessEvent(func, &params);
                }
            }
            // register hook for end play
            if (pEndPlayFunction && bHasImplementedInterface) {
                Unreal::UObjectGlobals::RegisterHook(pEndPlayFunction, OnPreEndPlayHook, OnPostEndPlayHook, nullptr);
            }
        });
    }

    /**
    * [private] OnPreEndPlayHook()
    * called when any hooked AActor's EndPlay event fires on the blueprint
    * removes the context items associated with the AActor when this happens. 
    */
    void IFace::OnPreEndPlayHook(Unreal::UnrealScriptFunctionCallableContext& Context, void* CustomData) {
        auto pActor = reinterpret_cast<Unreal::AActor*>(Context.Context);
        LOG("IFace::OnEndPlayHook", pActor->GetName());
        RemoveContextItems(pActor);
    }

    /**
    * [private] OnPostEndPlayHook()
    * only really added for completeness <3
    */
    void IFace::OnPostEndPlayHook(Unreal::UnrealScriptFunctionCallableContext& Context, void* CustomData) {
        // 
        // todo?: something, maybe...
        // 
    }

    /**
    * [public] PropogateTestFunction()
    * propogates test function to all Unreal::AActor that have included
    * the interface and BeginPlay and EndPlay within the blueprint.
    * NOTE: events are required to be implemented even if they do nothing.
    * otherwise the script hook never catches. 
    */
    void IFace::PropogateTestFunction() {
        //LOG("IFace::PropogateTestFunction()");
        struct Params { Unreal::FString ResultString; bool Success; };
        auto params = Params {
            .ResultString = Unreal::FString(STR("test success!!")),
            .Success = true,
        };
        // iterate over g_ContextItemMap for InterfaceHookTest with given params
        Iterate(L"InterfaceHookTest", &params);
    }

    /**
    * [private] AddContextItem()
    * adds IFaceUFunkMapItem `item` into the vector for function `id`
    */
    void IFace::AddContextItem(Unreal::UFunction* Function, Unreal::AActor* Context) {
        auto id = Function->GetName();
        //LOG("AddContextItem: {}", id);
        // ensure vectors for g_ContextItemMap id exist
        if (g_ContextItemMap.count(id) != 1) { // count = 1 when exist
            g_ContextItemMap[id] = std::vector<IFaceUFunkMapItem>(); // set g_ContextItemMap id to vector
            //LOG("created data {}", id);
        }
        // get desired g_ContextItemMap by id and then add funcmapitem
        auto& thismap = g_ContextItemMap.at(id);
        thismap.emplace_back(IFaceUFunkMapItem {
            .Function = Function,
            .Context = Context, 
        });
        //LOG("added item, size: {}", thismap.size());
    }

    /**
    * [private] RemoveContextItems()
    * removes IFaceUFunkMapItem from g_ContextItemMap when Context matches
    */
    void IFace::RemoveContextItems(Unreal::AActor* Context) {
        //LOG("RemoveContextItems {}", Context->GetName());
        for (auto& it : g_ContextItemMap) {
            decltype(auto) k = it.first; // funcmap key
            auto& v = it.second; // funcmap value (vector)
            // see: https://www.techiedelight.com/remove-elements-vector-inside-loop-cpp/
            for (auto vit = v.begin(); vit != v.end(); ++vit) {
                if ((*vit).Context == Context) {
                    vit = v.erase(vit);
                    vit--;
                }
            }
        }
    }

    /**
    * [private] Iterate()
    * iterates over registered items and attempts to call their function
    */
    void IFace::Iterate(std::wstring funcname, void* Params) {
        //LOG("IFace::Iterate {}", funcname);
        TRY([&] {
            for (auto& it : g_ContextItemMap) {
                // ensure matching function key
                auto k = it.first;
                if (k != funcname) continue;
                // iterate v (UFunction* vector)
                decltype(auto) v = it.second;
                //LOG("iterating: {} - {}", k, v.size());
                for (auto& iface : v) {
                    auto isValidContext = IsValidUObject(iface.Context);
                    auto isValidFunction = IsValidUObject(iface.Function);
                    // both should always be valid as removed on end play <3
                    // but always good to double check things to make sure
                    if (!isValidContext || !isValidFunction) {
                        LOG("iface.Context: {} [isValid:{}]", k, isValidContext);
                        LOG("iface.Function: {} [isValid:{}]", k, isValidFunction);
                        RemoveContextItems(iface.Context); // failsafe to remove | should never occur
                        continue; // no point in processing the event as it will crash the game, again, shouldnt happen
                    }
                    // call function if both ting are dem valid ting
                    iface.Context->ProcessEvent(iface.Function, Params);
                }
            }
        });
    }
};// namespace

