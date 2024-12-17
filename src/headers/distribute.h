#pragma once

namespace Vore
{
	class Dist
	{
	private:
		static inline RE::TESFaction* f_predator = nullptr;
		static inline RE::TESFaction* f_prey = nullptr;
	public:

		static void DistrNPC(RE::Character* target);
		static void Initialize();

	};
}