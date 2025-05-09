#include "headers/dialogue.h"
#include "headers/nutils.h"
#include "headers/settings.h"
#include "headers/voredata.h"
#include "headers/voremain.h"

#include <chrono>
#include <thread>

namespace Vore
{

	static float LocusToDevLocus(Locus loc)
	{
		switch (loc) {
		case (Locus::lStomach):
			return 0.0f;
		case (Locus::lBowel):
			return 1.0f;
		case (Locus::lWomb):
			return 2.0;
		case (Locus::lBreastl):
			return 3.0;
		case (Locus::lBreastr):
			return 4.0;
		case (Locus::lBalls):
			return 5.0;
		}
		return 0.0f;
	}

	void Dialogue::SetupBeforeDialogue()
	{
		if (plugin_loaded) {
			VoreDataEntry* vDataNPC = VoreData::IsValidGet(_handleNPC.get()->GetFormID());
			VoreDataEntry* vDataPlayer = VoreData::IsValidGet(_handlePlayer.get()->GetFormID());

			if (vDataPlayer && vDataPlayer->pred) {
				Funcs::ForceQuestAlias(_dialogueQuest, "DialoguePreyAlias", _handlePlayer.get().get());
				Funcs::ForceQuestAlias(_dialogueQuest, "DialoguePredAlias", _handleNPC.get().get());

				g_lethal->value = vDataPlayer->pyDigestion == VoreDataEntry::hLethal ? 1.0f : 0.0f;
				g_locus->value = LocusToDevLocus(vDataPlayer->pyLocus);
			} else if (vDataNPC && vDataNPC->pred) {
				Funcs::ForceQuestAlias(_dialogueQuest, "DialoguePredAlias", _handlePlayer.get().get());
				Funcs::ForceQuestAlias(_dialogueQuest, "DialoguePreyAlias", _handleNPC.get().get());

				g_lethal->value = vDataNPC->pyDigestion == VoreDataEntry::hLethal ? 1.0f : 0.0f;
				g_locus->value = LocusToDevLocus(vDataNPC->pyLocus);
			}
		}
	}

	void Dialogue::ClearAfterDialogue()
	{
		_running.store(kNone);

		VoreDataEntry* vDataNPC = VoreData::IsValidGet(_handleNPC.get()->GetFormID());
		if (vDataNPC) {
			vDataNPC->aDialogue = false;
			vDataNPC->CalcFast();
			if (vDataNPC->pred) {
				Core::SetPreyVisibility(_handleNPC.get().get(), nullptr, false, nullptr);
			}
		}

		VoreDataEntry* vDataPlayer = VoreData::IsValidGet(_handlePlayer.get()->GetFormID());
		if (vDataPlayer) {
			vDataPlayer->aDialogue = false;
			vDataPlayer->CalcFast();
		}

		flog::info("Dialogue ended");

		_handlePlayer = RE::ActorHandle();
		_handleNPC = RE::ActorHandle();

		if (plugin_loaded) {
			Funcs::ForceQuestAlias(_dialogueQuest, "DialoguePredAlias", nullptr);
			Funcs::ForceQuestAlias(_dialogueQuest, "DialoguePreyAlias", nullptr);
		}
	}

	void Dialogue::OnSwallow_Pred(RE::Actor* pred)
	{
		if (!pred || !plugin_loaded) {
			return;
		}
		if (!pred->HasSpell(s_fullness)) {
			pred->AddSpell(s_fullness);
		}
	}

	void Dialogue::OnSwallow_Prey(RE::Actor* pred, RE::TESObjectREFR* prey)
	{
		if (!prey || !plugin_loaded || prey->GetFormType() != RE::FormType::ActorCharacter) {
			return;
		}
		RE::Actor* preyA = prey->As<RE::Actor>();

		if (!preyA->HasSpell(s_notThere)) {
			preyA->AddSpell(s_notThere);
		}
		if (!pred->IsPlayerRef() && !preyA->IsPlayerRef() && Utils::AreFriends(RE::PlayerCharacter::GetSingleton(), preyA, RE::BGSRelationship::RELATIONSHIP_LEVEL::kFriend)) {
			if (!pred->HasSpell(s_statusSwalFol)) {
				pred->AddSpell(s_statusSwalFol);
			}
		}
	}

	void Dialogue::Clear_Pred(RE::Actor* pred)
	{
		if (!pred || !plugin_loaded) {
			return;
		}

		if (VoreDataEntry* predData = VoreData::IsValidGet(pred->GetFormID())) {
			if (predData->prey.empty()) {
				if (pred->HasSpell(s_fullness)) {
					pred->RemoveSpell(s_fullness);
				}
				if (!pred->IsPlayerRef() && pred->HasSpell(s_statusSwalFol)) {
					pred->RemoveSpell(s_statusSwalFol);
				}
			} else if (!pred->IsPlayerRef()) {
				bool hasFollowers = false;
				for (auto& pyId : predData->prey) {
					if (VoreDataEntry* pyData = VoreData::IsValidGet(pyId)) {
						if (pyData->aIsChar && !pyData->aIsPlayer &&
							Utils::AreFriends(RE::PlayerCharacter::GetSingleton(),
								pyData->get()->As<RE::Actor>(),
								RE::BGSRelationship::RELATIONSHIP_LEVEL::kFriend)) {
							hasFollowers = true;
							break;
						}
					}
				}
				if (hasFollowers) {
					if (pred->HasSpell(s_statusSwalFol)) {
						pred->RemoveSpell(s_statusSwalFol);
					}
				}
			}
		}
	}

	void Dialogue::Clear_Prey(RE::Actor* pred, RE::TESObjectREFR* prey, bool forceClear)
	{
		if (!prey || !plugin_loaded || prey->GetFormType() != RE::FormType::ActorCharacter) {
			return;
		}
		RE::Actor* preyA = prey->As<RE::Actor>();

		if (VoreDataEntry* preyData = VoreData::IsValidGet(prey->GetFormID())) {
			if (!preyData->pred) {
				forceClear = true;
			}
		}
		if (forceClear) {
			if (preyA->HasSpell(s_notThere)) {
				preyA->RemoveSpell(s_notThere);
			}
		}
		// pred is player
		if (pred && pred->IsPlayerRef()) {
			if (!preyA->HasSpell(s_statusSwalByP)) {
				preyA->AddSpell(s_statusSwalByP);
			}
		}
		// prey is player
		else if (pred && preyA->IsPlayerRef()) {
			if (!pred->HasSpell(s_statusSwalP)) {
				pred->AddSpell(s_statusSwalP);
			}

		} else if (preyA->IsPlayerRef()) {
			g_playerDead->value = 0.0f;
		}
	}

	void Dialogue::SetupForReform(RE::Actor* pred, RE::Actor* prey)
	{
		if (!prey || !plugin_loaded) {
			return;
		}
		if (prey->IsPlayerRef()) {
			if (!pred->HasSpell(s_statusRefP)) {
				pred->AddSpell(s_statusRefP);
			}
		} else if (pred->IsPlayerRef()) {
			if (!prey->HasSpell(s_statusRefByP)) {
				prey->AddSpell(s_statusRefByP);
			}
		}
	}

	void Dialogue::OnDigestionChange(RE::Actor* pred)
	{
		if (!pred || !plugin_loaded) {
			return;
		}

		bool removeF = true;
		bool removeM = true;
		bool removeD = true;

		if (VoreDataEntry* predData = VoreData::IsValidGet(pred->GetFormID())) {
			for (auto& el : predData->prey) {
				if (VoreDataEntry* preyData = VoreData::IsValidGet(el)) {
					if (predData->aIsChar) {
						if (preyData->aAlive && preyData->pyDigestion == VoreDataEntry::hLethal) {
							if (preyData->aSex == RE::SEX::kFemale) {
								removeF = false;
							} else if (preyData->aSex == RE::SEX::kMale) {
								removeM = false;
							}
						} else {
							removeD = false;
						}
					}
				}
			}
		}

		if (removeF) {
			if (pred->HasSpell(s_soundStrF)) {
				pred->RemoveSpell(s_soundStrF);
			}
		} else if (!pred->HasSpell(s_soundStrF)) {
			pred->AddSpell(s_soundStrF);
		}

		if (removeM) {
			if (pred->HasSpell(s_soundStrM)) {
				pred->RemoveSpell(s_soundStrM);
			}
		} else if (!pred->HasSpell(s_soundStrM)) {
			pred->AddSpell(s_soundStrM);
		}

		if (removeD) {
			if (pred->HasSpell(s_soundDig)) {
				pred->RemoveSpell(s_soundDig);
			}
		} else if (!pred->HasSpell(s_soundDig)) {
			pred->AddSpell(s_soundDig);
		}
	}

	void Dialogue::PlayerDied()
	{
		if (!plugin_loaded) {
			return;
		}
		g_playerDead->value = 1.0f;
	}

	Vore::Locus Dialogue::GetLocusForSwallow(RE::Actor* pred, RE::TESObjectREFR* prey)
	{
		if (!plugin_loaded) {
			return lStomach;
		}
		if (pred->IsPlayerRef()) {
			if (prey->GetFormType() == RE::FormType::ActorCharacter) {
				int32_t fRank = prey->As<RE::Actor>()->GetFactionRank(f_locusPrey, false);
				if (fRank >= (int32_t)lStomach && fRank < (int32_t)NUMOFLOCI) {
					return (Locus)fRank;
				} else {
					return PlayerPrefs::voreLoc;
				}
			} else {
				return PlayerPrefs::voreLoc;
			}
		} else {
			int32_t fRank = pred->GetFactionRank(f_locusPred, false);
			if (fRank >= (int32_t)lStomach && fRank < (int32_t)NUMOFLOCI) {
				return (Locus)fRank;
			}
		}
		return lStomach;
	}

	void Dialogue::SetConsent(RE::Actor* pred, RE::Actor* prey, bool willing, bool lethal)
	{
		if (!plugin_loaded) {
			return;
		}
		if (pred->IsPlayerRef()) {
			if (willing) {
				if (lethal) {
					if (!prey->IsInFaction(f_dialogueVorePrey)) {
						prey->AddToFaction(f_dialogueVorePrey, 0);
					}
					if (!prey->IsInFaction(f_dialogueEndoPrey)) {
						prey->AddToFaction(f_dialogueEndoPrey, 0);
					}
				} else {
					if (!prey->IsInFaction(f_dialogueEndoPrey)) {
						prey->AddToFaction(f_dialogueEndoPrey, 0);
					}
				}
			} else {
				if (lethal) {
					if (prey->IsInFaction(f_dialogueVorePrey)) {
						prey->RemoveFromFaction(f_dialogueVorePrey);
					}
				} else {
					if (prey->IsInFaction(f_dialogueEndoPrey)) {
						prey->RemoveFromFaction(f_dialogueEndoPrey);
					}
				}
			}
		} else if (prey->IsPlayerRef()) {
			if (willing) {
				if (lethal) {
					if (!pred->IsInFaction(f_dialogueVorePred)) {
						pred->AddToFaction(f_dialogueVorePred, 0);
					}
					if (!pred->IsInFaction(f_dialogueEndoPred)) {
						pred->AddToFaction(f_dialogueEndoPred, 0);
					}
				} else {
					if (!pred->IsInFaction(f_dialogueEndoPred)) {
						pred->AddToFaction(f_dialogueEndoPred, 0);
					}
				}
			} else {
				if (lethal) {
					if (pred->IsInFaction(f_dialogueVorePred)) {
						pred->RemoveFromFaction(f_dialogueVorePred);
					}
				} else {
					if (pred->IsInFaction(f_dialogueEndoPred)) {
						pred->RemoveFromFaction(f_dialogueEndoPred);
					}
				}
			}
		}
	}

	Dialogue::PreyWillingness Dialogue::IsWillingPrey(RE::Actor* pred, RE::Actor* prey, bool lethal)
	{
		if (!plugin_loaded) {
			return kDisabled;
		}
		if (pred->IsPlayerRef()) {
			if (prey->IsInFaction(f_dialogueVorePrey)) {
				return kWilling;
			}
			if (!lethal && prey->IsInFaction(f_dialogueEndoPrey)) {
				return kWilling;
			}
			return kUnwilling;
		} else if (prey->IsPlayerRef()) {
			if (pred->IsInFaction(f_dialogueVorePred)) {
				return kWilling;
			}
			if (!lethal && pred->IsInFaction(f_dialogueEndoPred)) {
				return kWilling;
			}
			return kUnwilling;
		} else {
			return kDisabled;
		}
	}

	RE::BGSListForm* Dialogue::GetIVFormlist()
	{
		if (!plugin_loaded) {
			return nullptr;
		}
		uint8_t stuffingType = (uint8_t)g_ivType->value;
		switch (stuffingType) {
		case 1:
			return l_food;
		case 2:
			return l_ingredients;
		case 3:
			return l_potions;
		}
		return nullptr;
	}

	void Dialogue::TalkToA(RE::Actor* target)
	{
		if (_running.load() != kNone) {
			return;
		}
		if (!target->IsHumanoid() || target->IsDead()) {
			flog::info("can't talk to non-humanoid or dead npcs");
			return;
		}

		_handlePlayer = RE::PlayerCharacter::GetSingleton()->GetHandle();
		_handleNPC = target->GetHandle();

		// setup player handle

		VoreDataEntry* vDataPlayer = VoreData::IsValidGet(RE::PlayerCharacter::GetSingleton()->GetFormID());
		VoreDataEntry* vDataNPC = VoreData::IsValidGet(target->GetFormID());

		if (!(vDataNPC && vDataPlayer && (vDataNPC->pred || vDataPlayer->pred))) {
			flog::info("can't talk to NPCs not in vore");
			return;
		}

		if (vDataPlayer->pred) {
			vDataPlayer->aDialogue = true;
			vDataPlayer->CalcFast();
		}

		// setup npc handle
		if (vDataNPC->pred) {
			vDataNPC->aDialogue = true;
			vDataNPC->CalcFast();

			// if prey, move to pred
			target->MoveTo(RE::PlayerCharacter::GetSingleton());
			Core::HidePrey(target);
		}

		_running.store(kWaiting);

		SetupBeforeDialogue();

		auto t = std::thread([]() {
			std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
			SKSE::GetTaskInterface()->AddTask([]() {
				if (_handleNPC && !_handleNPC.get()->IsDead()) {
					_handleNPC.get()->ActivateRef(RE::PlayerCharacter::GetSingleton(), 0, nullptr, 0, true);
					_running.store(kDialogue);
				} else {
					_running.store(kFail);
				}
			});
			std::this_thread::sleep_for(std::chrono::milliseconds{ 200 });
			if (_running.load() != kDialogue) {
				SKSE::GetTaskInterface()->AddTask([]() {
					flog::error("failed to start dialogue!");
					ClearAfterDialogue();
				});
				return;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds{ 300 });

			for (;;) {
				if (_running.load() == kDialogue) {
					SKSE::GetTaskInterface()->AddTask([]() {
						if (!_handleNPC || !Funcs::IsInDialogueWithPlayer(_handleNPC.get()->As<RE::Actor>())) {
							_running.store(kFail);
						}
					});
					std::this_thread::sleep_for(std::chrono::milliseconds{ 200 });
				} else {
					SKSE::GetTaskInterface()->AddTask([]() {
						ClearAfterDialogue();
					});
					break;
				}
			}
		});
		t.detach();
	}
	void Dialogue::CheckPlugin()
	{
		// lookup ttdialogue quest form
		_dialogueQuest = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESQuest>(0x5900, "DialogueReGherk.esp");
		if (!_dialogueQuest) {
			flog::info("No dialogue plugin loaded");
			return;
		}
		flog::info("Dialogue plugin found");
		plugin_loaded = true;

		//SPELs
		s_fullness = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0x815, "Devourment.esp");
		s_notThere = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0x95F, "Devourment.esp");

		s_soundStrF = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0x964, "Devourment.esp");
		s_soundStrM = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0x81C, "Devourment.esp");
		s_soundDig = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0xC8E, "Devourment.esp");

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// digestion sounds placeholder
		//s_soundEndo = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0xC8E, "DialogueReGherk.esp");
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		s_statusSwalFol = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0xCE6, "Devourment.esp");
		// unused
		s_statusSwalP = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0xA59, "Devourment.esp");
		s_statusSwalByP = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0xA3E, "Devourment.esp");
		s_statusRefP = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0xA9D, "Devourment.esp");
		s_statusRefByP = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0xA41, "Devourment.esp");
		s_statusDigP = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0xAA0, "Devourment.esp");

		//GLOBs
		g_ivType = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x2A742E, "DialogueReGherk.esp");
		g_playerDead = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x990, "Devourment.esp");

		g_lethal = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x991, "Devourment.esp");
		g_locus = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x994, "Devourment.esp");

		//FACT
		f_dialogueVorePrey = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x6FEC0, "DialogueReGherk.esp");
		f_dialogueEndoPrey = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x6FEBE, "DialogueReGherk.esp");

		f_dialogueVorePred = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x6FEBF, "DialogueReGherk.esp");
		f_dialogueEndoPred = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x6FEBD, "DialogueReGherk.esp");

		f_locusPrey = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x7A0C1, "DialogueReGherk.esp");
		f_locusPred = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x7A0C2, "DialogueReGherk.esp");

		//FLST
		l_food = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSListForm>(0x2A232B, "DialogueReGherk.esp");
		l_ingredients = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSListForm>(0x2A232D, "DialogueReGherk.esp");
		l_potions = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSListForm>(0x2A232C, "DialogueReGherk.esp");
	}
}