#include "headers/plugins.h"

namespace Vore
{
	bool Plugin::Enabled()
	{
		return enabled.load();
	}

	bool Plugin::InGame()
	{
		auto* ui = RE::UI::GetSingleton();
		if (!ui) {
			return false;
		}
		if (ui->IsMenuOpen(RE::MainMenu::MENU_NAME)) {
			return false;
		}

		return true;
	}
	void Plugin::SetInGame(bool value)
	{
		Plugin::ingame.store(value);
	}

	bool Plugin::Ready()
	{
		if (Plugin::InGame()) {
			// We are not loading or in the mainmenu
			auto* player_char = RE::PlayerCharacter::GetSingleton();
			if (player_char) {
				if (player_char->Is3DLoaded()) {
					// Player is loaded
					return true;
				}
			}
		}
		return false;
	}

	bool Plugin::Live()
	{
		if (Plugin::Ready()) {
			auto* ui = RE::UI::GetSingleton();
			if (!ui->GameIsPaused()) {
				// Not paused
				return true;
			}
		}
		return false;
	}

	bool Plugin::OnMainThread()
	{
		return onmainthread.load();
	}
	void Plugin::SetOnMainThread(bool value)
	{
		Plugin::onmainthread.store(value);
	}
}