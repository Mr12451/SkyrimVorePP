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
		void SwitchToDigestion(RE::FormID pred, Locus locus, VoreState dType, bool forceStopDigestion);
		bool CanMoveToLocus(RE::FormID pred, RE::FormID prey, Locus locus, Locus locusSource);
		bool CanBeRegurgitated(VoreDataEntry& prey);
		void MoveToLocus(RE::FormID pred, RE::FormID prey, Locus locus, Locus locusSource = Locus::lNone);
		void Swallow(RE::Actor* pred, std::vector<RE::TESObjectREFR*> preys, Locus locus, VoreState ldType, bool fullswallow = false);
		void Swallow(RE::Actor* pred, RE::TESObjectREFR* prey, Locus locus, VoreState ldType, bool fullswallow = false);
		void SwallowTarget(RE::Actor* pred, Locus locus, VoreState ldType, bool fullswallow = false);
		void Regurgitate(RE::Actor* pred, std::vector<RE::FormID> preys, RegType rtype);
		void Regurgitate(RE::Actor* pred, RE::FormID prey, RegType rtype);
		void RegurgitateAll(RE::Actor* pred, Locus locus = Locus::lNone, RegType rtype = RegType::rNormal);
		void SetupTimers();
	}
}