#pragma once

#pragma warning(push)
#define SKSE_SUPPORT_XBYAK
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <xbyak/xbyak.h>

#pragma warning(pop)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace std::literals;
namespace flog = SKSE::log;

#include "Plugin.h"

#include <ClibUtil/distribution.hpp>
#include <ClibUtil/editorID.hpp>
#include <ClibUtil/numeric.hpp>
#include <ClibUtil/rng.hpp>
#include <ClibUtil/simpleINI.hpp>

#include <set>

using uint = uint32_t;

#define DLLEXPORT __declspec(dllexport)



const int VORE_VERSION = 1;


