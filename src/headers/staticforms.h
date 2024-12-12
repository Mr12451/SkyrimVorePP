#pragma once

namespace Vore
{
	class StaticForms
	{
	public:
		static void Initialize();

		static inline RE::TESFaction* potential_follower{ nullptr };
	};
}