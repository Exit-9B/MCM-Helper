#pragma once

#pragma warning( disable : 5105)
#pragma warning( push, 0 )

#include <unordered_set>
#include <tsl/ordered_map.h>

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <MergeMapperPluginAPI.h>

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
using ScriptArgs = std::unique_ptr<RE::BSScript::IFunctionArguments>;
using VMAwaitable = RE::BSScript::IVirtualMachine::Awaitable;

template <typename T = void>
using LatentResult = RE::BSScript::LatentResult<T>;

namespace util
{
	using SKSE::stl::report_and_fail;
	using SKSE::stl::utf8_to_utf16;
	using SKSE::stl::utf16_to_utf8;
}

#define DLLEXPORT __declspec(dllexport)

#include "Plugin.h"
