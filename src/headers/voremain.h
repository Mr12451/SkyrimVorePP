#pragma once

#include "headers/voredata.h"

namespace Vore
{
	namespace Core
	{
		enum RegType : uint8_t
		{
			rNormal, //reguritate living and fully digested
			rTransfer, //called from swallow function when prey was already a prey
			rAll //regurgiate all prey, used when the pred dies
		};
		VoreDataEntry* GetApex(VoreDataEntry* prey);
		void HidePrey(RE::Actor* target);
		void UnhidePrey(RE::Actor* target);
		void FinishPlayerDigestion();
		void SwitchToDigestion(const RE::FormID pred, const Locus locus, const VoreDataEntry::VoreState dType, const bool forceStopDigestion, bool doDialogueUpd = true);
		void StartReformation(VoreDataEntry* preyData, VoreDataEntry* predData);
		void InventoryVore(RE::Actor* pred);
		bool CanMoveToLocus(const RE::FormID& pred, const RE::FormID& prey, const Locus& locus, const Locus& locusSource);
		bool CanBeRegurgitated(const VoreDataEntry* prey);
		void MoveToLocus(const RE::FormID& pred, const RE::FormID& prey, const Locus& locus, const Locus& locusSource = Locus::lNone);
		void SetPreyVisibility(RE::TESObjectREFR* preyObj, RE::Actor* pred, bool show, VoreDataEntry* preyData);

		void AddFakeFood(RE::Actor* pred, RE::AlchemyItem* item);
		void InstantWg(RE::Actor* pred, double amount);
		void InstantWgItem(RE::Actor* pred, RE::AlchemyItem* item);

		/// <summary>
		/// Swallows a list of preys
		/// </summary>
		/// <param name="pred">Actor who does the swallowing</param>
		/// <param name="preys">A list of preys. Can be actors or object refs</param>
		/// <param name="locus">Organ where preys will end up</param>
		/// <param name="ldType">Digestion type</param>
		/// <param name="fullswallow">Skip swallow process</param>
		/// <param name="safeSwitch">Stop digestion if ldType is set to safe and it's already happening</param>
		void Swallow(RE::Actor* pred, std::vector<RE::TESObjectREFR*> preys, Locus locus, VoreDataEntry::VoreState ldType, bool fullswallow, bool safeSwitch);
		/// <summary>
		/// Swallows a prey
		/// </summary>
		/// <param name="pred">Actor who does the swallowing</param>
		/// <param name="prey">A prey. Can be actor or object ref</param>
		/// <param name="locus">Organ where prey will end up</param>
		/// <param name="ldType">Digestion type</param>
		/// <param name="fullswallow">Skip swallow process</param>
		/// <param name="safeSwitch">Stop digestion if ldType is set to safe and it's already happening</param>
		void Swallow(RE::Actor* pred, RE::TESObjectREFR* prey, Locus locus, VoreDataEntry::VoreState ldType, bool fullswallow, bool safeSwitch);
		void SwallowTarget(RE::Actor* pred, Locus locus, VoreDataEntry::VoreState ldType, bool fullswallow = false);
		void Regurgitate(RE::Actor* pred, std::vector<RE::FormID> preys, RegType rtype);
		void Regurgitate(RE::Actor* pred, RE::FormID prey, RegType rtype);
		void RegurgitateAll(RE::Actor* pred, Locus locus = Locus::lNone, RegType rtype = RegType::rNormal);

		void AutoRelease(VoreDataEntry* preyData, VoreDataEntry* predData);

		void SetupTimers();
		void SetupBellies();
		void MegaDigest(const double& delta);
	}
}