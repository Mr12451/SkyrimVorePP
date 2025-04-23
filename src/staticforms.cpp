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
	
		r_arg = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESRace>(0x13740, "Skyrim.esm");
		r_bre = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESRace>(0x13741, "Skyrim.esm");
		r_del = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESRace>(0x13742, "Skyrim.esm");
		r_hel = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESRace>(0x13743, "Skyrim.esm");
		r_imp = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESRace>(0x13744, "Skyrim.esm");
		r_kha = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESRace>(0x13745, "Skyrim.esm");
		r_nor = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESRace>(0x13746, "Skyrim.esm");
		r_orc = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESRace>(0x13747, "Skyrim.esm");
		r_red = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESRace>(0x13748, "Skyrim.esm");
		r_wel = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESRace>(0x13749, "Skyrim.esm");
		
		r_old = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESRace>(0x67CD8, "Skyrim.esm");
	}
}
