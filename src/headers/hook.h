#pragma once

#define RELOCATION_OFFSET(SE, AE) REL::VariantOffset(SE, AE, 0).offset()

namespace Vore::Hooks
{
	class Hook_MainUpdate
	{
	public:
		static void Hook(SKSE::Trampoline& trampoline);

	private:
		static void Update(RE::Main* a_this, float a2);
		static inline REL::Relocation<decltype(Update)> _Update;
	};

	void InstallControls();
	void Install();
}