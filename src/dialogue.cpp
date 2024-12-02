#include "headers/dialogue.h"
#include "headers/voredata.h"
#include "headers/voremain.h"
#include "headers/nutils.h"

#include <thread>
#include <chrono>

namespace Vore
{
	void Dialogue::TalkToA(RE::Actor* target)
	{
		if (!target->IsHumanoid()) {
			flog::info("can't talk to non-humanoid npcs");
			return;
		}
		bool isPrey = VoreData::IsPrey(target->GetFormID());

		// start waiting for dialogue
		VoreDataEntry* vData = VoreData::IsValidGet(target->GetFormID());
		if (vData) {
			vData->aDialogue = VoreDataEntry::kWaiting;
			vData->aDialogueTimeout = 0.0;
			vData->CalcFast();
		} else {
			return;
		}

		// if prey, move to pred
		if (isPrey && !target->IsDead()) {

			target->MoveTo(RE::PlayerCharacter::GetSingleton());
			Core::HidePrey(target);
		}
		RE::ActorHandle myHandle = target->GetHandle();

		auto t = std::thread([myHandle]() {
			std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
			SKSE::GetTaskInterface()->AddTask([myHandle]() {
				if (myHandle) {

					VoreDataEntry* preyData = VoreData::IsValidGet(myHandle.get()->GetFormID());
					if (preyData) {
						if (myHandle.get()->IsDead()) {
							preyData->aDialogue = VoreDataEntry::kFail;
						} else {
							myHandle.get()->ActivateRef(RE::PlayerCharacter::GetSingleton(), 0, nullptr, 0, true);

						}
					}
					
				}
			});
			std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });

			SKSE::GetTaskInterface()->AddTask([myHandle]() {
				if (myHandle) {
					VoreDataEntry* preyData = VoreData::IsValidGet(myHandle.get()->GetFormID());
					if (preyData) {
						if (Funcs::IsInDialogueWithPlayer(myHandle.get().get())) {
							preyData->aDialogue = VoreDataEntry::kDialogue;
							flog::trace("DIALOGUE SUCCESS");
						} else {
							preyData->aDialogue = VoreDataEntry::kFail;
							flog::trace("dialogue fail");
						}
					}
				}
			});
		});
		t.detach();
		
	}
}