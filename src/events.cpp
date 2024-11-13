#include "headers/events.h"
#include "headers/nutils.h"
#include "headers/settings.h"
#include "headers/times.h"
#include "headers/ui.h"
#include "headers/util.h"
#include "headers/voredata.h"
#include "headers/voremain.h"
#include "headers/vutils.h"

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

				if (keyCode == VoreSettings::k_vore_key) {
					Core::SwallowTarget(RE::PlayerCharacter::GetSingleton(), PlayerPrefs::voreLoc, PlayerPrefs::voreType);
				} else if (keyCode == VoreSettings::k_regurg_key) {
					Core::RegurgitateAll(RE::PlayerCharacter::GetSingleton(), PlayerPrefs::regLoc);
				}
				// menu actions
				else if (keyCode == VoreSettings::k_menu_1) {
					UI::VoreMenu::DoMenuAction(UI::MenuAction::kMenuA1);
				} else if (keyCode == VoreSettings::k_menu_2) {
					UI::VoreMenu::DoMenuAction(UI::MenuAction::kMenuA2);
				} else if (keyCode == VoreSettings::k_menu_3) {
					UI::VoreMenu::DoMenuAction(UI::MenuAction::kMenuA3);
				} else if (keyCode == VoreSettings::k_menu_4) {
					UI::VoreMenu::DoMenuAction(UI::MenuAction::kMenuA4);
				}
				// menu switching
				else if (keyCode == VoreSettings::k_i_menu) {
					UI::VoreMenu::SetMenuMode(UI::VoreMenuMode::kInfo);
				} else if (keyCode == VoreSettings::k_sw_menu) {
					UI::VoreMenu::SetMenuMode(UI::VoreMenuMode::kSwallow);
				} else if (keyCode == VoreSettings::k_test) {
					Log::PrintVoreData();
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

	RE::BSEventNotifyControl EventProcessor::ProcessEvent(const RE::TESDeathEvent* event, RE::BSTEventSource<RE::TESDeathEvent>*)
	{
		if (!event)
			return RE::BSEventNotifyControl::kContinue;
		if (event->dead && event->actorDying && event->actorDying.get()) {
			RE::FormID aId = event->actorDying->GetFormID();
			if (VoreData::IsValid(aId)) {
				VoreDataEntry& actorData = VoreData::Data[aId];
				RE::Actor* asActor = event->actorDying->As<RE::Actor>();
				actorData.aAlive = false;

				if (actorData.aEssential) {
					asActor->GetActorBase()->actorData.actorBaseFlags.set(RE::ACTOR_BASE_DATA::Flag::kEssential);
				}
				if (actorData.aProtected) {
					asActor->GetActorBase()->actorData.actorBaseFlags.set(RE::ACTOR_BASE_DATA::Flag::kProtected);
				}

				if (VoreData::IsPred(aId, true)) {
					Core::RegurgitateAll(asActor, lNone, Core::rAll);
				}

				if (std::find(VoreGlobals::delete_queue.begin(), VoreGlobals::delete_queue.end(), aId) != VoreGlobals::delete_queue.end()) {
					flog::warn("Trying to kill a deleted prey: {}", Name::GetName(actorData.get()));
					return RE::BSEventNotifyControl::kContinue;
				}
				if (actorData.pred) {
					Core::SwitchToDigestion(actorData.pred, actorData.pyLocus, VoreState::hSafe, false);
					actorData.pyDigestProgress = 0.0;
					actorData.pyElimLocus = actorData.pyLocus;
					actorData.pyLocusMovement = mStill;
				}
			}
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl EventProcessor::ProcessEvent([[maybe_unused]] const RE::TESWaitStartEvent* event, RE::BSTEventSource<RE::TESWaitStartEvent>*)
	{
		_waitHours = RE::Calendar::GetSingleton()->GetHoursPassed();
		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl EventProcessor::ProcessEvent([[maybe_unused]] const RE::TESWaitStopEvent* event, RE::BSTEventSource<RE::TESWaitStopEvent>*)
	{
		double delta = static_cast<double>(RE::Calendar::GetSingleton()->GetHoursPassed() - _waitHours);
		double timeScale = static_cast<double>(RE::Calendar::GetSingleton()->GetTimescale());
		flog::debug("WAIT: {}, {}", delta, timeScale);
		if (timeScale > 0) {
			Core::MegaDigest(delta * 3600 / timeScale);
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl EventProcessor::ProcessEvent([[maybe_unused]] const RE::TESSleepStartEvent* event, RE::BSTEventSource<RE::TESSleepStartEvent>*)
	{
		_sleepHours = RE::Calendar::GetSingleton()->GetHoursPassed();
		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl EventProcessor::ProcessEvent([[maybe_unused]] const RE::TESSleepStopEvent* event, RE::BSTEventSource<RE::TESSleepStopEvent>*)
	{
		double delta = static_cast<double>(RE::Calendar::GetSingleton()->GetHoursPassed() - _sleepHours);
		double timeScale = static_cast<double>(RE::Calendar::GetSingleton()->GetTimescale());
		flog::debug("SLEEP: {}, {}", delta, timeScale);
		if (timeScale > 0) {
			Core::MegaDigest(delta * 3600 / timeScale);
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl EventProcessor::ProcessEvent(const RE::TESFastTravelEndEvent* event, RE::BSTEventSource<RE::TESFastTravelEndEvent>*)
	{
		double delta = static_cast<double>(event->fastTravelEndHours);
		double timeScale = static_cast<double>(RE::Calendar::GetSingleton()->GetTimescale());
		flog::debug("TRAVEL: {}, {}", delta, timeScale);
		if (timeScale > 0) {
			Core::MegaDigest(delta * 3600 / timeScale);
		}
		UI::VoreMenu::SetMenuMode(UI::kDefault);
		return RE::BSEventNotifyControl::kContinue;
	}

	EventProcessor::EventProcessor(){};
	EventProcessor::~EventProcessor(){};
	EventProcessor::EventProcessor(const EventProcessor&){};
	EventProcessor::EventProcessor(EventProcessor&&){};
}