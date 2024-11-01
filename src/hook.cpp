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
		//from tdm source
		REL::Relocation<uintptr_t> hook{ REL::RelocationID(35565, 36564) };  // 5B2FF0, 5D9F50, main update
		flog::info("Vore is applying Main Update Hook at {:X}", hook.address());
		_Update = trampoline.write_call<5>(hook.address() + RELOCATION_OFFSET(0x748, 0xC26), Update);  // 5B3738, 5DAB76

	}

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
		//Offsets::SetOffsets();
	}
}