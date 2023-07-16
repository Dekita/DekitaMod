/**
* system: dllmain.cpp
* reason: an example/test for ue4ss c++ mods
* author: dekitarpg@gmail.com
*/

#include <Mod/CppUserModBase.hpp>
#include "Kore.hpp"

/**
* DekitaMod:
* UE4SS c++ mod class defintion
*/
class DekitaMod : public CppUserModBase {
public:
    // constructor
    DekitaMod() : CppUserModBase() {
        ModVersion = STR("0.1");
        ModName = STR("DekitaRPG C++ Test Mod");
        ModAuthors = STR("DekitaRPG@gmail.com");
        ModDescription = STR("To help learn UE4SS C++ modding basics");
        INFO("{} constructed!", ModName);
    }
    // destructor
    ~DekitaMod() { 
        ERROR("{} deconstructed!", ModName);
        // fill when required
    }
    // You are allowed to use the 'Unreal' namespace in this function and anywhere else after this function has fired.
    auto on_unreal_init() -> void override {
        DekitaRPG::Kore::OnUnrealInitialized();
        INFO("{} initialized!", ModName);
    }
    // called after everything has fully initialized
    auto on_program_start() -> void override {
        DekitaRPG::Kore::OnProgramStarted();
        INFO("{} started!", ModName);
    }
    // called each update
    auto on_update() -> void override {
        DekitaRPG::Kore::OnUpdate();
    }
};//class

/**
* export the start_mod() and uninstall_mod() functions to 
* be used by the core ue4ss system to load in our dll mod
*/
#define MOD_EXPORT __declspec(dllexport) 
extern "C" {
    MOD_EXPORT CppUserModBase* start_mod() { return new DekitaMod(); }
    MOD_EXPORT void uninstall_mod(CppUserModBase* mod) { delete mod; }
}