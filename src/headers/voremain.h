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
		void HidePrey(RE::Actor* target);
		void UnhidePrey(RE::Actor* target);
		void SwitchToDigestion(const RE::FormID pred, const Locus locus, const VoreDataEntry::VoreState dType, const bool forceStopDigestion, bool doDialogueUpd = true);
		bool CanMoveToLocus(const RE::FormID& pred, const RE::FormID& prey, const Locus& locus, const Locus& locusSource);
		bool CanBeRegurgitated(const VoreDataEntry* prey);
		void MoveToLocus(const RE::FormID& pred, const RE::FormID& prey, const Locus& locus, const Locus& locusSource = Locus::lNone);
		void SetPreyVisibility(RE::TESObjectREFR* preyObj, RE::Actor* pred, bool show, VoreDataEntry* preyData);
		void Swallow(RE::Actor* pred, std::vector<RE::TESObjectREFR*> preys, Locus locus, VoreDataEntry::VoreState ldType, bool fullswallow = false);
		void Swallow(RE::Actor* pred, RE::TESObjectREFR* prey, Locus locus, VoreDataEntry::VoreState ldType, bool fullswallow = false);
		void SwallowTarget(RE::Actor* pred, Locus locus, VoreDataEntry::VoreState ldType, bool fullswallow = false);
		void Regurgitate(RE::Actor* pred, std::vector<RE::FormID> preys, RegType rtype);
		void Regurgitate(RE::Actor* pred, RE::FormID prey, RegType rtype);
		void RegurgitateAll(RE::Actor* pred, Locus locus = Locus::lNone, RegType rtype = RegType::rNormal);
		void SetupTimers();
		void SetupBellies();
		void MegaDigest(const double& delta);
	}
}