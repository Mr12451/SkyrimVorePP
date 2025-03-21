#pragma once

namespace Vore
{
	class Dist
	{
	private:
		static inline RE::TESFaction* f_predator = nullptr;
		static inline RE::TESFaction* f_prey = nullptr;
	public:
		static int8_t GetPRank(bool isPred, RE::Actor* target);
		static void SetPRank(bool isPred, RE::Actor* target, int8_t rank);
		static void DistrNPC(RE::Character* target);
		static void Initialize();

	};
}