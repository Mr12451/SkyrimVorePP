#include "headers/events.h"
#include "headers/util.h"
#include "headers/voredata.h"
#include "headers/voremain.h"
#include "headers/times.h"
#include "headers/vutils.h"
#include "headers/settings.h"
#include "headers/ui.h"

namespace Vore
{
	// Returns a reference to the one and only instance of EventProcessor :)
	//
	// Note: this is returned as a & reference. When you need this as a pointer, you'll want to use & (see below)
	EventProcessor& EventProcessor::GetSingleton()
	{
		static EventProcessor singleton;
		return singleton;
	}

	// Log information about Activate events that happen in the game
	RE::BSEventNotifyControl EventProcessor::ProcessEvent(const RE::TESActivateEvent* event, RE::BSTEventSource<RE::TESActivateEvent>*)
	{
		flog::info("{} activated {}", event->actionRef.get()->GetBaseObject()->GetName(),
			event->objectActivated.get()->GetBaseObject()->GetName());
		return RE::BSEventNotifyControl::kContinue;
	}

	//on input
	RE::BSEventNotifyControl EventProcessor::ProcessEvent(const InputEvents* event, RE::BSTEventSource<InputEvents>*)
	{
		if (!*event)
			return RE::BSEventNotifyControl::kContinue;
		for (RE::InputEvent* e = *event; e; e = e->next) {
			switch (e->eventType.get()) {
			case RE::INPUT_EVENT_TYPE::kButton:
				RE::ButtonEvent* a_event = e->AsButtonEvent();
				if (!a_event->IsDown()) {
					return RE::BSEventNotifyControl::kContinue;
				}

				uint32_t keyMask = a_event->idCode;
				uint32_t keyCode;

				// Mouse
				if (a_event->device.get() == RE::INPUT_DEVICE::kMouse) {
					keyCode = (uint32_t)KeyUtil::KBM_OFFSETS::kMacro_MouseButtonOffset + keyMask;
				}
				// Gamepad
				else if (a_event->device.get() == RE::INPUT_DEVICE::kGamepad) {
					keyCode = (uint32_t)KeyUtil::KBM_OFFSETS::kMacro_GamepadOffset + keyMask;
				}
				// Keyboard
				else
					keyCode = keyMask;

				//
				//
				// info("Key pressed: {}", keyCode);

				if (keyCode == VoreSettings::endo_key) {
					Vore::Core::SwallowTarget(RE::PlayerCharacter::GetSingleton(), Locus::lStomach, VoreState::hSafe, false);
				} else if (keyCode == VoreSettings::vore_key) {
					Vore::Core::SwallowTarget(RE::PlayerCharacter::GetSingleton(), Locus::lStomach, VoreState::hLethal, false);
				} else if (keyCode == VoreSettings::heal_key) {
					Vore::Core::SwallowTarget(RE::PlayerCharacter::GetSingleton(), Locus::lStomach, VoreState::hReformation, false);
				} else if (keyCode == VoreSettings::regurg_key) {
					Vore::Core::RegurgitateAll(RE::PlayerCharacter::GetSingleton());
				} else if (keyCode == VoreSettings::check_time_key) {
					//Prints vore data;
					Log::PrintVoreData();
				} else if (keyCode == VoreSettings::test_key) {
					//Prints vore data;
					Scaleform::VoreMenu::ShowShow();
				}

				break;
			}
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	// Log information about Menu open/close events that happen in the game
	RE::BSEventNotifyControl EventProcessor::ProcessEvent(const RE::MenuOpenCloseEvent* event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		flog::info("Menu {} Open? {}", event->menuName.c_str(), event->opening);
		return RE::BSEventNotifyControl::kContinue;
	}

	EventProcessor::EventProcessor(){};
	EventProcessor::~EventProcessor(){};
	EventProcessor::EventProcessor(const EventProcessor&){};
	EventProcessor::EventProcessor(EventProcessor&&){};
}