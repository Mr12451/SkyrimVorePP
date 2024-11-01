#pragma once

#include "headers/voredata.h"

namespace Vore
{
	namespace Core
	{
		enum RegType : uint8_t
		{
			rNormal,
			rTransfer,
			rAll
		};
		void SwitchToDigestion(RE::FormID pred, Locus locus, VoreState dType, bool forceStopDigestion);
		bool CanMoveToLocus(RE::FormID pred, RE::FormID prey, Locus locus, Locus locusSource);
		void MoveToLocus(RE::FormID pred, RE::FormID prey, Locus locus, Locus locusSource = Locus::lNone);
		void Swallow(RE::Actor* pred, std::vector<RE::TESObjectREFR*> preys, Locus locus, VoreState ldType, bool fullswallow);
		void Swallow(RE::Actor* pred, RE::TESObjectREFR* prey, Locus locus, VoreState ldType, bool fullswallow);
		void SwallowTarget(RE::Actor* pred, Locus locus, VoreState ldType, bool fullswallow);
		void Regurgitate(RE::Actor* pred, std::vector<RE::FormID> preys, RegType rtype);
		void Regurgitate(RE::Actor* pred, RE::FormID prey, RegType rtype);
		void RegurgitateAll(RE::Actor* pred, Locus locus = Locus::lNone, RegType rtype = RegType::rNormal);
		void KillPrey(RE::FormID prey, RE::Actor* asActor);
		void SetupTimers();
	}
}