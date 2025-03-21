#include "headers/events.h"
#include "headers/nutils.h"
#include "headers/settings.h"
#include "headers/times.h"
#include "headers/ui.h"
#include "headers/util.h"
#include "headers/voredata.h"
#include "headers/voremain.h"
#include "headers/vutils.h"
#include "headers/sounds.h"

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
					VoreDataEntry* playerData = VoreData::IsValidGet(RE::PlayerCharacter::GetSingleton()->GetFormID());
					if (playerData && playerData->pred) {
						if (playerData->pyDigestProgress == 100.0) {
							Core::FinishPlayerDigestion();
						}
					} else {
						Core::SwallowTarget(RE::PlayerCharacter::GetSingleton(), PlayerPrefs::voreLoc, PlayerPrefs::voreType);
					}
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
					flog::info("pd lvl {}", VoreData::GetStatOrMake(RE::PlayerCharacter::GetSingleton())->predLevel);
					flog::info("pd xp {}", VoreData::GetStatOrMake(RE::PlayerCharacter::GetSingleton())->predXp);
					flog::info("pd t {}", VoreData::GetStatOrMake(RE::PlayerCharacter::GetSingleton())->predThreshold);
					flog::info("py lvl {}", VoreData::GetStatOrMake(RE::PlayerCharacter::GetSingleton())->preyLevel);
					flog::info("py xp {}", VoreData::GetStatOrMake(RE::PlayerCharacter::GetSingleton())->preyXp);
					flog::info("py t {}", VoreData::GetStatOrMake(RE::PlayerCharacter::GetSingleton())->preyThreshold);
					//Log::PrintVoreData();
					//Funcs::MoveTo(RE::PlayerCharacter::GetSingleton(), nullptr);
					/*for (auto& i : RE::PlayerCharacter::GetSingleton()->addedSpells) {
						flog::info("found spell {}", i->GetFullName());
					}*/
				}

				break;
			}
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	// Log information about Menu open/close events that happen in the game
	RE::BSEventNotifyControl EventProcessor::ProcessEvent(const RE::MenuOpenCloseEvent*, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		//if (event->menuName == "GiftMenu" && VoreGlobals::gift_menu_target) {
		//	if (event->opening) {
		//		flog::info("Opened vore GiftMenu");
		//	} else {
		//		flog::info("Closed vore GiftMenu");
		//		VoreGlobals::gift_menu_target = RE::ActorHandle();
		//		VoreGlobals::gift_menu_ic = RE::ObjectRefHandle();
		//	}
		//}
		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl EventProcessor::ProcessEvent(const RE::TESDeathEvent* event, RE::BSTEventSource<RE::TESDeathEvent>*)
	{
		if (!event)
			return RE::BSEventNotifyControl::kContinue;
		if (event->dead && event->actorDying && event->actorDying.get()) {
			RE::FormID aId = event->actorDying->GetFormID();
			if (VoreDataEntry* actorData = VoreData::IsValidGet(aId)) {
				RE::Actor* asActor = event->actorDying->As<RE::Actor>();
				actorData->aAlive = false;

				if (actorData->aEssential) {
					asActor->GetActorBase()->actorData.actorBaseFlags.set(RE::ACTOR_BASE_DATA::Flag::kEssential);
				}
				if (actorData->aProtected) {
					asActor->GetActorBase()->actorData.actorBaseFlags.set(RE::ACTOR_BASE_DATA::Flag::kProtected);
				}

				if (VoreData::IsPred(aId, true)) {
					Core::RegurgitateAll(asActor, lNone, Core::rAll);
				}

				if (VoreGlobals::delete_queue.contains(aId)) {
					flog::warn("Trying to kill a deleted prey: {}", Name::GetName(actorData->get()));
					return RE::BSEventNotifyControl::kContinue;
				}
				if (actorData->pred) {
					actorData->pyElimLocus = actorData->pyLocus;
					actorData->pyLocusMovement = VoreDataEntry::mStill;
					
					//
					if (actorData->get()->IsDragon() && actorData->pred == RE::PlayerCharacter::GetSingleton()->GetFormID()) {
						AV::ModAV(RE::PlayerCharacter::GetSingleton(), RE::ActorValue::kDragonSouls, 1.0);
					}
					//TODO dragon quest progression?
					//idk what's that
					if (VoreDataEntry* predData = VoreData::IsValidGet(actorData->pred)) {
						predData->PlayScream(actorData);
						predData->EmoteSmile(5000);
						predData->UpdateStats(true);
					}
					actorData->UpdateStats(false);
					actorData->SetMyDigestion(VoreDataEntry::hLethal, true);
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


	RE::BSEventNotifyControl EventProcessor::ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>*)
	{
		if (!a_event || !a_event->actor || a_event->actor->GetFormType() != RE::FormType::ActorCharacter) {
			return RE::BSEventNotifyControl::kContinue;
		}
		RE::AlchemyItem* alchemyItem = RE::TESForm::LookupByID<RE::AlchemyItem>(a_event->baseObject);
		if (alchemyItem) {
			RE::Actor* actorA = a_event->actor->As<RE::Actor>();
			if (VoreSettings::fake_food_player && actorA->IsPlayerRef() || VoreSettings::fake_food_team && actorA->IsPlayerTeammate()) {
				Core::AddFakeFood(a_event->actor->As<RE::Actor>(), alchemyItem);
			} else if (CalcWgEnabled(actorA)) {
				Core::InstantWgItem(actorA, alchemyItem);
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	EventProcessor::EventProcessor(){};
	EventProcessor::~EventProcessor(){};
	EventProcessor::EventProcessor(const EventProcessor&){};
	EventProcessor::EventProcessor(EventProcessor&&){};


	void VEventProcessor::Hook()
	{
		flog::info("Hooking footstep event");
		REL::Relocation<std::uintptr_t> VtblImpact{ RE::VTABLE_BGSImpactManager[0] };
		_ProcessFootstepEvent = VtblImpact.write_vfunc(0x01, ProcessFootstepEvent);
		//flog::info("Hooking gift menu events");
		/*REL::Relocation<std::uintptr_t> VtblGift{ RE::VTABLE_IGiftMenuScriptCallback[0] };
		const auto a_idx = 0x01;
		const auto addr = VtblGift.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<void(RE::GiftMenu*)> func(result);
		func(this);*/
		//auto g_menu = RE::UI::GetSingleton()->GetMenu<RE::GiftMenu>();
		//CallbackProcessor
	}

	RE::BSEventNotifyControl VEventProcessor::ProcessFootstepEvent(RE::BGSImpactManager* a_this, const RE::BGSFootstepEvent* a_event, RE::BSTEventSource<RE::BGSFootstepEvent>* a_eventSource)
	{
		//flog::info("Footstep event {}", a_event->actor.get()->GetDisplayFullName());
		if (a_event->tag == "FootRight") {
			if (VoreDataEntry* predData = VoreData::IsValidGet(a_event->actor.get()->GetFormID())) {;
				if (predData->pdFullBurden > 30.0f) {
					predData->PlaySound(Sounds::FootstepSlosh, std::min(predData->pdFullBurden / 100.0f, 1.0f));
				}
			} 
		}

		auto result = _ProcessFootstepEvent(a_this, a_event, a_eventSource);
		return result;
	}
}