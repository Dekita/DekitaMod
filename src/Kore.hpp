#pragma once

// for checking time passed on updates
#include <chrono>

// dek libs: 
#include "BPFL.hpp"
#include "IFace.hpp"
#include "Utils.hpp"

using namespace RC;

namespace DekitaRPG {

	class Kore {
	public:
		static void OnUnrealInitialized();
		static void OnProgramStarted();
		static void OnUpdate();
	private:
		static void OnActorBeginPlay(Unreal::AActor* Context);
		static DefineUnrealScriptFunction(OnPostScriptFunction);
	};

};// namespace
