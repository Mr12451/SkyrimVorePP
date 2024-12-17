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

	template <class F, class T>
	void write_vtable_func()
	{
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[T::index] };
		T::func = vtbl.write_vfunc(T::size, T::thunk);
	}


	//class Hook_ItemRemove
	//{
	//public:

	//	// Update after RemoveItem
	//	static RE::ObjectRefHandle& RemoveItem_Hook(RE::TESObjectREFR* a_this, RE::ObjectRefHandle& a_handle, RE::TESBoundObject* a_item, std::int32_t a_count, RE::ITEM_REMOVE_REASON a_reason, RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef, const RE::NiPoint3* a_dropLoc = 0, const RE::NiPoint3* a_rotate = 0);
	//	static inline REL::Relocation<decltype(RemoveItem_Hook)> _RemoveItem;
	//	static void Hook(SKSE::Trampoline& trampoline);
	//};

	void InstallControls();
	void Install();
}