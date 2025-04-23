#pragma once
#include "voredataentry.h"

namespace Vore
{
	class VoreCharStats
	{
	public:
		float predLevel = 0.0f;
		float predXp = 0.0f;
		float predThreshold = 0.0f;

		float preyLevel = 0.0f;
		float preyXp = 0.0f;
		float preyThreshold = 0.0f;

		RE::FormID reformer = 0;

		void CalcThreshold(bool isPred);
		void AdvSkill(bool isPred, float xp);
		void SetLvl(bool isPred, float lvl);
		void SyncWithActor(bool isPred, RE::Actor* actor) const;
		void AdvSkillAndSync(bool isPred, float xp, RE::Actor* actor);
	};
	class VoreData
	{
		//presistent data will be saved here
	public:
		static inline std::unordered_map<RE::FormID, Vore::VoreDataEntry> Data{};
		static inline std::unordered_map<RE::FormID, Vore::VoreCharStats> Stats{};
		static inline int32_t stats_dist_version = 0;

		static bool IsValid(RE::FormID character);
		static VoreDataEntry* IsValidGet(RE::FormID character);
		static VoreDataEntry* GetDataOrMake(RE::TESObjectREFR* character);
		static VoreCharStats* IsValidStatGet(RE::FormID character);
		static VoreCharStats* GetStatOrMake(RE::TESObjectREFR* character);
		static bool IsPred(RE::FormID character, bool onlyActive);
		static bool IsPrey(RE::FormID character);

		static void SoftDelete(RE::FormID character, bool leaveOnlyActive = false);
		static void HardDelete(RE::FormID character);

		static void DataSetup();

		static void OnSave(SKSE::SerializationInterface* a_intfc);
		static void OnLoad(SKSE::SerializationInterface* a_intfc);
		static void OnRevert(SKSE::SerializationInterface* a_intfc);
	};
}