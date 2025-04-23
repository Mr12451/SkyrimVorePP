#pragma once

namespace Vore
{
	class StaticForms
	{
	public:
		static void Initialize();

		static inline RE::TESFaction* potential_follower{ nullptr };
		static inline RE::TESFaction* f_skyrim_pred{ nullptr };
		static inline RE::TESFaction* f_skyrim_prey{ nullptr };

		static inline RE::TESFaction* f_giant{ nullptr };
		static inline RE::TESFaction* f_mammoth{ nullptr };

		static inline RE::TESFaction* f_vampire{ nullptr };
		static inline RE::TESFaction* f_vampireDLC{ nullptr };
		static inline RE::TESFaction* f_vampireSerana{ nullptr };
		static inline RE::TESFaction* f_companions{ nullptr };
		static inline RE::TESFaction* f_hag{ nullptr };

		static inline RE::TESFaction* f_daedra{ nullptr };
		static inline RE::TESFaction* f_dwarvenrobot{ nullptr };

		static inline RE::TESFaction* f_skeleton{ nullptr };
		static inline RE::TESFaction* f_ghost{ nullptr };
		static inline RE::TESFaction* f_whisp{ nullptr };

		static inline RE::TESRace* r_arg{ nullptr };
		static inline RE::TESRace* r_bre{ nullptr };
		static inline RE::TESRace* r_del{ nullptr };
		static inline RE::TESRace* r_hel{ nullptr };
		static inline RE::TESRace* r_imp{ nullptr };
		static inline RE::TESRace* r_kha{ nullptr };
		static inline RE::TESRace* r_nor{ nullptr };
		static inline RE::TESRace* r_orc{ nullptr };
		static inline RE::TESRace* r_red{ nullptr };
		static inline RE::TESRace* r_wel{ nullptr };

		static inline RE::TESRace* r_old{ nullptr };

	};
}