#include "headers/voremain.h"
#include "headers/nutils.h"
#include "headers/settings.h"
#include "headers/times.h"
#include "headers/ui.h"
#include "headers/vutils.h"

namespace Vore::Core
{

	VoreState GetStruggle(RE::Actor* prey, Locus locus, VoreState dType)
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
			Funcs::SendAssaultAlarm(prey);
			return VoreState::sStruggling;
		}
		return VoreState::sStill;
	}

	void SwitchToDigestion(const RE::FormID& pred, const Locus& locus, const VoreState& dType, const bool& forceStopDigestion)
	{
		if (!VoreData::IsPred(pred, true)) {
			flog::warn("Switching to digestion failed: bad pred: {}", Name::GetName(pred));
			return;
		}
		if (dType == VoreState::hNone) {
			flog::warn("Switching to None digestion type! Returning.: {}", Name::GetName(pred));
			return;
		}
		if (VoreData::Data[pred].pdLoci[locus] == VoreState::hNone ||
			dType == VoreState::hSafe && forceStopDigestion || dType != VoreState::hSafe) {
			VoreData::Data[pred].pdLoci[locus] = dType;
			for (auto& el : FilterPrey(pred, locus, false)) {
				auto& pyData = VoreData::Data[el];
				pyData.pyDigestion = dType;
				pyData.pyStruggle = GetStruggle(pyData.get()->As<RE::Actor>(), pyData.pyLocus, pyData.pyDigestion);

				flog::info("Prey {} digestion set to {}", Name::GetName(pyData.get()), (uint8_t)pyData.pyDigestion);
			}
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
			pyData.pyDigestion = VoreData::Data[pred].pdLoci[locus];
			if (locus == lBowel) {
				pyData.pyLocusProcess = 100.0;
			} else {
				pyData.pyLocusProcess = 0.0;
			}
			flog::info("Moved Prey {} to Locus {}", Name::GetName(pyData.get()), (uint8_t)locus);
		}
	}

	void SetPreyVisibility(RE::Actor* prey, bool show)
	{
		if (!show) {
			Funcs::StopCombatAlarm(prey);
			Funcs::StopCombat(prey);
		}
		Funcs::SetAlpha(prey, (show) ? 1.0f : 0.0f, false);
		Funcs::SetGhost(prey, !show);
		Funcs::SetAlpha(prey, (show) ? 1.0f : 0.0f, false);
		Funcs::SetRestrained(prey, !show);

		/*
		Funcs::SetAlpha(preyA, 1.0f, false);
		Funcs::SetGhost(preyA, false);
		Funcs::SetAlpha(preyA, 1.0f, false);
		Funcs::SetRestrained(preyA, false);
		*/
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
					SetPreyVisibility(preyA, false);
				}

				//inset prey
				predData.prey.insert(preyId);

				preyData.aAlive = !(preyA->IsDead());

				preyData.pred = predId;
				preyData.pyLocus = locus;
				preyData.pyDigestion = ldType;
				preyData.pyStruggle = GetStruggle(preyA, locus, ldType);

				preyData.pyDigestProgress = 0;
				preyData.pySwallowProcess = fullswallow ? 100 : 20;

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
			SwitchToDigestion(predId, locus, ldType, false);
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
		//these preys will be regurgitated
		std::vector<RE::TESObjectREFR*> preysToDelete = {};
		//these preys will be transferred to top pred
		std::vector<RE::TESObjectREFR*> preysToSwallow = {};

		for (RE::FormID prey : preys) {
			flog::info("Prey {}", Name::GetName(prey));
			if (!VoreData::IsPrey(prey)) {
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

				preyData.pyDigestProgress = 0;
				preyData.pySwallowProcess = 0;
				preyData.pyLocusProcess = 0;

				if (rtype == RegType::rTransfer) {
					flog::info("prey transfer");
				} else if (topPred) {
					preysToSwallow.push_back(preyData.get());
				} else {
					preyData.pred = 0;
					VoreData::SoftDelete(prey, !preyData.aAlive);
					preysToDelete.push_back(preyData.get());
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
		// regurgitateion
		for (auto& el : preysToDelete) {
			RE::Actor* preyA = skyrim_cast<RE::Actor*>(el);
			SetPreyVisibility(preyA, true);
			//teleportation goes here
		}

		predData.pdUpdateGoal = true;

		// reset indigestion
		std::array<bool, Locus::NUMOFLOCI> indigestion = { false };
		for (RE::FormID prey : predData.prey) {
			if (VoreData::Data.contains(prey) && VoreData::Data[prey].pyLocus < Locus::NUMOFLOCI) {
				indigestion[VoreData::Data[prey].pyLocus] = true;
			}
		}
		for (uint8_t l = Locus::lStomach; l < Locus::NUMOFLOCI; l++) {
			if (!indigestion[l]) {
				predData.pdIndigestion[l] = 0.0;
			}
		}
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

	void UpdateStruggleGoals(RE::FormID pred)
	{
		if (!VoreData::IsValid(pred)) {
			flog::error("Attempting to update slider goal for invalid pred: {}!", Name::GetName(pred));
			return;
		}

		//if fast update happens too slow or slider max step is too big (aka morphing happens too fast), this may break
		float deltaMax = std::min(static_cast<float>(VoreSettings::fast_update) * VoreSettings::slider_maxstep, 100.0f);

		VoreDataEntry& predData = VoreData::Data[pred];

		predData.pdUpdateStruggleSlider = true;

		predData.pdStruggleGoal.fill(0.0f);
		// if we don't need to randomize sliders

		std::array<float, Locus::NUMOFLOCI> accum_struggle = { 0.0f };
		bool quitStruggle = true;

		for (auto& prey : predData.prey) {
			if (!VoreData::IsValid(prey)) {
				flog::error("Found an invalid prey: {}!", Name::GetName(prey));
				continue;
			}
			VoreDataEntry& preyData = VoreData::Data[prey];
			if (preyData.pyStruggle == VoreState::sStruggling) {
				quitStruggle = false;
				accum_struggle[preyData.pyLocus] += static_cast<float>(preyData.aSize * AV::GetPercentageAV(preyData.get()->As<RE::Actor>(), RE::ActorValue::kStamina) * 2);
			}
		}

		if (quitStruggle) {
			predData.pdUpdateStruggleGoal = false;
			return;
		}

		auto& ssliders = VoreSettings::struggle_sliders;

		for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
			//skip sliders that are not defined by user
			if (accum_struggle[i] == 0.0f) {
				continue;
			}
			for (int j = 0; j < ssliders[i].size(); j++) {
				//the id of this slider in predData
				int sliderId = i * Locus::NUMOFLOCI + j;
				//if the slider will reach it's goal before the next fast_update, make a new random goal
				//the new random goal will be at least deltamax distance from this goal
				if (std::abs(predData.pdStruggleGoal[sliderId] - predData.pdStruggleSliders[sliderId]) < deltaMax) {
					float maxValue = accum_struggle[i];
					//if the new goal is lower than current value AND the new goal will not end up being negative
					if (predData.pdStruggleSliders[sliderId] - deltaMax > 0 &&
						(maxValue <= predData.pdStruggleSliders[sliderId] ||
							predData.pdStruggleSliders[sliderId] > maxValue - predData.pdStruggleSliders[sliderId])) {
						//make a new random goal
						predData.pdStruggleGoal[sliderId] = randfloat(0.0f, predData.pdStruggleSliders[sliderId] - deltaMax);
					} else {
						if (predData.pdStruggleSliders[sliderId] + deltaMax < maxValue) {
							predData.pdStruggleGoal[sliderId] = randfloat(predData.pdStruggleSliders[sliderId] + deltaMax, maxValue);
						}
						// old value was more than max but less than delta
						else {
							predData.pdStruggleGoal[sliderId] = predData.pdStruggleSliders[sliderId] + deltaMax;
						}
					}
					//flog::trace("Setting slider {} to {}", sliderId, predData.pdStruggleGoal[sliderId]);
				}
				// i got brain damage when writing this lol
			}
		}
	}

	//updates slider goal
	void UpdateSliderGoals(RE::FormID pred)
	{
		if (!VoreData::IsValid(pred)) {
			flog::error("Attempting to update slider goal for invalid pred: {}!", Name::GetName(pred));
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
			double thisBurden = 0.0;
			double digestionMod = 1 - preyData.pyDigestProgress / 100.0;
			preyData.GetSizeWeight(preySize, thisBurden);
			preySize *= digestionMod;
			predData.pdFullBurden += thisBurden * digestionMod;

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
		for (size_t i = 0; i < (size_t)(LocusSliders::uFatLow - LocusSliders::uFatBelly); i++) {
			predData.pdGoal[i + LocusSliders::uFatBelly] = static_cast<float>(predData.pdGrowthLocus[i]);
		}

		//fat and perma fat
		predData.pdGoal[LocusSliders::uFatLow] = static_cast<float>((predData.pdFat < 0) ? predData.pdFat : 0);
		predData.pdGoal[LocusSliders::uFatHigh] = static_cast<float>((predData.pdFat >= 0) ? predData.pdFat : 0);
		predData.pdGoal[LocusSliders::uGrowthLow] = static_cast<float>((predData.pdFatgrowth < 0) ? predData.pdFatgrowth : 0);
		predData.pdGoal[LocusSliders::uGrowthHigh] = static_cast<float>((predData.pdFatgrowth >= 0) ? predData.pdFatgrowth : 0);

		//size growth

		//slow down pred based on full burden
	}

	//updates dead characters and idle preds
	void UpdateSlow()
	{
		static double& delta = VoreSettings::slow_update;
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
				val.pyDigestProgress = std::min(val.pyDigestProgress + digestBase * 100 / val.aWeight, 100.0);
				if (val.pyLocus == Locus::lBowel) {
					val.pyLocusProcess = 100 - val.pyDigestProgress;
				}
				// weight gain

				// locus increase
				// fat
				// fat growth
				// height (size) increase; (for me) check gts for info
				for (uint8_t i = 0; i < LocusSliders::uFatLow - LocusSliders::uFatBelly; i++) {
					pred.pdGrowthLocus[i] += digestBase * VoreSettings::voretypes_partgain[val.pyElimLocus][i] * VoreSettings::wg_locusgrowth;
				}
				pred.pdFat += digestBase * VoreSettings::wg_fattemp;
				pred.pdFatgrowth += digestBase * VoreSettings::wg_fatlong;
				pred.pdSizegrowth += digestBase * VoreSettings::wg_sizegrowth;
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
				doGoalUpdate = true;
			}
			for (auto& el : val.pdGrowthLocus) {
				if (el > 0) {
					el = std::max(el - VoreSettings::wg_loss_locus * delta, 0.0);
					doGoalUpdate = true;
				}
			}
			//reduce indigestion
			for (auto& el : val.pdIndigestion) {
				if (el > 0) {
					el = std::max(el - VoreSettings::indigestion_loss * delta, 0.0);
					doGoalUpdate = true;
				}
			}
			//only update goal for top preds
			if (doGoalUpdate && !val.pred) {
				val.pdUpdateGoal = true;
			}
		}

		//handle erase queue
		// wait until all sliders are zeroed

		std::vector<RE::FormID>& dq = VoreGlobals::delete_queue;
		for (auto it = dq.begin(); it != dq.end();) {
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
					VoreGlobals::body_morphs->ClearBodyMorphKeys(VoreData::Data[*it].get(), VoreGlobals::MORPH_KEY);
					VoreData::Data.erase(*it);
					flog::info("Deleting {} from VoreData", Name::GetName(*it));
					it = dq.erase(it);
				}
			}
			// skip this element and delete it from dq because it's not in Data
			else {
				it = dq.erase(it);
			}
		}

		UI::VoreMenu::NeedUpdate = true;
	}

	//updates live characters
	void UpdateFast()
	{
		static double& delta = VoreSettings::fast_update;

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
				if (VoreSettings::swallow_auto) {
					val.pySwallowProcess += VoreSettings::swallow_auto_speed * delta;
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

			} else if (asActor) {
				//process digestion
				if (val.pyDigestion == VoreState::hLethal) {
					if (AV::GetAV(asActor, RE::ActorValue::kHealth) - VoreSettings::acid_damage * delta <= 0) {
						// LATER implement entrapment for essential/protected instead of regurgitation NPCs

						if (asActor->IsEssential()) {
							if (VoreSettings::digest_essential) {
								asActor->KillImmediate();
							} else {
								Regurgitate(predData.get()->As<RE::Actor>(), key, RegType::rAll);
							}
						} else if (asActor->IsProtected()) {
							if (VoreSettings::digest_protected) {
								asActor->KillImmediate();
							} else {
								Regurgitate(predData.get()->As<RE::Actor>(), key, RegType::rAll);
							}
						} else {
							asActor->KillImmediate();
						}
						AV::DamageAV(asActor, RE::ActorValue::kHealth, AV::GetAV(asActor, RE::ActorValue::kHealth));
						predData.pdUpdateStruggleGoal = true;
						//val.aAlive = false;
					} else {
						AV::DamageAV(asActor, RE::ActorValue::kHealth, VoreSettings::acid_damage * delta + 1);
					}
				} else if (val.pyDigestion == VoreState::hReformation) {
					AV::DamageAV(asActor, RE::ActorValue::kHealth, -VoreSettings::acid_damage * delta);
				}
				// handle crime and aggro ????????????????????????

				// handle struggle
				if (val.pyStruggle == VoreState::sStruggling) {
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

				} else if (val.pyStruggle == VoreState::sExhausted && AV::GetPercentageAV(asActor, RE::ActorValue::kStamina) >= 0.9) {
					val.pyStruggle = VoreState::sStruggling;
				}
			}
			needUIUpdate = true;
		}
		//updates top preds
		for (auto const& [key, val] : VoreData::Data) {
			if (!val.pred && val.pdUpdateGoal) {
				UpdateSliderGoals(key);
			}
			if (!val.pred && val.pdUpdateStruggleGoal) {
				UpdateStruggleGoals(key);
			}
		}
		UI::VoreMenu::NeedUpdate = true;
	}

	void SetupTimers()
	{
		Timer digUpdateS(1, VoreSettings::slow_update, UpdateSlow);
		Timer digUpdatef(2, VoreSettings::fast_update, UpdateFast);
		Timer belUpdatef(3, (double)VoreSettings::belly_fast_update, UpdateBelly);
		Time::SetTimer(digUpdateS);
		Time::SetTimer(digUpdatef);
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
}
