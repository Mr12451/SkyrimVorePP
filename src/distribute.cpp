#include "headers/distribute.h"
#include "headers/hook.h"
#include "headers/settings.h"
#include "headers/staticforms.h"
#include "headers/nutils.h"

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

	void Dist::DistrNPC(RE::Character* target)
	{
		if (target->IsInFaction(f_predator) || target->IsInFaction(f_prey)) {
			return;
		} else {
			float heightMod = 1.0f;
			if (VoreSettings::dist_size_influence) {
				heightMod = target->GetBaseHeight();			
			}
			float prob = heightMod;
			if (target->IsHumanoid()) {
				if (target->GetActorBase()->GetSex() == RE::SEX::kFemale) {
					prob *= VoreSettings::dist_female;
				} else {
					prob *= VoreSettings::dist_male;
				}

				if (target->IsInFaction(StaticForms::f_companions) || target->IsInFaction(StaticForms::f_vampire) || target->IsInFaction(StaticForms::f_vampireDLC) || target->IsInFaction(StaticForms::f_vampireSerana)) {
					prob *= VoreSettings::dist_vampire;
				}
			} else {
				prob *= VoreSettings::dist_creature;

				if (target->IsDragon()) {
					prob *= VoreSettings::dist_dragon;
				} else if (target->IsInFaction(StaticForms::f_giant) || target->IsInFaction(StaticForms::f_mammoth)) {
					prob *= VoreSettings::dist_giant;
				} else if (target->IsInFaction(StaticForms::f_skyrim_pred)) {
					prob *= VoreSettings::dist_creature_predator;
				} else if (target->IsInFaction(StaticForms::f_skyrim_prey)) {
					prob *= VoreSettings::dist_creature_prey;
				} else if (target->IsInFaction(StaticForms::f_vampire) || target->IsInFaction(StaticForms::f_hag) || target->IsInFaction(StaticForms::f_vampireDLC)) {
					prob *= VoreSettings::dist_vampire;
				} else if (target->IsInFaction(StaticForms::f_daedra)) {
					prob *= VoreSettings::dist_daedra;
				} else if (target->IsInFaction(StaticForms::f_dwarvenrobot)) {
					prob *= VoreSettings::dist_robot;
				} else if (target->IsInFaction(StaticForms::f_skeleton)) {
					prob *= VoreSettings::dist_skeleton;
				} else if (target->IsInFaction(StaticForms::f_ghost) || target->IsInFaction(StaticForms::f_whisp)) {
					prob *= VoreSettings::dist_ghost;
				} else if (target->HasKeywordWithType(RE::DEFAULT_OBJECT::kKeywordUndead)) {
					prob *= VoreSettings::dist_undead;
				}
			}

			float random = Math::randfloat(0.0f, 100.0f);
			if (random < prob) {
				target->AddToFaction(f_predator, 0);
				flog::trace("PREDATOR: {}, height {}, prob {}", target->GetName(), heightMod, prob);
			} else {
				target->AddToFaction(f_prey, 0);
				flog::trace("PREY: {}, height {}, prob {}", target->GetName(), heightMod, prob);
			}

		}
	}

	void Dist::Initialize()
	{
		Hooks::write_vtable_func<RE::Character, ShouldBackgroundClone>();
		// probably don't need this. we'll see
		Hooks::write_vtable_func<RE::Character, InitLoadGame>();

		flog::info("Installed actor load hooks");

		f_predator = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x83773, "SkyrimVorePP.esp");
		f_prey = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x83774, "SkyrimVorePP.esp");

		flog::info("Loaded vore factions");

	}
}