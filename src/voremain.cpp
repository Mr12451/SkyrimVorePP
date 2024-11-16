#include "headers/voremain.h"
#include "headers/nutils.h"
#include "headers/settings.h"
#include "headers/times.h"
#include "headers/ui.h"
#include "headers/vutils.h"

namespace Vore::Core
{

	static VoreState GetStruggle(RE::Actor* prey, RE::Actor* pred, Locus locus, VoreState dType)
	{
		//placeholder!!!
		if (locus == Locus::lNone) {
			return VoreState::sStill;
		}
		//placeholder!!!
		if (prey->IsDead()) {
			return VoreState::sStill;
		}
		if (dType == VoreState::hLethal) {
			if (pred->IsPlayerRef()) {
				//Funcs::SendAssaultAlarm(prey);
			}
			return VoreState::sStruggling;
		}
		return VoreState::sStill;
	}

	void SwitchToDigestion(const RE::FormID& pred, const Locus& locus, const VoreState& dType, const bool& forceStopDigestion)
	{
		if (!VoreData::IsValid(pred)) {
			flog::warn("Switching to digestion failed: bad pred: {}", Name::GetName(pred));
			return;
		}
		if (dType == VoreState::hNone) {
			flog::warn("Switching to None digestion type! Returning.: {}", Name::GetName(pred));
			return;
		}
		if (locus == Locus::lNone) {
			for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
				SwitchToDigestion(pred, (Locus)i, dType, forceStopDigestion);
			}
			return;
		}
		std::vector<RE::FormID> locusPreys = FilterPrey(pred, locus, false);
		if (!forceStopDigestion && dType == VoreState::hSafe) {
			for (const RE::FormID& el : locusPreys) {
				auto& pyData = VoreData::Data[el];
				if (pyData.aAlive && (pyData.pyDigestion == hLethal || pyData.pyDigestion == hReformation)) {
					flog::trace("Can't switch locus {} from {} for {}", (uint8_t)locus, (uint8_t)VoreData::Data[pred].pdLoci[locus], Name::GetName(VoreData::Data[pred].get()));
					return;
				}
			}
		}
		VoreData::Data[pred].pdLoci[locus] = dType;
		for (const RE::FormID& el : locusPreys) {
			auto& pyData = VoreData::Data[el];
			if (!pyData.aAlive) {
				continue;
			}
			pyData.pyDigestion = dType;
			pyData.pyStruggle = GetStruggle(pyData.get()->As<RE::Actor>(), VoreData::Data[pred].get()->As<RE::Actor>(), pyData.pyLocus, pyData.pyDigestion);

			flog::info("Prey {} digestion set to {}", Name::GetName(pyData.get()), (uint8_t)pyData.pyDigestion);
		}
		flog::info("Pred {} digestion in locus {} set to {}", Name::GetName(VoreData::Data[pred].get()), (uint8_t)locus, (uint8_t)VoreData::Data[pred].pdLoci[locus]);
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

	bool CanBeRegurgitated(const VoreDataEntry& prey)
	{
		// prey is fully digested
		if (!prey.aAlive && prey.pyDigestProgress == 100) {
			return true;
		}
		// no full tour
		if (prey.aAlive && prey.pyLocusProcess == 0.0) {
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
			flog::info("Moved Prey {} to Locus {}", Name::GetName(pyData.get()), (uint8_t)locus);
		}
	}

	static void SetPreyVisibility(RE::Actor* prey, RE::Actor* pred, bool show, bool digested, double preySize)
	{
		if (!show) {
			Funcs::StopCombatAlarm(prey);
			Funcs::StopCombat(prey);
			//RE::TESObjectCELL* stomachCell = RE::TESForm::LookupByEditorID("QASmoke")->As<RE::TESObjectCELL>();
			RE::TESObjectREFR* stomachCell = RE::TESForm::LookupByEditorID<RE::TESObjectREFR>("BellyAMarker");
			//flog::critical("FOUND CELL MARKER {}", (int)stomachCell);
			// test: can you move dead actors?
			prey->MoveTo(stomachCell);

			Funcs::SetAlpha(prey, 0.0f, false);
			Funcs::SetGhost(prey, true);
			Funcs::SetAlpha(prey, 0.0f, false);
			Funcs::SetRestrained(prey, true);

			if (!prey->IsPlayerRef()) {
				prey->EnableAI(false);
			}
		} else {
			Funcs::SetAlpha(prey, 1.0f, false);
			Funcs::SetGhost(prey, false);
			Funcs::SetAlpha(prey, 1.0f, false);
			Funcs::SetRestrained(prey, false);

			if (!prey->IsPlayerRef()) {
				prey->EnableAI(true);
			}
			if (digested) {
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
				bones->SetSize(static_cast<float>(preySize / VoreGlobals::slider_one));
				for (auto& [i, j] : items) {
					prey->RemoveItem(i, j.first, RE::ITEM_REMOVE_REASON::kStoreInContainer, nullptr, bones);
				}


				flog::critical("RELEASING {}", Name::GetName(prey));
				flog::critical("Is prey persistent? {}", prey->IsPersistent());

			} else {
				prey->MoveTo(pred);
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
					SetPreyVisibility(preyA, pred, false, false, preyData.aSize);
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

				preyCount++;

			} else {
				flog::warn("not character, skipping for now");
			}
		}
		if (preyCount > 0) {
			//VoreGlobals::body_morphs->SetMorph(pred, "Vore prey belly", VoreGlobals::MORPH_KEY, 1.0);
			//VoreGlobals::body_morphs->UpdateModelWeight(pred);
			predData.pdUpdateGoal = true;
			SwitchToDigestion(predId, locus, ldType, true);
		} else {
			flog::warn("No prey were swallowed");
		}
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

		for (RE::FormID prey : preys) {
			flog::info("Prey {}", Name::GetName(prey));
			if (!VoreData::IsPrey(prey)) {
				flog::warn("ERASING BAD PREY");
				predData.prey.erase(prey);
				preysToErase.push_back(prey);
				continue;
			}

			VoreDataEntry& preyData = VoreData::Data[prey];

			if (preyData.pred != predId) {
				flog::warn("Prey and pred mismatch! Skipping.");
				continue;
			}

			if (preyData.pyDigestProgress > 0 && preyData.pyDigestProgress < 100) {
				flog::info("Prey is not digested yet.");
				continue;
			}

			if (rtype != rAll && !CanBeRegurgitated(preyData)) {
				flog::info("Can't regurgitate {}", Name::GetName(preyData.get()));
				continue;
			}

			if (preyData.aCharType == RE::FormType::ActorCharacter) {
				// erase prey from pred
				predData.prey.erase(prey);

				//clear prey
				preyData.pyLocus = Locus::lNone;
				preyData.pyElimLocus = Locus::lNone;
				preyData.pyDigestion = VoreState::hNone;
				preyData.pyStruggle = VoreState::sStill;
				preyData.pyLocusMovement = VoreState::mStill;

				preyData.pySwallowProcess = 0;
				preyData.pyLocusProcess = 0;

				if (rtype == RegType::rTransfer) {
					preyData.pyDigestProgress = 0;
					flog::info("prey transfer");
				} else if (topPred) {
					preyData.pyDigestProgress = 0;
					preysToSwallow.push_back(preyData.get());
				} else {
					preyData.pred = 0;
					preyData.pdUpdateGoal = true;
					VoreData::SoftDelete(prey, !preyData.aAlive);
					preysToDelete.push_back({ preyData.get(), &preyData });
				}

			} else {
				preysToErase.push_back(prey);
				flog::warn("not character, skipping");
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
			RE::Actor* preyA = skyrim_cast<RE::Actor*>(prey);
			SetPreyVisibility(preyA, pred, true, pData->pyDigestProgress == 100, pData->aSize);
			pData->pyDigestProgress = 0;
		}

		predData.pdUpdateGoal = true;

		// reset indigestion
		std::array<bool, Locus::NUMOFLOCI> indigestion = { false };
		for (const RE::FormID& prey : predData.prey) {
			if (VoreData::Data.contains(prey) && VoreData::Data[prey].pyLocus < Locus::NUMOFLOCI) {
				indigestion[VoreData::Data[prey].pyLocus] = true;
			}
		}
		for (uint8_t l = Locus::lStomach; l < Locus::NUMOFLOCI; l++) {
			if (!indigestion[l]) {
				predData.pdIndigestion[l] = 0.0;
			}
		}
		SwitchToDigestion(predId, Locus::lNone, VoreState::hSafe, true);
		
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

	static void UpdateStruggleGoals(RE::FormID pred)
	{
		if (!VoreData::IsValid(pred)) {
			flog::error("Attempting to update slider goal for invalid pred: {}!", Name::GetName(pred));
			return;
		}

		//if fast update happens too slow or slider max step is too big (aka morphing happens too fast), this may break
		//float deltaMax = std::min(static_cast<float>(VoreSettings::fast_update) * VoreSettings::slider_struggle_maxstep, 100.0f);

		VoreDataEntry& predData = VoreData::Data[pred];

		predData.pdUpdateStruggleSlider = true;

		std::array<float, Locus::NUMOFLOCI> accum_struggle = { 0.0f };
		bool quitStruggle = true;

		for (const RE::FormID& prey : predData.prey) {
			if (!VoreData::IsValid(prey)) {
				flog::error("Found an invalid prey: {}!", Name::GetName(prey));
				continue;
			}
			VoreDataEntry& preyData = VoreData::Data[prey];
			if (preyData.aAlive && preyData.pyStruggle == VoreState::sStruggling) {
				accum_struggle[preyData.pyLocus] += static_cast<float>(preyData.aSize * AV::GetPercentageAV(preyData.get()->As<RE::Actor>(), RE::ActorValue::kStamina) / 1.5f);
				//accum_struggle[preyData.pyLocus] += (float)preyData.aSize;
			}
		}

		auto& ssliders = VoreSettings::struggle_sliders;

		for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
			//skip sliders that are not defined by user

			if (accum_struggle[i] == 0.0f && predData.pdAccumStruggle[i] == 0.0f) {
				continue;
			} else if (accum_struggle[i] == 0.0f) {
				for (int j = 0; j < ssliders[i].size(); j++) {
					//the id of this slider in predData
					int sliderId = i * Locus::NUMOFLOCI + j;
					predData.pdStruggleGoalStep[sliderId] = predData.pdStruggleSliders[sliderId];
					predData.pdStruggleGoal[sliderId] = 0.0f;
				}
				continue;
			} else {
				quitStruggle = false;
				predData.pdAccumStruggle[i] = accum_struggle[i];
			}

			for (int j = 0; j < ssliders[i].size(); j++) {
				//the id of this slider in predData
				int sliderId = i * Locus::NUMOFLOCI + j;

				// goal diff is reset by updatebelly when slider reaches it's goal
				if (predData.pdStruggleGoalStep[sliderId] == 0.0f) {
					float maxValue = accum_struggle[i];
					if (predData.pdStruggleGoal[sliderId] > maxValue / 2.0f) {
						predData.pdStruggleGoal[sliderId] = randfloat(0.0, maxValue / 2.0f);
					} else {
						predData.pdStruggleGoal[sliderId] = randfloat(maxValue / 2.0f, maxValue);
					}

					predData.pdStruggleGoalStep[sliderId] = VoreSettings::slider_maxstep * std::pow(std::abs(predData.pdStruggleGoal[sliderId] - predData.pdStruggleSliders[sliderId]) / VoreGlobals::slider_one, 0.75f);
				}
				// i got brain damage when writing this lol
			}
		}

		if (quitStruggle) {
			predData.pdUpdateStruggleGoal = false;
			return;
		}
	}

	//updates slider goal
	static void UpdateSliderGoals(RE::FormID pred)
	{
		if (!VoreData::IsValid(pred)) {
			flog::error("Attempting to update slider goal for invalid character: {}!", Name::GetName(pred));
			return;
		}
		VoreDataEntry& predData = VoreData::Data[pred];
		predData.pdUpdateGoal = false;
		predData.pdUpdateSlider = true;

		predData.pdGoal.fill(0.0f);

		predData.pdFullBurden = 0.0;

		//calculate locus size with switch

		for (const RE::FormID& prey : predData.prey) {
			VoreDataEntry& preyData = VoreData::Data[prey];
			//calculate prey full size
			//calculate prey full weight
			double preySize = 0.0;
			preyData.GetSize(preySize);
			predData.pdFullBurden += preySize;

			//unfortunately sliders are floats, while everything else in this mod is a double
			//doubles can be better for precision at really small wg/digestion steps, that's why I use them
			//so these casts are necessary
			switch (preyData.pyLocus) {
			// for swallowing
			case Locus::lStomach:
				predData.pdGoal[LocusSliders::uThroat] += static_cast<float>(preySize * (1 - preyData.pySwallowProcess / 100.0));
				predData.pdGoal[LocusSliders::uStomach] += static_cast<float>(preySize * preyData.pySwallowProcess / 100.0);
				break;
			// for full tour
			case Locus::lBowel:
				predData.pdGoal[LocusSliders::uBowel] += static_cast<float>(preySize * (1 - preyData.pyLocusProcess / 100.0));
				predData.pdGoal[LocusSliders::uStomach] += static_cast<float>(preySize * preyData.pyLocusProcess / 100.0);
				break;
			case Locus::lBreastl:
				predData.pdGoal[LocusSliders::uBreastl] += static_cast<float>(preySize);
				break;
			case Locus::lBreastr:
				predData.pdGoal[LocusSliders::uBreastr] += static_cast<float>(preySize);
				break;
			case Locus::lWomb:
				predData.pdGoal[LocusSliders::uWomb] += static_cast<float>(preySize);
				break;
			// for swallowing cv
			case Locus::lBalls:
				predData.pdGoal[LocusSliders::uCock] += static_cast<float>(preySize * (1 - preyData.pySwallowProcess / 100.0));
				predData.pdGoal[LocusSliders::uBalls] += static_cast<float>(preySize * preyData.pySwallowProcess / 100.0);
				break;
			}
		}

		//update other sliders

		// locus fat
		predData.pdGoal[LocusSliders::uFatBelly] = static_cast<float>(predData.pdGrowthLocus[0]);
		predData.pdGoal[LocusSliders::uFatAss] = static_cast<float>(predData.pdGrowthLocus[1]);
		predData.pdGoal[LocusSliders::uFatBreasts] = static_cast<float>(predData.pdGrowthLocus[2]);
		predData.pdGoal[LocusSliders::uFatCock] = static_cast<float>(predData.pdGrowthLocus[3]);

		//fat and perma fat
		predData.pdGoal[LocusSliders::uFatLow] = static_cast<float>((predData.pdFat < 0) ? predData.pdFat : 0);
		predData.pdGoal[LocusSliders::uFatHigh] = static_cast<float>((predData.pdFat >= 0) ? predData.pdFat : 0);
		predData.pdGoal[LocusSliders::uGrowthLow] = static_cast<float>((predData.pdFatgrowth < 0) ? predData.pdFatgrowth : 0);
		predData.pdGoal[LocusSliders::uGrowthHigh] = static_cast<float>((predData.pdFatgrowth >= 0) ? predData.pdFatgrowth : 0);

		for (uint8_t i = 0; i < LocusSliders::NUMOFSLIDERS; i++) {
			predData.pdGoalStep[i] = VoreSettings::slider_maxstep * std::pow(std::abs(predData.pdGoal[i] - predData.pdSliders[i]) / VoreGlobals::slider_one, 0.75f);
		}

		//size growth

		//slow down pred based on full burden
	}

	static void FinishDigestion([[maybe_unused]] const RE::FormID& prey, VoreDataEntry& preyData)
	{
		//play some sound
		//(possibly) turn into bones?
		if (preyData.aCharType == RE::FormType::ActorCharacter) {
			flog::info("{} was fully digested", Name::GetName(preyData.get()));
			//RE::Actor* preyA = preyData.get()->As<RE::Actor>();
			//preyA->SetCollision
		}
	}

	//updates dead characters and idle preds
	static void UpdateSlow(const double& delta)
	{
		double digestBase = VoreSettings::digestion_amount_base * delta;

		//process prey
		for (auto& [key, val] : VoreData::Data) {
			if (!val.pred) {
				continue;
			}
			VoreDataEntry& pred = VoreData::Data[val.pred];
			//full tour process for living prey
			if (val.aAlive) {
				if (val.pyLocus == Locus::lBowel) {
					if (val.pyLocusMovement == VoreState::mIncrease) {
						val.pyLocusProcess += VoreSettings::locus_process_speed * delta;
					} else if (val.pyLocusMovement == VoreState::mDecrease) {
						val.pyLocusProcess -= VoreSettings::locus_process_speed * delta;
					}
					if (val.pyLocusMovement != VoreState::mStill) {
						pred.pdUpdateGoal = true;
					}
					if (val.pyLocusProcess >= 100) {
						val.pyLocusProcess = 100;
						val.pyLocusMovement = VoreState::mStill;
					} else if (val.pyLocusProcess <= 0) {
						val.pyLocusProcess = 0;
						val.pyLocusMovement = VoreState::mStill;
					}
				}
			}
			//digestion
			else if (val.pyDigestProgress < 100) {
				//aWeight can increase / decrease digestion time
				//calculate speed -> (100 - val.pyDigestProgress) / VoreSettings::digestion_amount_base * 100 / val.aSize
				//get actual digestion time
				// multiply it by speed
				double digestMod = 1 / std::max(std::pow(val.aSize / VoreGlobals::slider_one, 0.5), 0.6);
				double toDigest = (100 - val.pyDigestProgress) / digestMod;
				const double& newBase = digestBase > toDigest ? toDigest : digestBase;
				val.pyDigestProgress += newBase * digestMod;
				if (val.pyDigestProgress >= 100) {
					val.pyDigestProgress = 100;
					FinishDigestion(key, val);
				}
				if (val.pyLocus == Locus::lBowel) {
					val.pyLocusProcess = 100 - val.pyDigestProgress;
				}
				if (val.pyLocus == lStomach && val.pyDigestProgress > 30) {
					MoveToLocus(val.pred, key, Locus::lBowel);
					/*RE::TESObjectREFR* stomachDeadCell = RE::TESForm::LookupByEditorID<RE::TESObjectREFR>("BellyDMarker");
					val.get()->MoveTo(stomachDeadCell);*/
				}

				// weight gain

				// locus increase
				// fat
				// fat growth
				// height (size) increase; (for me) check gts for info
				for (uint8_t i = 0; i < 4; i++) {
					pred.pdGrowthLocus[i] += newBase * VoreSettings::voretypes_partgain[val.pyElimLocus][i] * VoreSettings::wg_locusgrowth;
				}

				pred.pdFat += newBase * VoreSettings::wg_fattemp;
				pred.pdFatgrowth += newBase * VoreSettings::wg_fatlong;
				pred.pdSizegrowth += newBase * VoreSettings::wg_sizegrowth;
				pred.pdUpdateGoal = true;
			}
		}

		//updates all preds
		for (auto& [key, val] : VoreData::Data) {
			if (!val.aAlive) {
				continue;
			}
			//RE::Actor* asActor = val.get()->As<RE::Actor>();
			bool doGoalUpdate = false;
			//reduce wg
			if (val.pdFat > 0) {
				val.pdFat = std::max(val.pdFat - VoreSettings::wg_loss_temp * delta, 0.0);
				doGoalUpdate = true;
			}
			if (val.pdFatgrowth > 0) {
				val.pdFatgrowth = std::max(val.pdFatgrowth - VoreSettings::wg_loss_long * delta, 0.0);
				doGoalUpdate = true;
			}
			if (val.pdSizegrowth > 0) {
				val.pdSizegrowth = std::max(val.pdFat - VoreSettings::wg_loss_size * delta, 0.0);
				// DO SIZE UPDATE
			}
			for (auto& el : val.pdGrowthLocus) {
				if (el > 0) {
					el = std::max(el - VoreSettings::wg_loss_locus * delta, 0.0);
					doGoalUpdate = true;
				}
			}
			//reduce acid and indigestion
			for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
				if (val.pdAcid[i] > 0 && val.pdLoci[i] != VoreState::hLethal) {
					val.pdAcid[i] = std::max(val.pdAcid[i] - VoreSettings::acid_loss * delta, 0.0);
				}
				if (val.pdIndigestion[i] > 0) {
					val.pdIndigestion[i] = std::max(val.pdIndigestion[i] - VoreSettings::indigestion_loss * delta, 0.0);
				}
			}
			//only update goal for top preds
			if (doGoalUpdate && !val.pred) {
				val.pdUpdateGoal = true;
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
					if (VoreData::Data[*it].get() && VoreData::Data[*it].get()->Is3DLoaded() && VoreGlobals::body_morphs->HasBodyMorphKey(VoreData::Data[*it].get(), VoreGlobals::MORPH_KEY)) {
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

	static void HandlePreyDeathImmidiate(VoreDataEntry& preyData)
	{
		preyData.aAlive = false;
		preyData.pyDigestProgress = 0.0;
		preyData.pyElimLocus = preyData.pyLocus;
		preyData.pyLocusMovement = mStill;
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
			if (!val.aAlive || !val.pred) {
				continue;
			}

			if (!VoreData::Data.contains(val.pred)) {
				flog::critical("!!!!!Found a prey with a broken pred: {}", Name::GetName(key));
				continue;
			}

			RE::Actor* asActor = val.get()->As<RE::Actor>();

			VoreDataEntry& predData = VoreData::Data[val.pred];
			//swallow process

			if (val.pySwallowProcess < 100) {
				needUIUpdate = true;
				if (delta > 10) {
					val.pySwallowProcess = 100;
				}
				if (VoreSettings::swallow_auto) {
					val.pySwallowProcess += VoreSettings::swallow_auto_speed * 1 / std::max(std::pow(val.aSize / VoreGlobals::slider_one, 0.3), 0.7) * delta;
					predData.pdUpdateGoal = true;
				} else {
					val.pySwallowProcess -= VoreSettings::swallow_decrease_speed * delta;
					predData.pdUpdateGoal = true;
				}

				//finish swallow
				if (val.pySwallowProcess >= 100) {
					val.pySwallowProcess = 100;
					predData.pdUpdateGoal = true;
				}
				//regurgitate prey because they escaped
				else if (val.pySwallowProcess < 0) {
					val.pySwallowProcess = 0;
					Regurgitate(predData.get()->As<RE::Actor>(), key, RegType::rAll);
				}

			}
			if (val.pySwallowProcess == 100 && asActor) {
				//process digestion
				if (val.pyDigestion == VoreState::hLethal) {
					needUIUpdate = true;
					// calculate if the actor escapes or dies during long digestion
					if (delta > 10 && val.pyStruggle != sStill) {
						//can the prey struggle out while he lives
						//this is not entirely accurate because it doesn't account for the struggles of other prey,
						//and acid damage increase
						//but it also doesn't account stamina drain and regen, so it's fine for the time being

						predData.pdAcid[val.pyLocus] = 100;
						double ttl = AV::GetAV(asActor, RE::ActorValue::kHealth) / VoreSettings::acid_damage;
						if (ttl <= delta && predData.pdIndigestion[val.pyLocus] + VoreSettings::struggle_amount * ttl > 100) {
							Regurgitate(predData.get()->As<RE::Actor>(), key, RegType::rAll);
							continue;
						}
					}
					predData.pdAcid[val.pyLocus] = std::min(predData.pdAcid[val.pyLocus] + VoreSettings::acid_gain * delta, 100.0);
					double acidMulti = predData.pdAcid[val.pyLocus] / 100.0;
					if (AV::GetAV(asActor, RE::ActorValue::kHealth) - VoreSettings::acid_damage * delta <= 0) {
						// LATER implement entrapment for essential/protected instead of regurgitation NPCs

						if (asActor->IsEssential()) {
							if (VoreSettings::digest_essential) {
								//asActor->boolFlags = (unsigned int)(asActor->boolFlags) & ~(unsigned int)RE::Actor::BOOL_FLAGS::kEssential;
								asActor->GetActorBase()->actorData.actorBaseFlags.reset(RE::ACTOR_BASE_DATA::Flag::kEssential);
								AV::DamageAV(asActor, RE::ActorValue::kHealth, AV::GetAV(asActor, RE::ActorValue::kHealth));

								HandlePreyDeathImmidiate(val);
							} else {
								Regurgitate(predData.get()->As<RE::Actor>(), key, RegType::rAll);
							}
						} else if (asActor->IsProtected()) {
							if (VoreSettings::digest_protected) {
								asActor->GetActorBase()->actorData.actorBaseFlags.reset(RE::ACTOR_BASE_DATA::Flag::kProtected);
								AV::DamageAV(asActor, RE::ActorValue::kHealth, AV::GetAV(asActor, RE::ActorValue::kHealth));

								HandlePreyDeathImmidiate(val);
							} else {
								Regurgitate(predData.get()->As<RE::Actor>(), key, RegType::rAll);
							}
						} else {
							AV::DamageAV(asActor, RE::ActorValue::kHealth, AV::GetAV(asActor, RE::ActorValue::kHealth));
							//asActor->KillImmediate();
							HandlePreyDeathImmidiate(val);
						}
						
						predData.pdUpdateStruggleGoal = true;
						continue;
						//
					} else {
						AV::DamageAV(asActor, RE::ActorValue::kHealth, VoreSettings::acid_damage * delta * acidMulti);
					}
				} else if (val.pyDigestion == VoreState::hReformation) {
					needUIUpdate = true;
					AV::DamageAV(asActor, RE::ActorValue::kHealth, -VoreSettings::acid_damage * delta);
				}
				// handle crime and aggro ????????????????????????

				// handle struggle
				if (val.pyStruggle == VoreState::sStruggling) {
					needUIUpdate = true;
					if (AV::GetAV(asActor, RE::ActorValue::kStamina) > 0) {
						//damage prey's stamina and pred's struggle bar for this locus
						AV::DamageAV(asActor, RE::ActorValue::kStamina, VoreSettings::struggle_stamina * delta);
						predData.pdIndigestion[val.pyLocus] += VoreSettings::struggle_amount * delta;

						if (predData.pdIndigestion[val.pyLocus] >= 100) {
							RegurgitateAll(predData.get()->As<RE::Actor>(), val.pyLocus);
						}
					} else {
						val.pyStruggle = VoreState::sExhausted;
					}
					//pred is top pred
					if (!predData.pred) {
						predData.pdUpdateStruggleGoal = true;
					}

				} else if (val.pyStruggle == VoreState::sExhausted) {
					needUIUpdate = true;
					//restore stamina
					AV::DamageAV(asActor, RE::ActorValue::kStamina, -AV::GetAV(asActor, RE::ActorValue::kStaminaRate) * AV::GetAV(asActor, RE::ActorValue::kStaminaRateMult) / 100.0 * delta);
					if (AV::GetPercentageAV(asActor, RE::ActorValue::kStamina) >= 0.9) {
						val.pyStruggle = VoreState::sStruggling;
					}
				}
			}
		}
		//updates top preds
		for (auto const& [key, val] : VoreData::Data) {
			if (!val.pred) {
				if (val.pdUpdateGoal) {
					UpdateSliderGoals(key);
				}
				if (val.pdUpdateStruggleGoal) {
					UpdateStruggleGoals(key);
				}
			}
		}
		UI::VoreMenu::NeedUpdate = UI::VoreMenu::NeedUpdate || needUIUpdate;
	}

	void SetupTimers()
	{
		Timer digUpdatef(1, VoreSettings::fast_update, UpdateFast);
		Timer digUpdateS(2, VoreSettings::slow_update, UpdateSlow);
		Timer belUpdatef(3, (double)VoreSettings::belly_fast_update, UpdateBelly);
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
		for (auto const& [key, val] : VoreData::Data) {
			if (!val.pred) {
				if (val.pdUpdateGoal) {
					UpdateSliderGoals(key);
				}
				if (val.pdUpdateStruggleGoal) {
					UpdateStruggleGoals(key);
				}
			}
		}
		UpdateBelly(delta);
	}
}
