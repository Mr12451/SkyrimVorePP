#include "headers/voremain.h"
#include "headers/nutils.h"
#include "headers/settings.h"
#include "headers/times.h"
#include "headers/ui.h"
#include "headers/vutils.h"
#include "headers/sounds.h"
#include "headers/dialogue.h"

namespace Vore::Core
{

	bool HasConsented(RE::Actor* prey, RE::Actor* pred, VoreState dType)
	{
		return dType != hLethal;
	}
	VoreState GetStruggle(RE::Actor* prey, RE::Actor* pred, Locus locus, VoreState dType)
	{
		//placeholder!!!
		if (locus == Locus::lNone) {
			return VoreState::sStill;
		}
		//placeholder!!!
		if (prey->IsDead()) {
			return VoreState::sStill;
		}

		if (!HasConsented(prey, pred, dType)) {
			if (pred->IsPlayerRef()) {
				//Funcs::StartCombat(pred, prey);
				//Funcs::SendAssaultAlarm(prey);
				Funcs::Attacked(prey, pred);
			}
			return VoreState::sStruggling;
		}
		return VoreState::sStill;
	}

	void HidePrey(RE::Actor* target)
	{
		Funcs::StopCombatAlarm(target);
		Funcs::StopCombat(target);
		Funcs::SetAlpha(target, 0.0f, false);
		Funcs::SetGhost(target, true);
		Funcs::SetAlpha(target, 0.0f, false);
		Funcs::SetRestrained(target, true);
	}

	void UnhidePrey(RE::Actor* target)
	{
		Funcs::SetAlpha(target, 1.0f, false);
		Funcs::SetGhost(target, false);
		Funcs::SetAlpha(target, 1.0f, false);
		Funcs::SetRestrained(target, false);
	}

	void SwitchToDigestion(const RE::FormID pred, const Locus locus, const VoreState dType, const bool forceStopDigestion, bool doDialogueUpd)
	{
		VoreDataEntry* predData = VoreData::IsValidGet(pred);
		if (!predData) {
			flog::warn("Switching to digestion failed: bad pred: {}", Name::GetName(predData->get()));
			return;
		}
		if (dType == VoreState::hNone) {
			flog::warn("Switching to None digestion type! Returning.: {}", Name::GetName(predData->get()));
			return;
		}
		if (locus == Locus::lNone) {
			for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
				SwitchToDigestion(pred, (Locus)i, dType, forceStopDigestion, false);
			}
			if (doDialogueUpd) {
				Dialogue::OnDigestionChange(predData->get()->As<RE::Actor>());
			}
			return;
		}
		bool allowChange = true;
		std::vector<RE::FormID> locusPreys = FilterPrey(pred, locus, false);
		if (!forceStopDigestion && dType == VoreState::hSafe) {
			for (const RE::FormID& el : locusPreys) {
				if (VoreDataEntry* pyData = VoreData::IsValidGet(el)) {
					if (pyData->aAlive && (pyData->pyDigestion == hLethal || pyData->pyDigestion == hReformation)) {
						flog::trace("Can't switch locus {} from {} for {}", (uint8_t)locus, (uint8_t)predData->pdLoci[locus], Name::GetName(predData->get()));
						allowChange = false;
						break;
					}
				}
			}
		}
		if (allowChange) {
			predData->pdLoci[locus] = dType;
			for (const RE::FormID& el : locusPreys) {
				if (VoreDataEntry* pyData = VoreData::IsValidGet(el)) {
					if (!pyData->aAlive) {
						continue;
					}
					pyData->pyDigestion = dType;
					if (pyData->aIsChar) {
						pyData->pyStruggle = GetStruggle(pyData->get()->As<RE::Actor>(), predData->get()->As<RE::Actor>(), pyData->pyLocus, pyData->pyDigestion);
					} else if (pyData->pyDigestion == VoreState::hLethal) {
						pyData->aAlive = false;
					}
					pyData->CalcFast();
					pyData->CalcSlow();

					flog::info("Prey {} digestion set to {}", Name::GetName(pyData->get()), (uint8_t)pyData->pyDigestion);
				}
			}
			flog::info("Pred {} digestion in locus {} set to {}", Name::GetName(predData->get()), (uint8_t)locus, (uint8_t)predData->pdLoci[locus]);
		}
		if (doDialogueUpd) {
			Dialogue::OnDigestionChange(predData->get()->As<RE::Actor>());
		}
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

	void MoveToLocus(const RE::FormID& pred, const RE::FormID& prey, const Locus& locus, const Locus& locusSource)
	{
		if (!VoreData::IsValid(pred) || !VoreData::IsValid(prey)) {
			return;
		}
		if (CanMoveToLocus(pred, prey, locus, locusSource)) {
			auto& pyData = VoreData::Data[prey];
			pyData.pyLocus = locus;
			if (pyData.aAlive) {
				pyData.pyDigestion = VoreData::Data[pred].pdLoci[locus];
			}
			if (locus == lBowel) {
				pyData.pyLocusProcess = 100.0;
			} else {
				pyData.pyLocusProcess = 0.0;
			}
			pyData.CalcFast();
			pyData.CalcSlow();
			flog::info("Moved Prey {} to Locus {}", Name::GetName(pyData.get()), (uint8_t)locus);
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
				RE::TESObjectREFR* stomachCell = RE::TESForm::LookupByEditorID<RE::TESObjectREFR>("BellyAMarker");
				//flog::critical("FOUND CELL MARKER {}", (int)stomachCell);
				// test: can you move dead actors?
				prey->MoveTo(stomachCell);
				
				HidePrey(prey);

				/*if (!prey->IsPlayerRef()) {
					prey->EnableAI(false);
				}*/
			} else if (pred) {
				UnhidePrey(prey);

				/*if (!prey->IsPlayerRef()) {
					prey->EnableAI(true);
				}*/

				if (preyData && preyData->pyDigestProgress >= 100.0) {
					RE::TESObjectREFR* stomachDeadCell = RE::TESForm::LookupByEditorID<RE::TESObjectREFR>("BellyDMarker");
					prey->MoveTo(stomachDeadCell);
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
					// so I'll use the good 'ol bone piles
					// RE::TESRace* skeleRace = RE::TESForm::LookupByEditorID<RE::TESRace>("SkeletonRace");
					// prey->SwitchRace(skeleRace, prey->IsPlayerRef());

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

					flog::critical("RELEASING {}", Name::GetName(prey));
					flog::critical("Is prey persistent? {}", prey->IsPersistent());

				} else {
					prey->MoveTo(pred);
				}
			} else {
				UnhidePrey(prey);
			}
		} else {
			if (!show) {
				RE::TESObjectREFR* stomachCell = RE::TESForm::LookupByEditorID<RE::TESObjectREFR>("BellyIMarker");
				preyObj->MoveTo(stomachCell);
			} else if (pred) {
				if (preyData && preyData->pyDigestProgress >= 100.0) {
					uint32_t refCound = preyObj->QRefCount();

					flog::info("Ref cound {}", refCound);

					flog::critical("RELEASING {}", Name::GetName(preyObj));
					//finish this!!!!!!!!
				} else {
					preyObj->MoveTo(pred);
				}
			}
		}
	}

	void Swallow(RE::Actor* pred, std::vector<RE::TESObjectREFR*> preys, Locus locus, VoreState ldType, bool fullswallow)
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

		//initialize pred

		flog::info("Pred {}", Name::GetName(pred));
		RE::FormID predId = VoreData::MakeData(pred);
		if (!predId) {
			flog::warn("Bad pred, aborting!");
			return;
		}
		Vore::VoreDataEntry& predData = VoreData::Data[predId];

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

				RE::FormID preyId = VoreData::MakeData(preyA);
				if (!preyId) {
					flog::warn("broken prey");
					continue;
				}
				Vore::VoreDataEntry& preyData = VoreData::Data[preyId];

				RE::FormID oldPred = preyData.pred;

				if (oldPred) {
					if (VoreData::Data.contains(oldPred) && VoreData::Data[oldPred].prey.contains(preyId)) {
						Regurgitate(VoreData::Data[oldPred].get()->As<RE::Actor>(), preyId, RegType::rTransfer);
					}
				} else {
					SetPreyVisibility(preyA, pred, false, &preyData);
				}

				// don't delete a prey if we're planning on using them
				if (VoreGlobals::delete_queue.contains(preyId)) {
					VoreGlobals::delete_queue.erase(preyId);
				}

				//inset prey
				predData.prey.insert(preyId);

				preyData.aAlive = !(preyA->IsDead());

				preyData.pred = predId;
				preyData.pyLocus = locus;
				preyData.pyElimLocus = locus;
				preyData.pyDigestion = ldType;
				preyData.pyStruggle = GetStruggle(preyA, pred, locus, ldType);

				preyData.pyDigestProgress = 0;
				preyData.pySwallowProcess = fullswallow || !preyData.aAlive ? 100 : 20;

				//full tour related shit
				preyData.pyLocusMovement = (preyData.pyLocus == Locus::lBowel) ? VoreState::mIncrease : VoreState::mStill;
				preyData.pyLocusProcess = 0;

				preyData.CalcFast();
				preyData.CalcSlow();
				Dialogue::OnSwallow_Prey(pred, prey);

				preyCount++;

			} else {
				RE::TESBoundObject* base = prey->GetBaseObject();
				if (VoreGlobals::allowed_pickup.contains(base->GetFormType())) {
					flog::info("Pred {}, prey {}, eatable object", Name::GetName(pred), Name::GetName(prey));

					RE::FormID preyId = VoreData::MakeData(prey);
					Vore::VoreDataEntry& preyData = VoreData::Data[preyId];

					RE::FormID oldPred = preyData.pred;

					if (oldPred) {
						if (VoreData::Data.contains(oldPred) && VoreData::Data[oldPred].prey.contains(preyId)) {
							Regurgitate(VoreData::Data[oldPred].get()->As<RE::Actor>(), preyId, RegType::rTransfer);
						}
					} else {
						SetPreyVisibility(prey, pred, false, &preyData);
					}
					//auto* scForm = RE::TESForm::LookupByEditorID<RE::TESObjectCONT>("StomachContainer");
					//auto* scPtr = pred->PlaceObjectAtMe(scForm, false).get();
					//auto * pyItem = prey->
					//scPtr->GetContainer()->

					predData.prey.insert(preyId);

					// items that are "alive" are endoed items
					// dead items are items that are being digested
					preyData.aAlive = true;

					preyData.pred = predId;
					preyData.pyLocus = locus;
					preyData.pyElimLocus = locus;
					preyData.pyDigestion = ldType;
					preyData.pyStruggle = VoreState::sStill;

					preyData.pyDigestProgress = 0;
					preyData.pySwallowProcess = fullswallow || !preyData.aAlive ? 100 : 20;

					//full tour related shit
					preyData.pyLocusMovement = (preyData.pyLocus == Locus::lBowel) ? VoreState::mIncrease : VoreState::mStill;
					preyData.pyLocusProcess = 0;

					preyData.CalcFast();
					preyData.CalcSlow();

					preyCount++;
				}
				//armor
				//weapon
				//misc
				// alchemy
				// ingredient
				// book
				// Scroll
				// Ammo
				// SoulGem
				//
				// gold?
				// flora?
				// Tree
				//
				//furnitre
				//container
				flog::info("Base form {}", (int)prey->GetBaseObject()->GetFormType());
				flog::warn("not character, skipping for now");
			}
		}
		if (preyCount > 0) {
			//VoreGlobals::body_morphs->SetMorph(pred, "Vore prey belly", VoreGlobals::MORPH_KEY, 1.0);
			//VoreGlobals::body_morphs->UpdateModelWeight(pred);
			predData.pdUpdateGoal = true;
			Dialogue::OnSwallow_Pred(pred);
			SwitchToDigestion(predId, locus, ldType, true);
			if (fullswallow) {
				predData.UpdateSliderGoals();
				predData.PlayStomachSounds();
			}
			predData.SetPredUpdate(true);
		} else {
			flog::warn("No prey were swallowed");
		}
		predData.PlaySwallow();
		UI::VoreMenu::SetMenuMode(UI::kDefault);
		Log::PrintVoreData();
		flog::info("Swallow end");
	}

	void Swallow(RE::Actor* pred, RE::TESObjectREFR* prey, Locus locus, VoreState ldType, bool fullswallow)
	{
		std::vector<RE::TESObjectREFR*> preys = { prey };
		Swallow(pred, preys, locus, ldType, fullswallow);
	}

	void SwallowTarget(RE::Actor* pred, Locus locus, VoreState ldType, bool fullswallow)
	{
		//flog::info("{}'s target for swallow: {}", pred-> ,pred->GetDisplayFullName());
		//find target
		//Swallow(pred, target)

		if (pred == RE::PlayerCharacter::GetSingleton()) {
			Swallow(pred, Vore::Utils::GetCrosshairObject(), locus, ldType, fullswallow);
		} else {
			Swallow(pred, Vore::Utils::GetFrontObjects(pred, 1), locus, ldType, fullswallow);
		}
	}

	void Regurgitate(RE::Actor* pred, std::vector<RE::FormID> preys, RegType rtype)
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

		Vore::VoreDataEntry& predData = VoreData::Data[predId];

		//for nested vore
		RE::FormID topPred = predData.pred;

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
				predData.prey.erase(prey);
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

			predData.prey.erase(prey);

			//clear prey

			if (preyData->pyLocus == Locus::lStomach) {
				playVomit = true;
			} else if (preyData->pyLocus == Locus::lBowel) {
				playDisposal = true;
			}
			preyData->pyLocus = Locus::lNone;
			preyData->pyElimLocus = Locus::lNone;
			preyData->pyDigestion = VoreState::hNone;
			preyData->pyStruggle = VoreState::sStill;
			preyData->pyLocusMovement = VoreState::mStill;

			preyData->pySwallowProcess = 0;
			preyData->pyLocusProcess = 0;

			preyData->CalcFast(true);
			preyData->CalcSlow(true);


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
			Swallow(skyrim_cast<RE::Actor*>(VoreData::Data[topPred].get()), preysToSwallow, predData.pyLocus, predData.pyDigestion, true);
		}
		// regurgitation
		for (auto& [prey, pData] : preysToDelete) {
			SetPreyVisibility(prey, pred, true, pData);
			pData->pyDigestProgress = 0;
		}

		predData.pdUpdateGoal = true;

		// reset indigestion
		std::array<bool, Locus::NUMOFLOCI> indigestion = { false };
		for (const RE::FormID& prey : predData.prey) {
			if (VoreDataEntry* pyData = VoreData::IsValidGet(prey); pyData->pyLocus < Locus::NUMOFLOCI) {
				indigestion[pyData->pyLocus] = true;
			}
		}
		for (uint8_t l = Locus::lStomach; l < Locus::NUMOFLOCI; l++) {
			if (!indigestion[l]) {
				predData.pdIndigestion[l] = 0.0;
			}
		}
		SwitchToDigestion(predId, Locus::lNone, VoreState::hSafe, false);

		if (playVomit) {
			predData.PlayRegurgitation(false);
		}
		if (playDisposal) {
			predData.PlayRegurgitation(true);
		}

		Dialogue::Clear_Pred(pred);

		VoreData::SoftDelete(predId, !predData.aAlive);
		UI::VoreMenu::SetMenuMode(UI::kDefault);
		Log::PrintVoreData();
		flog::info("End of Regurgitation");
	}

	void Regurgitate(RE::Actor* pred, RE::FormID prey, RegType rtype)
	{
		std::vector<RE::FormID> preys = { prey };
		Regurgitate(pred, preys, rtype);
	}

	void RegurgitateAll(RE::Actor* pred, Locus locus, RegType rtype)
	{
		if (!pred) {
			flog::warn("Regurgitation: Missing pred");
			return;
		}
		if (!VoreData::IsPred(pred->GetFormID(), true)) {
			flog::warn("Regurgitation: {} is not a pred", Name::GetName(pred));
			return;
		}
		Regurgitate(pred, FilterPrey(pred->GetFormID(), locus, true), rtype);
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
					if (VoreData::Data[*it].aIsChar && VoreData::Data[*it].get() && VoreData::Data[*it].get()->Is3DLoaded() && VoreGlobals::body_morphs->HasBodyMorphKey(VoreData::Data[*it].get(), VoreGlobals::MORPH_KEY)) {
						VoreGlobals::body_morphs->ClearBodyMorphKeys(VoreData::Data[*it].get(), VoreGlobals::MORPH_KEY);
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
