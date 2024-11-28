#include "headers/sounds.h"
#include "headers/settings.h"

namespace Vore
{
	void Sounds::InitSounds()
	{
		RE::BGSSoundOutput* t = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundOutput>(0x9F8, "Devourment.esp");
		if (!t) {
			flog::info("No Devourment Plugin, sounds are disabled");
			return;

		}
		if (!VoreSettings::enable_sound) {
			flog::info("Sounds are disabled in config");
			return;
		}
		flog::info("Found Devourment Plugin, enabling sounds");
		AllowSounds = true;
		Bones_F = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0x8BF, "Devourment.esp");
		Bones_M = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xA3D, "Devourment.esp");
		Burp = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0x8FB, "Devourment.esp");
		CockVore = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xBD6, "Devourment.esp");
		FootstepSlosh = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xC2E, "Devourment.esp");
		Gurgle = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0x970, "Devourment.esp");
		Poop = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0x8FE, "Devourment.esp");
		Poop_F = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0x9FC, "Devourment.esp");
		Poop_M = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0x9FD, "Devourment.esp");
		Screamless_F = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xA95, "Devourment.esp");
		Screamless_M = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xA96, "Devourment.esp");
		Scream_A = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0x98D, "Devourment.esp");
		Scream_C = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xBCB, "Devourment.esp");
		Scream_F = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xCC8, "Devourment.esp");
		Scream_M = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xCC7, "Devourment.esp");
		Scream_U = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xBC9, "Devourment.esp");
		Stomach = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0x8FC, "Devourment.esp");
		Struggling_CF = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xBC3, "Devourment.esp");
		Struggling_CM = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xBC6, "Devourment.esp");
		Struggling_FF = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xA00, "Devourment.esp");
		Struggling_FM = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xA01, "Devourment.esp");
		Struggling_MF = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xA02, "Devourment.esp");
		Struggling_MM = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0xA03, "Devourment.esp");
		Swallow = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0x8FA, "Devourment.esp");
		Vomit = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSSoundDescriptorForm>(0x8FD, "Devourment.esp");
		//std::stoul("0008FA", nullptr, 16)
		//::LookupByEditorID<RE::TESSound>("DevourmentMarker_Swallow");

		if (!FootstepSlosh) {
			flog::error("Sounds are broken!");
			AllowSounds = false;
		}
		//testSw->


	}
}

