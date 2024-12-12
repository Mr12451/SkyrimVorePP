#include "headers/staticforms.h"

namespace Vore
{
	void StaticForms::Initialize()
	{
		potential_follower = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x5C84D, "Skyrim.esm");
		if (potential_follower) {
			flog::error("found potential_follower TESFaction");
		}
	}
}
