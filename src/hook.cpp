#include "headers/hook.h"
#include "headers/plugins.h"
#include "headers/times.h"
#include "headers/voremain.h"

namespace Vore::Hooks
{

	void Hook_MainUpdate::Update(RE::Main* a_this, float a2)
	{
		_Update(a_this, a2);
		static std::atomic_bool started = std::atomic_bool(false);
		Plugin::SetOnMainThread(true);

		if (Plugin::Live()) {
			//We are not loading or in the mainmenu
			//Player loaded and not paused
			if (started.exchange(true)) {
				//Not first updated
				Time::Update();
				//EventDispatcher::DoUpdate();
			} else {
				// First update this load
				//EventDispatcher::DoStart();
				flog::trace("First Update");
				Core::SetupBellies();
				Core::SetupTimers();
			}
		} else if (!Plugin::InGame()) {
			//Loading or in main menu
			//flog::trace("In menu");
			started.store(false);
		}
		Plugin::SetOnMainThread(false);
	}

	void Hook_MainUpdate::Hook(SKSE::Trampoline& trampoline)
	{
		//from TrueDirectionalMovement
		//REL::Relocation<uintptr_t> hook{ REL::RelocationID(35565, 36564) };  // 5B2FF0, 5D9F50, main update
		REL::Relocation<uintptr_t> target{ REL::ID(36564), 0xC26 };  // 5B2FF0, 5D9F50, main update
		flog::info("Vore is applying Main Update Hook at {:X}", target.address());
		_Update = trampoline.write_call<5>(target.address(), &Update);  // 5B3738, 5DAB76

	}

	//RE::ObjectRefHandle& Hook_ItemRemove::RemoveItem_Hook(RE::TESObjectREFR* a_this, RE::ObjectRefHandle& a_handle, RE::TESBoundObject* a_item, std::int32_t a_count, RE::ITEM_REMOVE_REASON a_reason, RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef, const RE::NiPoint3* a_dropLoc, const RE::NiPoint3* a_rotate)
	//{
	//	flog::info("Item removed {}", a_item->GetName());
	//	return _RemoveItem(a_this, a_handle, a_item, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
	//}
	//void Hook_ItemRemove::Hook(SKSE::Trampoline& trampoline)
	//{
	//	// remove item hook from SkyrimSouls
	//	REL::Relocation<uintptr_t> RemoveItem_Hook1{ REL::ID(22002), 0x9A }; // + 0x9A (6 call) - Actor - when called from Console (Drop)
	//	REL::Relocation<uintptr_t> RemoveItem_Hook2{ REL::ID(56125), 0xDB }; // + 0xDB (6 call) - Actor - when called from Payprus (DropObject)
	//	REL::Relocation<uintptr_t> RemoveItem_Hook3{ REL::ID(24715), 0x472 }; // + 0x472 (6 call) - Actor - when other NPCs interact
	//	REL::Relocation<uintptr_t> RemoveItem_Hook4{ REL::ID(17887), 0x26A }; // + 0x26A (6 call) - TESObjectREFR - when other NPCs interact
	//	trampoline.write_call<6>(RemoveItem_Hook1.address(), &RemoveItem_Hook);
	//	trampoline.write_call<6>(RemoveItem_Hook2.address(), &RemoveItem_Hook);
	//	trampoline.write_call<6>(RemoveItem_Hook3.address(), &RemoveItem_Hook);
	//	trampoline.write_call<6>(RemoveItem_Hook4.address(), &RemoveItem_Hook);
	//}

	//hooks for player controls
	void InstallControls()
	{
	}

	//main hooks
	void Install()
	{
		flog::info("Installing hooks");
		SKSE::AllocTrampoline(64);
		auto& trampoline = SKSE::GetTrampoline();
		//trampoline.create(64);
		Hook_MainUpdate::Hook(trampoline);
		//Hook_ItemRemove::Hook(trampoline);
		//Offsets::SetOffsets();
	}
}