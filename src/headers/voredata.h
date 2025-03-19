#pragma once
#include "voredataentry.h"

namespace Vore
{
	class VoreData
	{
		//presistent data will be saved here
	public:
		static inline std::unordered_map<RE::FormID, Vore::VoreDataEntry> Data{};
		static inline std::unordered_map<RE::FormID, RE::FormID> Reforms{};

		static bool IsValid(RE::FormID character);
		static VoreDataEntry* IsValidGet(RE::FormID character);
		static bool IsPred(RE::FormID character, bool onlyActive);
		static bool IsPrey(RE::FormID character);

		static RE::FormID MakeData(RE::TESObjectREFR* character);
		static void SoftDelete(RE::FormID character, bool leaveOnlyActive = false);
		static void HardDelete(RE::FormID character);

		static void DataSetup();

		static void OnSave(SKSE::SerializationInterface* a_intfc);
		static void OnLoad(SKSE::SerializationInterface* a_intfc);
		static void OnRevert(SKSE::SerializationInterface* a_intfc);
	};
}