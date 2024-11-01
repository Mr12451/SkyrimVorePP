#pragma once

// from gts
namespace Vore
{
	class Plugin
	{
	public:
		static bool Enabled();
		static bool InGame();
		static bool Ready();
		static bool Live();
		static void SetInGame(bool value);
		static bool OnMainThread();
		static void SetOnMainThread(bool value);

	private:
		static inline std::atomic_bool enabled = std::atomic_bool(true);
		static inline std::atomic_bool ingame = std::atomic_bool(false);
		static inline std::atomic_bool onmainthread = std::atomic_bool(false);
	};
}
