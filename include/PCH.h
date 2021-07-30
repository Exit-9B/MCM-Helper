#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <unordered_set>
#include <tsl/ordered_map.h>

constexpr std::int32_t PLUGIN_VERSION = 1;

#ifdef NDEBUG
#include <spdlog/sinks/basic_file_sink.h>
#else
#include <spdlog/sinks/msvc_sink.h>
#endif

using namespace std::literals;

namespace logger = SKSE::log;

using ScriptObjectPtr = RE::BSTSmartPointer<RE::BSScript::Object>;
using ScriptArrayPtr = RE::BSTSmartPointer<RE::BSScript::Array>;
using ScriptCallbackPtr = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>;

#define DLLEXPORT __declspec(dllexport)
