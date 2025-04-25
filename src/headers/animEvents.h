#pragma once

namespace Vore
{
	class AnimEvenHandler
	{
	private:
		struct ProcessAnimEvent
		{
			static void thunk(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_src);
			static inline REL::Relocation<decltype(thunk)> func;

			static inline constexpr std::size_t index{ 2 };
			static inline constexpr std::size_t size{ 0x1 };
		};
		static void EPlaySwallow(const RE::TESObjectREFR* pred);
	public:
		static void Initialize();
	};
}