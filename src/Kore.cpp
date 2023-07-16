
#include "Kore.hpp"

/**
* DekitaRPG::Kore
* main mod logic class
*/
namespace DekitaRPG {

    // simple variable to store update time for timed update testing
    std::chrono::steady_clock::time_point last_update = std::chrono::steady_clock::now();

    /**
    * OnUnrealInitialized()
    * called when unreal engine initializes
    * able to use the 'Unreal' namespace in this function and anywhere else after this function has fired.
    */

    void Kore::OnUnrealInitialized() {
        Unreal::Hook::RegisterBeginPlayPostCallback(&OnActorBeginPlay);
        Unreal::Hook::RegisterProcessLocalScriptFunctionPostCallback(&OnPostScriptFunction);
    }

    /**
    * OnProgramStarted()
    * called when the application properly begins
    */
    void Kore::OnProgramStarted() {

    }

    /**
    * OnUpdate()
    * called during each update
    */
    void Kore::OnUpdate() {
        auto this_update = std::chrono::steady_clock::now();
        auto counter = std::chrono::duration_cast<std::chrono::seconds>(this_update - last_update).count();
        if (counter >= 5) {
            DekitaRPG::IFace::PropogateTestFunction();
            last_update = std::chrono::steady_clock::now();
        }
    }

    /*
    * hook functions defined here as wrappers in case more than one
    * class needs to listen for the same event
    */
    void Kore::OnActorBeginPlay(Unreal::AActor* Context) {
        DekitaRPG::IFace::Hook(Context);
    }
    DefineUnrealScriptFunction(Kore::OnPostScriptFunction) {
        CallUnrealScriptFunction(DekitaRPG::BPFL::Hook);
    }
};// namespace
