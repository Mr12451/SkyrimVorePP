#include "headers/animEvents.h"
#include "headers/hook.h"
#include "headers/voredata.h"

namespace Vore
{
	void AnimEvenHandler::ProcessAnimEvent::thunk(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_src)
	{
		func(a_this, a_event, a_src);
		std::string_view tagName( a_event.tag );
		if (tagName.substr(0, 4) == "SVPP") {
			if (tagName == "SVPP_ESwallow") {
				EPlaySwallow(a_event.holder);
			}
		}
		//flog::info("{} {}", a_event.tag.c_str(), a_event.payload.c_str());
	}

	void AnimEvenHandler::EPlaySwallow(const RE::TESObjectREFR* pred)
	{
		if (pred) {
			if (VoreDataEntry* predData = VoreData::IsValidGet(pred->GetFormID())) {
				predData->DoSwallow();
			}
		}
	}
	void AnimEvenHandler::Initialize()
	{
		Hooks::write_vtable_func<RE::Character, ProcessAnimEvent>();
		Hooks::write_vtable_func<RE::PlayerCharacter, ProcessAnimEvent>();
		flog::info("Hooked Animation Events");
	}
}