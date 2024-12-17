#include "headers/staticforms.h"

namespace Vore
{
	void StaticForms::Initialize()
	{
		potential_follower = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x5C84D, "Skyrim.esm");
		f_skyrim_pred = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x2E893, "Skyrim.esm");
		f_skyrim_prey = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x2E894, "Skyrim.esm");

		f_giant = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x4359A, "Skyrim.esm");
		f_mammoth = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x4359B, "Skyrim.esm");

		f_vampire = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x27242, "Skyrim.esm");
		f_vampireDLC = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x3376, "Dawnguard.esm");
		f_vampireSerana = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x183A5, "Dawnguard.esm");
		f_companions = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0xFF234, "Skyrim.esm");
		f_hag = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x4359E, "Skyrim.esm");

		f_daedra = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x2B0E5, "Skyrim.esm");
		f_dwarvenrobot = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x43598, "Skyrim.esm");

		f_skeleton = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x2D1DF, "Skyrim.esm");
		f_ghost = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0xD33A2, "Skyrim.esm");
		f_whisp = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x3E096, "Skyrim.esm");
	}
}
