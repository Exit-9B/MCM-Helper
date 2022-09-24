#pragma once

#pragma warning( disable : 5105)
#pragma warning( push, 0 )

#include <unordered_set>
#include <tsl/ordered_map.h>

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#ifdef NDEBUG
#include <spdlog/sinks/basic_file_sink.h>
#else
#include <spdlog/sinks/msvc_sink.h>
#endif

#pragma warning( pop )

using namespace std::literals;

namespace logger = SKSE::log;

using ScriptObjectPtr = RE::BSTSmartPointer<RE::BSScript::Object>;
using ScriptArrayPtr = RE::BSTSmartPointer<RE::BSScript::Array>;
using ScriptCallbackPtr = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>;

namespace util
{
	using SKSE::stl::report_and_fail;
}

#define DLLEXPORT __declspec(dllexport)

#include "Plugin.h"
