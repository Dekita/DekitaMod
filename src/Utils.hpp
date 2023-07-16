#pragma once

#include <Helpers/String.hpp>
#include <ExceptionHandling.hpp>
#include <DynamicOutput/Output.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObjectArray.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/UPackage.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/FFrame.hpp>
#include <Unreal/AActor.hpp>
#include <Unreal/Hooks.hpp>

// From UML/Tangerie
#define DefineUnrealScriptFunction(F) void F(Unreal::UObject* Context, Unreal::FFrame& Stack, void* RESULT_DECL)
#define CallUnrealScriptFunction(Function) Function(Context, Stack, RESULT_DECL)

// logging helpers
#define MAINLOG(Lv, Fmt, ...) Output::send<Lv>(STR("[Dek] ") STR(##Fmt), __VA_ARGS__, STR("\n")); 
#define LOG(FormatString, ...) MAINLOG(LogLevel::Default, FormatString, __VA_ARGS__)
#define INFO(FormatString, ...) MAINLOG(LogLevel::Verbose, FormatString, __VA_ARGS__)
#define WARNING(FormatString, ...) MAINLOG(LogLevel::Warning, FormatString, __VA_ARGS__)
#define ERROR(FormatString, ...) MAINLOG(LogLevel::Error, FormatString, __VA_ARGS__)
#define LOG_EXPR(...) { LOG(#__VA_ARGS__ L" = {}", __VA_ARGS__); }

using namespace RC;
namespace DekitaRPG {

	// basic enum for operating systems
	enum EOperatingSystem {
		Invalid,
		Windows,
		Linux,
		Mac,
	};

	// store the id of the current build platform
#ifdef _WIN32
	static const int PLATFORM_ID = 1;
#elif _WIN64
	static const int PLATFORM_ID = 1;
#elif __linux__
	static const int PLATFORM_ID = 2;
#elif __APPLE__
	static const int PLATFORM_ID = 3;
#else
	static const int PLATFORM_ID = 0;
#endif

	// store reference to the current operatingsystem as enum
	static const EOperatingSystem OperatingSystem = static_cast<EOperatingSystem>(PLATFORM_ID);

	// check if a uobject is valid
	bool IsValidUObject(Unreal::UObject*);

};// namespace

