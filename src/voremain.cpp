#include "headers/voremain.h"
#include "headers/dialogue.h"
#include "headers/nutils.h"
#include "headers/settings.h"
#include "headers/sounds.h"
#include "headers/times.h"
#include "headers/ui.h"
#include "headers/vutils.h"

namespace Vore::Core
{
	static bool CalculateConsent(RE::Actor* pred, RE::Actor* prey, bool lethal)
	{
		Dialogue::PreyWillingness preyWilling = Dialogue::IsWillingPrey(pred, prey, lethal);
		if (preyWilling < Dialogue::kDisabled) {
			return (bool)preyWilling;
		}
		if (!lethal) {
			if (Utils::AreFriends(pred, prey, RE::BGSRelationship::RELATIONSHIP_LEVEL::kConfidant)) {
				return true;
			} else {
				return false;
			}
		} else {
			if (Utils::AreFriends(pred, prey, RE::BGSRelationship::RELATIONSHIP_LEVEL::kLover)) {
				return true;
			} else {
				return false;
			}
		}
	}

	VoreDataEntry* GetApex(VoreDataEntry* prey)
	{
		if (!prey) {
			return nullptr;
		}
		if (prey->pred) {
			VoreDataEntry* predData = VoreData::IsValidGet(prey->pred);
			if (predData) {
				return GetApex(predData);
			} else {
				return prey;
			}
		} else {
			return prey;
		}
	}

	static void UpdatePlayerCamera()
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		VoreDataEntry* playerData = VoreData::IsValidGet(player->GetFormID());
		if (playerData && playerData->pred) {
			VoreDataEntry* predData = GetApex(playerData);
			if (predData->get()->GetFormID() != VoreGlobals::player_camera_owner) {
				VoreGlobals::player_camera_owner = predData->get()->GetFormID();
				RE::Actor* predA = predData->get()->As<RE::Actor>();
				RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
				camera->cameraTarget = predA->GetHandle();
				camera->ForceFirstPerson();
				camera->PushCameraState(RE::CameraState::kFirstPerson);
				/*camera->cameraRoot->local.translate = predA->GetPosition();
				camera->cameraRoot->world.translate = predA->GetPosition();
				RE::NiUpdateData ctx;
				camera->cameraRoot->UpdateWorldData(&ctx);*/

				/*RE::bhkCollisionFilter* filter = static_cast<RE::bhkCollisionFilter*>(world->GetWorld2()->collisionFilter);
				filter->layerBitfields[static_cast<uint8_t>(COL_LAYER::kCamera)] &= ~(static_cast<uint64_t>(1) << static_cast<uint64_t>(COL_LAYER::kBiped));
				filter->layerBitfields[static_cast<uint8_t>(COL_LAYER::kCamera)] &= ~(static_cast<uint64_t>(1) << static_cast<uint64_t>(COL_LAYER::kCharController));*/

				/* if (predA) {
					auto* a3d = predA->Get3D1(false);
					if (a3d) {
						flog::info("found 3d");
						auto* aNode = a3d->GetObjectByName("NPC Belly");
						if (aNode) {
							flog::info("found node");
							RE::NiPointer<RE::NiNode> nodePointer(aNode->AsNode());
							camera->SetCameraRoot(nodePointer);
						}
					}
				}*/
			}
		} else if (VoreGlobals::player_camera_owner != 0) {
			VoreGlobals::player_camera_owner = 0;
			RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
			camera->cameraTarget = player->GetHandle();
			//return control to player
		}
	}

	void HidePrey(RE::Actor* target)
	{
		Funcs::StopCombatAlarm(target);
		Funcs::StopCombat(target);
		Funcs::SetAlpha(target, 0.0f, false);
		Funcs::SetGhost(target, true);
		Funcs::SetAlpha(target, 0.0f, false);
		if (target->IsPlayerRef()) {
			RE::ControlMap::GetSingleton()->ToggleControls(RE::UserEvents::USER_EVENT_FLAG::kMovement, false);
			RE::ControlMap::GetSingleton()->ToggleControls(RE::UserEvents::USER_EVENT_FLAG::kFighting, false);
			RE::ControlMap::GetSingleton()->ToggleControls(RE::UserEvents::USER_EVENT_FLAG::kSneaking, false);
			RE::ControlMap::GetSingleton()->ToggleControls(RE::UserEvents::USER_EVENT_FLAG::kPOVSwitch, false);
			RE::ControlMap::GetSingleton()->ToggleControls(RE::UserEvents::USER_EVENT_FLAG::kActivate, false);
			RE::ControlMap::GetSingleton()->ToggleControls(RE::UserEvents::USER_EVENT_FLAG::kWheelZoom, false);
			target->SetCollision(false);
		} else {
			Funcs::SetRestrained(target, true);
		}
	}

	void UnhidePrey(RE::Actor* target)
	{
		Funcs::SetAlpha(target, 1.0f, false);
		Funcs::SetGhost(target, false);
		Funcs::SetAlpha(target, 1.0f, false);
		if (target->IsPlayerRef()) {
			RE::ControlMap::GetSingleton()->ToggleControls(RE::UserEvents::USER_EVENT_FLAG::kMovement, true);
			RE::ControlMap::GetSingleton()->ToggleControls(RE::UserEvents::USER_EVENT_FLAG::kFighting, true);
			RE::ControlMap::GetSingleton()->ToggleControls(RE::UserEvents::USER_EVENT_FLAG::kSneaking, true);
			RE::ControlMap::GetSingleton()->ToggleControls(RE::UserEvents::USER_EVENT_FLAG::kPOVSwitch, true);
			RE::ControlMap::GetSingleton()->ToggleControls(RE::UserEvents::USER_EVENT_FLAG::kActivate, true);
			RE::ControlMap::GetSingleton()->ToggleControls(RE::UserEvents::USER_EVENT_FLAG::kWheelZoom, true);
			target->SetCollision(true);
		} else {
			Funcs::SetRestrained(target, false);
		}
	}

	void FinishPlayerDigestion()
	{
		RE::Actor* player = RE::PlayerCharacter::GetSingleton();
		VoreDataEntry* playerData = VoreData::IsValidGet(player->GetFormID());
		if (playerData && !playerData->aAlive) {
			AV::DamageAV(player, RE::ActorValue::kHealth, AV::GetAV(player, RE::ActorValue::kHealth) + 1.0f);
			VoreDataEntry* predData = VoreData::IsValidGet(playerData->pred);
			if (predData) {
				Regurgitate(predData->get()->As<RE::Actor>(), player->GetFormID(), RegType::rAll, false);
			}
		}
	}


	void StartReformation(VoreDataEntry* preyData, VoreDataEntry* predData)
	{
		if (!preyData->aIsChar) {
			return;
		}
		preyData->pyLocusMovement = VoreDataEntry::mStill;
		preyData->pyElimLocus = preyData->pyLocus;
		preyData->aAlive = false;
		preyData->pyDigestProgress = 99.999;
		preyData->SetMyDigestion(VoreDataEntry::hReformation, true);
		Dialogue::SetupForReform(predData->get()->As<RE::Actor>(), preyData->get()->As<RE::Actor>());
	}

	void InventoryVore(RE::Actor* pred)
	{
		//apply effect
		auto* iVoreSpell = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0x6F372, "SkyrimVorePP.esp");
		if (!iVoreSpell) {
			flog::error("No item vore spell found!");
		}
		auto* caster = RE::PlayerCharacter::GetSingleton()->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
		if (!caster) {
			flog::error("Can't cast item vore spell - no magic caster on player");
		}
		caster->CastSpellImmediate(iVoreSpell, false, pred, 1.0f, false, 1.0f, nullptr);
		//bool activated = false;
		//activated = iVoreSpell->Activate(pred, RE::PlayerCharacter::GetSingleton(), 0, nullptr, 0);
		//activated = RE::PlayerCharacter::GetSingleton()->Activate(pred, nullptr, 0, iVoreSpell, 0);
		Funcs::ShowGiftMenu(pred, Dialogue::GetIVFormlist());
		//flog::info("Item vore success? {}", activated);
	}

	bool CanMoveToLocus([[maybe_unused]] const RE::FormID& pred, [[maybe_unused]] const RE::FormID& prey, const Locus& locus, const Locus& locusSource)
	{
		// incorrect locus destination
		if (locus >= Locus::NUMOFLOCI) {
			return false;
		}
		if (locusSource == Locus::lNone) {
			return true;
		}
		if (locusSource < Locus::NUMOFLOCI && VoreGlobals::locus_transfer_destination[locusSource] == locus) {
			return true;
		}
		return false;
	}

	bool CanBeRegurgitated(const VoreDataEntry* prey)
	{
		// prey is fully digested
		if (!prey->aAlive && prey->pyDigestProgress == 100) {
			return true;
		}
		// no full tour
		if (prey->aAlive && prey->pyLocusProcess == 0.0) {
			return true;
		}
		return false;
	}

	void MoveToLocus(const RE::FormID prey, const Locus locus)
	{
		if (VoreDataEntry* pyData = VoreData::IsValidGet(prey)) {

			if (CanMoveToLocus(pyData->pred, prey, locus, pyData->pyLocus)) {
				pyData->pyLocus = locus;
				if (pyData->aAlive) {
					pyData->SetMyDigestion(VoreDataEntry::hNone, true);
				}
				if (locus == lBowel) {
					pyData->pyLocusProcess = 100.0;
				} else {
					pyData->pyLocusProcess = 0.0;
				}
				flog::info("Moved Prey {} to Locus {}", Name::GetName(pyData->get()), (uint8_t)locus);
			}
		}
	}

	void SetPreyVisibility(RE::TESObjectREFR* preyObj, RE::Actor* pred, bool show, VoreDataEntry* preyData)
	{
		if (!preyObj) {
			flog::error("Can't set prey visibility for non-existent prey");
			return;
		}
		if (preyObj->GetFormType() == RE::FormType::ActorCharacter) {
			RE::Actor* prey = preyObj->As<RE::Actor>();

			if (!show) {
				//RE::TESObjectCELL* stomachCell = RE::TESForm::LookupByEditorID("QASmoke")->As<RE::TESObjectCELL>();
				RE::TESObjectREFR* stomachCell = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectREFR>(0x12D9, "SkyrimVorePP.esp");
				//flog::critical("FOUND CELL MARKER {}", (int)stomachCell);
				// test: can you move dead actors?
				if (!prey->IsPlayerRef()) {
					Funcs::MoveTo(prey, stomachCell);
				}
				UpdatePlayerCamera();
				HidePrey(prey);

				/*if (!prey->IsPlayerRef()) {
					prey->EnableAI(false);
				}*/
			} else if (pred) {
				UpdatePlayerCamera();
				UnhidePrey(prey);

				/*if (!prey->IsPlayerRef()) {
					prey->EnableAI(true);
				}*/

				if (preyData && preyData->pyDigestProgress >= 100.0) {
					RE::TESObjectREFR* stomachDeadCell = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectREFR>(0x333D, "SkyrimVorePP.esp");
					Funcs::MoveTo(prey, stomachDeadCell);
					// make remains
					// pred->PlaceObjectAtMe();```
					// unequip all from prey
					// move items to remains
					// tp them to pred
					// mark prey for removal
					// prey->race
					//
					// switching race doesn't work for dead actors
					// plus I don't think it's healthy for the game
					// so I'll use the bone piles

					std::string_view bonesName = VoreGlobals::race_remains.contains(prey->GetRace()->GetFormEditorID()) ?
					                                 VoreGlobals::race_remains[prey->GetRace()->GetFormEditorID()] :
					                                 (prey->IsHumanoid() ? "VoreSkeletonHuman" : "VoreSkeletonHuman");

					//std::string_view bonesName = "VoreSkeletonHuman";
					RE::TESActorBase* bonesBase = RE::TESForm::LookupByEditorID<RE::TESActorBase>(bonesName);
					//RE::TESActorBase* bonesBase = RE::TESForm::LookupByID<RE::TESActorBase>(0x00039cf5);

					RE::Actor* bones = pred->PlaceObjectAtMe(bonesBase, false)->As<RE::Actor>();
					//flog::critical("GET BONE AND PREY SCALE {} {} {}", bones->GetBaseHeight(), bones->GetScale(), prey->GetHeight());
					//bones->KillImmediate();
					AV::DamageAV(bones, RE::ActorValue::kHealth, AV::GetAV(bones, RE::ActorValue::kHealth));
					bones->KillImpl(nullptr, (float)AV::GetAV(bones, RE::ActorValue::kHealth), true, true);
					auto items = prey->GetInventory();
					bones->SetSize(static_cast<float>(preyData->aSize / VoreGlobals::slider_one));
					for (auto& [i, j] : items) {
						prey->RemoveItem(i, j.first, RE::ITEM_REMOVE_REASON::kStoreInContainer, nullptr, bones);
					}

					flog::info("RELEASING {}", Name::GetName(prey));
					flog::info("Is prey persistent? {}", prey->IsPersistent());

				} else {
					Funcs::MoveTo(prey, pred);
				}
			} else {
				UnhidePrey(prey);
			}
		} else {
			if (!show) {
				RE::TESObjectREFR* stomachCell = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectREFR>(0x28556, "SkyrimVorePP.esp");
				Funcs::MoveTo(preyObj, stomachCell);
			} else if (pred) {
				if (preyData && preyData->pyDigestProgress >= 100.0) {
					uint32_t refCound = preyObj->QRefCount();

					flog::info("Ref cound {}", refCound);

					flog::info("RELEASING {}", Name::GetName(preyObj));
					//finish this!!!!!!!!
				} else {
					if (preyData && preyData->aDeleteWhenDone) {
						//RE::NiPoint3 predPos{ pred->GetPosition() };
						if (preyObj->GetInventory().size() > 0) {
							flog::info("Releasing stomach container");
							//auto* player = RE::PlayerCharacter::GetSingleton();
							//flog::info("player pos {} {} {}", player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
							for (auto& [i, j] : preyObj->GetInventory()) {
								auto itemPreyRefr = preyObj->RemoveItem(i, j.first, RE::ITEM_REMOVE_REASON::kDropping, nullptr, nullptr);
								auto iobj = pred->PlaceObjectAtMe(i, false);
								flog::info("Releaing {}", iobj->GetDisplayFullName());
								//RE::TESObjectREFR* itemReft = itemPreyRefr.get().get();
								//flog::info("Item: {} pos {} {} {}", itemReft->GetDisplayFullName(), itemReft->GetPositionX(), itemReft->GetPositionY(), itemReft->GetPositionZ());
							}
						}
					} else {
						Funcs::MoveTo(preyObj, pred);
					}
				}
			}
		}
	}

	void AddFakeFood(RE::Actor* pred, RE::AlchemyItem* item)
	{
		VoreDataEntry* predData = VoreData::GetDataOrMake(pred);
		if (!predData) {
			return;
		}
		RE::AlchemyItem* fakeFood = RE::TESDataHandler::GetSingleton()->LookupForm<RE::AlchemyItem>(0x88875, "SkyrimVorePP.esp");
		bool foundFF = false;
		double itemSize = GetItemSize(item); 
		if (!predData->prey.empty()) {
			
			for (auto& el : predData->prey) {
				if (VoreDataEntry* preyData = VoreData::IsValidGet(el)) {
					if (preyData->get()->GetBaseObject() == fakeFood && preyData->pyLocus == lStomach) {
						flog::debug("Adding to existing fake food");
						// how much weight has been digested
						double x = preyData->aSize * preyData->pyDigestProgress;
						preyData->aSize += itemSize;
						preyData->pyDigestProgress = x / preyData->aSize;
						foundFF = true;
						break;
					}
					
				}
			}
		}
		if (!foundFF) {

			RE::TESObjectREFR* itemCell = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectREFR>(0x28556, "SkyrimVorePP.esp");
			RE::TESObjectREFR* ffRefr = itemCell->PlaceObjectAtMe(fakeFood, false).get();
			VoreDataEntry* ffData = VoreData::GetDataOrMake(ffRefr);
			if (ffData) {
				ffData->aSize = itemSize;
				ffData->aSizeDefault = itemSize;
				ffData->aDeleteWhenDone = true;
				Swallow(pred, ffRefr, lStomach, VoreDataEntry::hSafe, true, false);
				ffData->DigestLive();
			} else {
				flog::error("Could not make fake food for {}!", Name::GetName(pred));
			}
		}

		// check if pred has
	}

	void InstantWg(RE::Actor* pred, double amount)
	{
		if (VoreDataEntry* predData = VoreData::GetDataOrMake(pred)) {
			if (predData->pdWGAllowed) {
				predData->pdFat += amount * VoreSettings::wg_fattemp;
				predData->pdFatgrowth += amount * VoreSettings::wg_fattemp;
				predData->SetPredUpdate(true);
				//predData->pdSizegrowth += amount * 100.0;
			}
		}
	}

	void InstantWgItem(RE::Actor* pred, RE::AlchemyItem* item)
	{
		InstantWg(pred, GetItemSize(item));
	}

	void Swallow(RE::Actor* pred, std::vector<RE::TESObjectREFR*> preys, Locus locus, VoreDataEntry::VoreState ldType, bool fullswallow, bool safeSwitch)
	{
		flog::info("Swallow start");
		if (!pred) {
			flog::warn("Missing pred");
			return;
		}
		if (preys.empty()) {
			flog::warn("Missing preys");
			return;
		}
		if (ldType == VoreDataEntry::hNone) {
			flog::error("Trying to swallow with a wrong digestion type");
			return;
		}

		//initialize pred

		flog::info("Pred {}", Name::GetName(pred));
		Vore::VoreDataEntry* predData = VoreData::GetDataOrMake(pred);
		if (!predData) {
			flog::warn("Bad pred, aborting!");
		}

		int preyCount = 0;
		for (auto& prey : preys) {
			if (!prey) {
				flog::warn("broken prey");
				continue;
			}

			if (prey->GetFormType() == RE::FormType::ActorCharacter) {
				RE::Actor* preyA = skyrim_cast<RE::Actor*>(prey);
				flog::info("Pred {}, prey {}, health {}", Name::GetName(pred), Name::GetName(preyA), AV::GetAV(preyA, RE::ActorValue::kHealth));

				//initialize prey

				Vore::VoreDataEntry* preyData = VoreData::GetDataOrMake(preyA);
				if (!preyData) {
					flog::warn("Bad prey, skipping!");
					continue;
				}

				RE::FormID oldPred = preyData->pred;

				if (oldPred) {
					if (VoreDataEntry* oldPredData = VoreData::IsValidGet(oldPred)) {
						if (oldPredData->prey.contains(preyA->GetFormID())) {
							Regurgitate(oldPredData->get()->As<RE::Actor>(), preyA->GetFormID(), RegType::rTransfer, false);
						}
					}
				}

				// don't delete a prey if we're planning on using them
				if (VoreGlobals::delete_queue.contains(preyA->GetFormID())) {
					VoreGlobals::delete_queue.erase(preyA->GetFormID());
				}
				// get perks and levels
				preyData->GetVStats();
				//inset prey to PRED
				predData->prey.insert(preyA->GetFormID());
				//----------
				preyData->aAlive = !(preyA->IsDead());

				preyData->pred = pred->GetFormID();
				preyData->pyLocus = locus;
				preyData->pyElimLocus = locus;

				//preyData.pyPendingReformation = false;

				preyData->pyStruggleResource = 4;

				// prey will be willing if they are a friend or if they are in a special faction from dialogue plugin
				preyData->pyConsentLethal = CalculateConsent(pred, preyA, true);
				preyData->pyConsentEndo = CalculateConsent(pred, preyA, false);

				preyData->pyDigestProgress = 0;
				preyData->pySwallowProcess = fullswallow || !preyData->aAlive ? 100 : 20;

				//full tour related shit
				preyData->pyLocusMovement = (preyData->pyLocus == Locus::lBowel) ? VoreDataEntry::mIncrease : VoreDataEntry::mStill;
				preyData->pyLocusProcess = 0;

				//crime / combat when prey is unwilling
				if (ldType == VoreDataEntry::hLethal && !preyData->pyConsentLethal) {
					Funcs::Attacked(preyA, pred);
				} else if (ldType != VoreDataEntry::hLethal && !preyData->pyConsentEndo) {
					Funcs::Attacked(preyA, pred);
				}

				preyData->SetMyDigestion(ldType, false);
				if (!oldPred) {
					SetPreyVisibility(preyA, pred, false, preyData);
				}

				Dialogue::OnSwallow_Prey(pred, prey);

				preyCount++;

			} else {
				RE::TESBoundObject* base = prey->GetBaseObject();
				// check if the object is valid bc it can be a stomach contaner with pre-created voredata or fake food, but it's form type isn't usually allowed for swallowing
				if (VoreData::IsValid(prey->GetFormID()) || VoreGlobals::allowed_pickup.contains(base->GetFormType())) {
					flog::info("Pred {}, prey {}, eatable object", Name::GetName(pred), Name::GetName(prey));

					Vore::VoreDataEntry* preyData = VoreData::GetDataOrMake(prey);
					if (!preyData) {
						flog::warn("Bad prey, skipping!");
						continue;
					}

					RE::FormID oldPred = preyData->pred;

					if (oldPred) {
						if (VoreDataEntry* oldPredData = VoreData::IsValidGet(oldPred)) {
							if (oldPredData->prey.contains(prey->GetFormID())) {
								Regurgitate(oldPredData->get()->As<RE::Actor>(), prey->GetFormID(), RegType::rTransfer, false);
							}
						}
					}
					if (VoreGlobals::delete_queue.contains(prey->GetFormID())) {
						VoreGlobals::delete_queue.erase(prey->GetFormID());
					}
					// get perks and levels
					preyData->GetVStats();

					predData->prey.insert(prey->GetFormID());

					// items that are "alive" are endoed items
					// dead items are items that are being digested
					preyData->aAlive = true;

					preyData->pred = pred->GetFormID();
					preyData->pyLocus = locus;
					preyData->pyElimLocus = locus;

					preyData->pyDigestProgress = 0;

					preyData->pyConsentEndo = true;
					preyData->pyConsentLethal = true;

					preyData->pySwallowProcess = fullswallow || !preyData->aAlive ? 100 : 40;

					//full tour related shit
					preyData->pyLocusMovement = (preyData->pyLocus == Locus::lBowel) ? VoreDataEntry::mIncrease : VoreDataEntry::mStill;
					preyData->pyLocusProcess = 0;

					preyData->SetMyDigestion(ldType, false);
					if (!oldPred) {
						SetPreyVisibility(prey, pred, false, preyData);
					}

					preyCount++;
				}
				flog::info("Base form {}", (int)prey->GetBaseObject()->GetFormType());
				flog::warn("not character, skipping for now");
			}
		}
		if (preyCount > 0) {

			predData->pdUpdateGoal = true;
			Dialogue::OnSwallow_Pred(pred);
			predData->GetVStats();
			predData->SetDigestionAsPred(locus, ldType, safeSwitch);
			if (fullswallow) {
				predData->UpdateSliderGoals();
				predData->PlayStomachSounds();
			}
			predData->SetPredUpdate(true);
		} else {
			flog::warn("No prey were swallowed");
		}

		predData->PlaySwallow();
		UI::VoreMenu::SetMenuMode(UI::kDefault);
		Log::PrintVoreData();
		flog::info("Swallow end");
	}

	void Swallow(RE::Actor* pred, RE::TESObjectREFR* prey, Locus locus, VoreDataEntry::VoreState ldType, bool fullswallow, bool safeSwitch)
	{
		std::vector<RE::TESObjectREFR*> preys = { prey };
		Swallow(pred, preys, locus, ldType, fullswallow, safeSwitch);
	}

	void SwallowTarget(RE::Actor* pred, Locus locus, VoreDataEntry::VoreState ldType, bool fullswallow)
	{
		//flog::info("{}'s target for swallow: {}", pred-> ,pred->GetDisplayFullName());
		//find target
		//Swallow(pred, target)

		if (pred == RE::PlayerCharacter::GetSingleton()) {
			Swallow(pred, Vore::Utils::GetCrosshairObject(), locus, ldType, fullswallow, true);
		} else {
			Swallow(pred, Vore::Utils::GetFrontObjects(pred, 1), locus, ldType, fullswallow, true);
		}
	}

	void Regurgitate(RE::Actor* pred, std::vector<RE::FormID> preys, RegType rtype, bool isEscape)
	{
		flog::info("Start of Regurgitation");

		flog::info("Pred {}", Name::GetName(pred));

		RE::FormID predId = pred->GetFormID();

		if (!VoreData::IsPred(predId, true)) {
			flog::warn("Pred {} is not pred", Name::GetName(pred));
			return;
		}

		if (preys.empty()) {
			flog::warn("Missing preys");
			return;
		}

		Vore::VoreDataEntry* predData = VoreData::IsValidGet(pred->GetFormID());
		if (!predData) {
			flog::warn("Bad pred, aborting regurgitation!");
		}

		//for nested vore
		RE::FormID topPred = predData->pred;

		//these preys are broken and will be deleted from the system
		std::vector<RE::FormID> preysToErase = {};
		//these preys will be transferred to top pred
		std::vector<RE::TESObjectREFR*> preysToSwallow = {};
		//these preys will be regurgitated

		std::vector<std::pair<RE::TESObjectREFR*, VoreDataEntry*>> preysToDelete = {};

		bool playVomit{ false };
		bool playDisposal{ false };

		for (RE::FormID prey : preys) {
			flog::info("Prey {}", Name::GetName(prey));
			VoreDataEntry* preyData = VoreData::IsValidGet(prey);
			if (!preyData) {
				flog::warn("SKIPPING NON-EXISTENT PREY");
				continue;
			}
			if (!VoreData::IsPrey(prey)) {
				flog::warn("ERASING BAD PREY");
				predData->prey.erase(prey);
				preysToErase.push_back(prey);
				SetPreyVisibility(preyData->get(), pred, true, preyData);
				if (preyData->aIsChar) {
					Dialogue::Clear_Prey(nullptr, preyData->get(), true);
				}
				continue;
			}

			if (preyData->pred != predId) {
				flog::error("Prey and pred mismatch! Skipping.");
				continue;
			}

			if (rtype != rAll && !CanBeRegurgitated(preyData)) {
				flog::info("Can't regurgitate {}", Name::GetName(preyData->get()));
				continue;
			}

			predData->prey.erase(prey);

			//clear prey
			//give prey XP
			if (preyData->aIsChar) {
				//give full xp IF
				//1. prey escaped on their own
				//2. prey is unwilling but got released, and the pred is not player
				// this is to prevent player from power leveling preys or power leveling themselves
				// also endo is significantly less risky so the xp is reduced
				preyData->pyXP += VoreSettings::gain_py_release * predData->myStats.pdLvl / 10.0f *
					(isEscape || !predData->aIsPlayer && (preyData->pyStruggling || preyData->pyStruggleResource == 0) ?
						(preyData->pyDigestion == VoreDataEntry::hLethal ? 1.0f : 0.4f) : 0.1f);
			}

			if (preyData->pyLocus == Locus::lStomach) {
				playVomit = true;
			} else if (preyData->pyLocus == Locus::lBowel) {
				playDisposal = true;
			}
			preyData->pyLocus = Locus::lNone;
			preyData->pyElimLocus = Locus::lNone;
			//preyData->pyPendingReformation = false;

			preyData->pyStruggleResource = 0;
			preyData->pyConsentEndo = false;
			preyData->pyConsentLethal = false;

			preyData->pyLocusMovement = VoreDataEntry::mStill;

			preyData->pySwallowProcess = 0;
			preyData->pyLocusProcess = 0;

			preyData->pyDigestion = VoreDataEntry::hNone;
			preyData->CalcFast(true);
			preyData->CalcSlow(true);
			preyData->UpdateStats(false);
			preyData->GetVStats();

			if (rtype == RegType::rTransfer) {
				preyData->pyDigestProgress = 0;
				flog::info("prey transfer");
			} else if (topPred) {
				preyData->pyDigestProgress = 0;
				preysToSwallow.push_back(preyData->get());
			} else {
				preyData->pred = 0;
				//preyData->pdUpdateGoal = true;
				VoreData::SoftDelete(prey, !preyData->aAlive);
				preysToDelete.push_back({ preyData->get(), preyData });
			}

			//it's important to call this in the end, because it checks if prey has a pred
			if (preyData->aIsChar) {
				Dialogue::Clear_Prey(pred, preyData->get(), false);
			}
		}

		// erase bad prey
		for (auto& el : preysToErase) {
			VoreData::HardDelete(el);
		}
		// nested vore
		if (!preysToSwallow.empty()) {
			Swallow(skyrim_cast<RE::Actor*>(VoreData::Data[topPred].get()), preysToSwallow, predData->pyLocus, predData->pyDigestion, true, false);
		}
		// regurgitation
		for (auto& [prey, pData] : preysToDelete) {
			SetPreyVisibility(prey, pred, true, pData);
			pData->pyDigestProgress = 0;
		}

		predData->pdUpdateGoal = true;

		// reset indigestion
		std::array<bool, Locus::NUMOFLOCI> indigestion = { false };
		for (const RE::FormID& prey : predData->prey) {
			if (VoreDataEntry* pyData = VoreData::IsValidGet(prey); pyData->pyLocus < Locus::NUMOFLOCI) {
				indigestion[pyData->pyLocus] = true;
			}
		}
		for (uint8_t l = Locus::lStomach; l < Locus::NUMOFLOCI; l++) {
			if (!indigestion[l]) {
				predData->pdIndigestion[l] = 0.0;
			}
		}
		// commit vore xp
		predData->UpdateStats(true);
		predData->GetVStats();
		// update digestion sounds
		Dialogue::OnDigestionChange(pred);

		if (playVomit) {
			predData->PlayRegurgitation(false);
		}
		if (playDisposal) {
			predData->PlayRegurgitation(true);
		}

		Dialogue::Clear_Pred(pred);

		VoreData::SoftDelete(predId, !predData->aAlive);
		UI::VoreMenu::SetMenuMode(UI::kDefault);
		Log::PrintVoreData();
		flog::info("End of Regurgitation");
	}

	void Regurgitate(RE::Actor* pred, RE::FormID prey, RegType rtype, bool isEscape)
	{
		std::vector<RE::FormID> preys = { prey };
		Regurgitate(pred, preys, rtype, isEscape);
	}

	void RegurgitateAll(RE::Actor* pred, Locus locus, RegType rtype, bool isEscape)
	{
		if (!pred) {
			flog::warn("Regurgitation: Missing pred");
			return;
		}
		if (!VoreData::IsPred(pred->GetFormID(), true)) {
			flog::warn("Regurgitation: {} is not a pred", Name::GetName(pred));
			return;
		}
		Regurgitate(pred, FilterPrey(pred->GetFormID(), locus, true), rtype, isEscape);
	}

	void AutoRelease(VoreDataEntry* preyData, VoreDataEntry* predData)
	{
		RE::Actor* predA = predData->get()->As<RE::Actor>();
		if (!VoreSettings::companion_disposal && predA->IsPlayerTeammate()) {
			return;
		}
		Regurgitate(predA, preyData->get()->GetFormID(), rAll, false);
	}

	//updates dead characters and idle preds
	static void UpdateSlow(const double& delta)
	{
		//double digestBase = VoreSettings::digestion_amount_base * delta;

		//process prey
		for (auto& [key, val] : VoreData::Data) {
			if (val.Slow()) {
				(val.*(val.Slow()))(delta);
			}
		}

		//updates all preds
		for (auto& [key, val] : VoreData::Data) {
			if (val.Predd()) {
				(val.*(val.Predd()))(delta);
			}
		}

		//handle erase queue
		// wait until all sliders are zeroed

		for (auto it = VoreGlobals::delete_queue.begin(); it != VoreGlobals::delete_queue.end();) {
			bool deleteThis = true;
			if (VoreData::Data.contains(*it)) {
				for (auto& sl : VoreData::Data[*it].pdSliders) {
					if (sl > 0.01f) {
						deleteThis = false;
					}
				}
				if (!deleteThis) {
					++it;
				} else {
					flog::info("Starting deletion of {} from VoreData", Name::GetName(*it));
					if (VoreData::Data[*it].aIsChar && VoreData::Data[*it].get()) {
						// unsure if this works, but there must be a way to remove body morphs when deleting from voredata
						if (!VoreData::Data[*it].get()->Is3DLoaded()) {
							flog::info("no 3d loaded when deleting from voredata, loading it");
							VoreData::Data[*it].get()->Load3D(false);
						}
						if (VoreData::Data[*it].get()->Is3DLoaded() && VoreGlobals::body_morphs->HasBodyMorphKey(VoreData::Data[*it].get(), VoreGlobals::MORPH_KEY)) {
							flog::info("found 3d, clearing body morphs");
							VoreGlobals::body_morphs->ClearBodyMorphKeys(VoreData::Data[*it].get(), VoreGlobals::MORPH_KEY);
						}
					}
					if (VoreData::Data[*it].aDeleteWhenDone) {
						VoreData::Data[*it].get()->SetDelete(true);
					}
					VoreData::Data.erase(*it);
					flog::info("Deleting {} from VoreData", Name::GetName(*it));
					it = VoreGlobals::delete_queue.erase(it);
				}
			}
			// skip this element and delete it from dq because it's not in Data
			else {
				it = VoreGlobals::delete_queue.erase(it);
			}
		}

		UI::VoreMenu::NeedUpdate = true;
	}

	//updates live characters
	static void UpdateFast(const double& delta)
	{
		// if (VoreGlobals::_persTest.get()) {
		// 	flog::debug("{} is persistent {}", Name::GetName(VoreGlobals::_persTest.get().get()), VoreGlobals::_persTest.get().get()->IsPersistent());
		// } else {
		// 	flog::debug("NO HANDLE");
		// }
		bool needUIUpdate = false;
		//updates prey only
		for (auto& [key, val] : VoreData::Data) {
			if (val.Fast()) {
				(val.*(val.Fast()))(delta);
				needUIUpdate = true;
			}
		}
		//updates top preds
		for (auto& [key, val] : VoreData::Data) {
			if (val.aIsChar && val.get()->Is3DLoaded()) {
				if (val.pdUpdateGoal) {
					val.UpdateSliderGoals();
				}
				if (val.pdUpdateStruggleGoal) {
					val.UpdateStruggleGoals();
				}
			}
		}
		UI::VoreMenu::NeedUpdate = UI::VoreMenu::NeedUpdate || needUIUpdate;
	}

	void SetupTimers()
	{
		Timer digUpdatef(1, VoreSettings::fast_update, &UpdateFast);
		Timer digUpdateS(2, VoreSettings::slow_update, &UpdateSlow);
		Timer belUpdatef(3, (double)VoreSettings::belly_fast_update, &UpdateBelly);
		Time::SetTimer(digUpdatef);
		Time::SetTimer(digUpdateS);
		Time::SetTimer(belUpdatef);
	}
	void SetupBellies()
	{
		for (auto& [key, val] : VoreData::Data) {
			if (!val.pred) {
				val.pdUpdateGoal = true;
			}
		}
	}
	void MegaDigest(const double& delta)
	{
		UpdateFast(delta);
		UpdateSlow(delta);
		for (auto& [key, val] : VoreData::Data) {
			if (val.get()->Is3DLoaded()) {
				if (val.pdUpdateGoal) {
					val.UpdateSliderGoals();
				}
				if (val.pdUpdateStruggleGoal) {
					val.UpdateStruggleGoals();
				}
			}
		}
		UpdateBelly(delta);
	}
}
