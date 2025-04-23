#include "headers/distribute.h"
#include "headers/hook.h"
#include "headers/nutils.h"
#include "headers/settings.h"
#include "headers/staticforms.h"

namespace Vore
{
	// from SPID by powerof3
	struct ShouldBackgroundClone
	{
		static bool thunk(RE::Character* a_this)
		{
			bool res = func(a_this);
			Dist::DistrNPC(a_this);
			return res;
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline constexpr std::size_t index{ 0 };
		static inline constexpr std::size_t size{ 0x6D };
	};

	// probably unneeded
	struct InitLoadGame
	{
		static void thunk(RE::Character* a_this, RE::BGSLoadFormBuffer* a_buf)
		{
			func(a_this, a_buf);

			if (a_this->Is3DLoaded()) {
				Dist::DistrNPC(a_this);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline constexpr std::size_t index{ 0 };
		static inline constexpr std::size_t size{ 0x10 };
	};

	int8_t Dist::GetPRank(bool isPred, RE::Actor* target)
	{
		if (isPred) {
			return (int8_t)target->GetFactionRank(f_predator, target->IsPlayerRef());
		} else {
			return (int8_t)target->GetFactionRank(f_prey, target->IsPlayerRef());
		}
	}

	void Dist::SetPRank(bool isPred, RE::Actor* target, int8_t rank)
	{
		int8_t prank = GetPRank(isPred, target);
		if (prank == rank) {
			return;
		}
		if (isPred) {
			if (prank > -2) {
				target->RemoveFromFaction(f_predator);
			}
			if (rank > -2) {
				target->AddToFaction(f_predator, rank);
			}
		} else {
			if (prank > -2) {
				target->RemoveFromFaction(f_prey);
			}
			if (rank > -2) {
				target->AddToFaction(f_prey, rank);
			}
		}
	}

	static int RandomRank(std::array<int, 6>& rankDist, RE::SEX sex, bool pred = false)
	{
		if (pred) {
			if (sex == RE::SEXES::kFemale && !VoreSettings::dist_f_allow) {
				return 0;
			} else if (sex == RE::SEXES::kMale && !VoreSettings::dist_m_allow) {
				return 0;
			} else if (sex == RE::SEXES::kNone && !VoreSettings::dist_c_allow) {
				return 0;
			}
		}
		int res = (int)Math::randInt(1, 100);
		int acc = 100;
		for (int i = 5; i >= 0; i--) {
			acc -= rankDist[i];
			if (res > acc) {
				return i + 1;
			}
		}
		return 0;
	}

	void Dist::DistrNPC(RE::Character* target)
	{
		// the idea is this
		// all npcs will have pred and prey factions, which basically serves as a skill
		// these factions will have ranks that show how efficient said npc as a pred / prey
		// it does not reflect how often they use vore or how much they like
		// pred 0 = normal human who can't do vore (the stomach capacity is too small. Can be trained through item vore to reach rank 1)
		// pred 1 = can swallow less than a full human (small animal)
		// pred 2 = 1 human
		// pred 3+ = prey limit is determined by randomly distributed perks
		// basically the system distributes pred level according to many factors, and then adds perks corresponding to that level
		// when an npc levels their vore level, they will get new vore perks according to their vore personality
		// vore personality is also determined by their factions, which will be distributed when the vore level is distributed
		// but when an npc levels up, their preferences might change depending on their statistic
		// maybe add a stat which will determine how likely an npcs is to change their mind and preferences, something like stubborness
		// npcs will have their preferenses stored in a separate map
		// the map will be filled when vore is initiated for the npc the first time

		if (!target) {
			return;
		}
		if (target->GetFactionRank(f_distr_version, target->IsPlayerRef()) == VoreSettings::dist_version) {
			// dist version can be changed to reset all vore progress
			return;
		}
		//distr pred stats
		if (target->IsInFaction(f_distr_version)) {
			target->RemoveFromFaction(f_distr_version);
		}
		if (target->IsInFaction(f_predator)) {
			target->RemoveFromFaction(f_predator);
		}
		if (target->IsInFaction(f_prey)) {
			target->RemoveFromFaction(f_prey);
		}

		RE::TESRace* race = target->GetRace();
		int pDrank = 0;
		int pYrank = 0;

		if (target->IsHumanoid()) {
			if (target->GetActorBase()->GetSex() == RE::SEX::kFemale) {
				if (target->IsInFaction(StaticForms::f_companions) || target->IsInFaction(StaticForms::f_vampire) || target->IsInFaction(StaticForms::f_vampireDLC) || target->IsInFaction(StaticForms::f_vampireSerana)) {
					pDrank = RandomRank(VoreSettings::dist_pd_f_vampire, RE::SEXES::kFemale, true);
					pYrank = RandomRank(VoreSettings::dist_py_f_vampire, RE::SEXES::kFemale);
				} else if (race == StaticForms::r_arg) {
					pDrank = RandomRank(VoreSettings::dist_pd_f_arg, RE::SEXES::kFemale, true);
					pYrank = RandomRank(VoreSettings::dist_py_f_arg, RE::SEXES::kFemale);
				} else if (race == StaticForms::r_bre) {
					pDrank = RandomRank(VoreSettings::dist_pd_f_bre, RE::SEXES::kFemale, true);
					pYrank = RandomRank(VoreSettings::dist_py_f_bre, RE::SEXES::kFemale);
				} else if (race == StaticForms::r_del) {
					pDrank = RandomRank(VoreSettings::dist_pd_f_del, RE::SEXES::kFemale, true);
					pYrank = RandomRank(VoreSettings::dist_py_f_del, RE::SEXES::kFemale);
				} else if (race == StaticForms::r_hel) {
					pDrank = RandomRank(VoreSettings::dist_pd_f_hel, RE::SEXES::kFemale, true);
					pYrank = RandomRank(VoreSettings::dist_py_f_hel, RE::SEXES::kFemale);
				} else if (race == StaticForms::r_imp) {
					pDrank = RandomRank(VoreSettings::dist_pd_f_imp, RE::SEXES::kFemale, true);
					pYrank = RandomRank(VoreSettings::dist_py_f_imp, RE::SEXES::kFemale);
				} else if (race == StaticForms::r_kha) {
					pDrank = RandomRank(VoreSettings::dist_pd_f_kha, RE::SEXES::kFemale, true);
					pYrank = RandomRank(VoreSettings::dist_py_f_kha, RE::SEXES::kFemale);
				} else if (race == StaticForms::r_nor) {
					pDrank = RandomRank(VoreSettings::dist_pd_f_nor, RE::SEXES::kFemale, true);
					pYrank = RandomRank(VoreSettings::dist_py_f_nor, RE::SEXES::kFemale);
				} else if (race == StaticForms::r_orc) {
					pDrank = RandomRank(VoreSettings::dist_pd_f_orc, RE::SEXES::kFemale, true);
					pYrank = RandomRank(VoreSettings::dist_py_f_orc, RE::SEXES::kFemale);
				} else if (race == StaticForms::r_red) {
					pDrank = RandomRank(VoreSettings::dist_pd_f_red, RE::SEXES::kFemale, true);
					pYrank = RandomRank(VoreSettings::dist_py_f_red, RE::SEXES::kFemale);
				} else if (race == StaticForms::r_wel) {
					pDrank = RandomRank(VoreSettings::dist_pd_f_wel, RE::SEXES::kFemale, true);
					pYrank = RandomRank(VoreSettings::dist_py_f_wel, RE::SEXES::kFemale);
				} else if (race == StaticForms::r_old) {
					pDrank = RandomRank(VoreSettings::dist_pd_f_old, RE::SEXES::kFemale, true);
					pYrank = RandomRank(VoreSettings::dist_py_f_old, RE::SEXES::kFemale);
				} else if (target->IsChild()) {
					pDrank = 0;
					pYrank = 0;
				} else {
					pDrank = RandomRank(VoreSettings::dist_pd_f_imp, RE::SEXES::kFemale, true);
					pYrank = RandomRank(VoreSettings::dist_py_f_imp, RE::SEXES::kFemale);
				}
			} else {
				if (target->IsInFaction(StaticForms::f_companions) || target->IsInFaction(StaticForms::f_vampire) || target->IsInFaction(StaticForms::f_vampireDLC)) {
					pDrank = RandomRank(VoreSettings::dist_pd_m_vampire, RE::SEXES::kMale, true);
					pYrank = RandomRank(VoreSettings::dist_py_m_vampire, RE::SEXES::kMale);
				} else if (race == StaticForms::r_arg) {
					pDrank = RandomRank(VoreSettings::dist_pd_m_arg, RE::SEXES::kMale, true);
					pYrank = RandomRank(VoreSettings::dist_py_m_arg, RE::SEXES::kMale);
				} else if (race == StaticForms::r_bre) {
					pDrank = RandomRank(VoreSettings::dist_pd_m_bre, RE::SEXES::kMale, true);
					pYrank = RandomRank(VoreSettings::dist_py_m_bre, RE::SEXES::kMale);
				} else if (race == StaticForms::r_del) {
					pDrank = RandomRank(VoreSettings::dist_pd_m_del, RE::SEXES::kMale, true);
					pYrank = RandomRank(VoreSettings::dist_py_m_del, RE::SEXES::kMale);
				} else if (race == StaticForms::r_hel) {
					pDrank = RandomRank(VoreSettings::dist_pd_m_hel, RE::SEXES::kMale, true);
					pYrank = RandomRank(VoreSettings::dist_py_m_hel, RE::SEXES::kMale);
				} else if (race == StaticForms::r_imp) {
					pDrank = RandomRank(VoreSettings::dist_pd_m_imp, RE::SEXES::kMale, true);
					pYrank = RandomRank(VoreSettings::dist_py_m_imp, RE::SEXES::kMale);
				} else if (race == StaticForms::r_kha) {
					pDrank = RandomRank(VoreSettings::dist_pd_m_kha, RE::SEXES::kMale, true);
					pYrank = RandomRank(VoreSettings::dist_py_m_kha, RE::SEXES::kMale);
				} else if (race == StaticForms::r_nor) {
					pDrank = RandomRank(VoreSettings::dist_pd_m_nor, RE::SEXES::kMale, true);
					pYrank = RandomRank(VoreSettings::dist_py_m_nor, RE::SEXES::kMale);
				} else if (race == StaticForms::r_orc) {
					pDrank = RandomRank(VoreSettings::dist_pd_m_orc, RE::SEXES::kMale, true);
					pYrank = RandomRank(VoreSettings::dist_py_m_orc, RE::SEXES::kMale);
				} else if (race == StaticForms::r_red) {
					pDrank = RandomRank(VoreSettings::dist_pd_m_red, RE::SEXES::kMale, true);
					pYrank = RandomRank(VoreSettings::dist_py_m_red, RE::SEXES::kMale);
				} else if (race == StaticForms::r_wel) {
					pDrank = RandomRank(VoreSettings::dist_pd_m_wel, RE::SEXES::kMale, true);
					pYrank = RandomRank(VoreSettings::dist_py_m_wel, RE::SEXES::kMale);
				} else if (race == StaticForms::r_old) {
					pDrank = RandomRank(VoreSettings::dist_pd_m_old, RE::SEXES::kMale, true);
					pYrank = RandomRank(VoreSettings::dist_py_m_old, RE::SEXES::kMale);
				} else if (target->IsChild()) {
					pDrank = 0;
					pYrank = 0;
				} else {
					pDrank = RandomRank(VoreSettings::dist_pd_m_imp, RE::SEXES::kMale, true);
					pYrank = RandomRank(VoreSettings::dist_py_m_imp, RE::SEXES::kMale);
				}
			}
		} else {
			if (target->IsInFaction(StaticForms::f_vampire) || target->IsInFaction(StaticForms::f_hag) || target->IsInFaction(StaticForms::f_vampireDLC)) {
				pDrank = RandomRank(VoreSettings::dist_pd_vampire, RE::SEXES::kNone, true);
				pYrank = RandomRank(VoreSettings::dist_py_vampire, RE::SEXES::kNone);
			} else if (target->IsDragon()) {
				pDrank = RandomRank(VoreSettings::dist_pd_dragon, RE::SEXES::kNone, true);
				pYrank = RandomRank(VoreSettings::dist_py_dragon, RE::SEXES::kNone);
			} else if (target->IsInFaction(StaticForms::f_daedra)) {
				pDrank = RandomRank(VoreSettings::dist_pd_daedra, RE::SEXES::kNone, true);
				pYrank = RandomRank(VoreSettings::dist_py_daedra, RE::SEXES::kNone);
			} else if (target->IsInFaction(StaticForms::f_dwarvenrobot)) {
				pDrank = RandomRank(VoreSettings::dist_pd_robot, RE::SEXES::kNone, true);
				pYrank = RandomRank(VoreSettings::dist_py_robot, RE::SEXES::kNone);
			} else if (target->IsInFaction(StaticForms::f_skeleton) || target->IsInFaction(StaticForms::f_ghost) || target->IsInFaction(StaticForms::f_whisp)) {
				pDrank = RandomRank(VoreSettings::dist_pd_skeleton, RE::SEXES::kNone, true);
				pYrank = RandomRank(VoreSettings::dist_py_skeleton, RE::SEXES::kNone);
			} else {
				float baseHp = target->GetRace()->data.startingHealth;
				if (baseHp < 12) {
					pDrank = RandomRank(VoreSettings::dist_pd_c_0, RE::SEXES::kNone, true);
					pYrank = RandomRank(VoreSettings::dist_py_c_0, RE::SEXES::kNone);
				} else if (baseHp < 40) {
					pDrank = RandomRank(VoreSettings::dist_pd_c_1, RE::SEXES::kNone, true);
					pYrank = RandomRank(VoreSettings::dist_py_c_1, RE::SEXES::kNone);
				} else if (baseHp < 101) {
					pDrank = RandomRank(VoreSettings::dist_pd_c_2, RE::SEXES::kNone, true);
					pYrank = RandomRank(VoreSettings::dist_py_c_2, RE::SEXES::kNone);
				} else if (baseHp < 300) {
					pDrank = RandomRank(VoreSettings::dist_pd_c_3, RE::SEXES::kNone, true);
					pYrank = RandomRank(VoreSettings::dist_py_c_3, RE::SEXES::kNone);
				} else {
					pDrank = RandomRank(VoreSettings::dist_pd_c_4, RE::SEXES::kNone, true);
					pYrank = RandomRank(VoreSettings::dist_py_c_4, RE::SEXES::kNone);
				}
			}
		}


		target->AddToFaction(f_predator, (int8_t)pDrank);
		target->AddToFaction(f_prey, (int8_t)pYrank);
		target->AddToFaction(f_distr_version, (int8_t)VoreSettings::dist_version);

		flog::trace("DISTR: {}, pred {}, prey {}", target->GetName(), pDrank, pYrank);
	}

	void Dist::Initialize()
	{
		Hooks::write_vtable_func<RE::Character, ShouldBackgroundClone>();
		Hooks::write_vtable_func<RE::Character, InitLoadGame>();

		flog::info("Installed actor load hooks");

		f_predator = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x83773, "SkyrimVorePP.esp");
		f_prey = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x83774, "SkyrimVorePP.esp");
		f_distr_version = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x8D977, "SkyrimVorePP.esp");

		flog::info("Loaded vore factions");
	}
}