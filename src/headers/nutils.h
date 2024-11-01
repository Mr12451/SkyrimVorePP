#pragma once

namespace Vore
{
	namespace Utils
	{

		std::vector<RE::Actor*> FindActors();

		std::vector<RE::TESObjectREFR*> GetFrontObjects(RE::Actor* source, size_t maxActors);

		RE::TESObjectREFR* GetCrosshairObject();
	}
	namespace AV
	{
		double GetMaxAV(RE::Actor* actor, RE::ActorValue av);
		double GetAV(RE::Actor* actor, RE::ActorValue av);
		void ModAV(RE::Actor* actor, RE::ActorValue av, double amount);
		void SetAV(RE::Actor* actor, RE::ActorValue av, double amount);
		void DamageAV(RE::Actor* actor, RE::ActorValue av, double amount);
		double GetPercentageAV(RE::Actor* actor, RE::ActorValue av);
		void SetPercentageAV(RE::Actor* actor, RE::ActorValue av, double target);
	}
	namespace Funcs
	{
		void SetGhost(RE::Actor* actor, bool param);
		void StartCombat(RE::Actor* agressor, RE::Actor* victim);
		void ApplyDamage(RE::Actor* dealer, RE::Actor* receiver, float damage);
		void SetRestrained(RE::Actor* actor, bool param);
		void SetAlpha(RE::Actor* actor, float alpha, bool fade);
		void StopCombat(RE::Actor* actor);
		void StopCombatAlarm(RE::Actor* actor);
	}
	namespace Name
	{
		const char* GetName(RE::TESObjectREFR* form);
		const char* GetName(RE::FormID form);
	}
}
