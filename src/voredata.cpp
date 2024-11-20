#include "headers/voredata.h"
#include "headers/nutils.h"
#include "headers/papyrusUtil.h"
#include "headers/settings.h"
#include "headers/ui.h"
#include "headers/voremain.h"
#include "headers/vutils.h"

namespace Vore
{

	void VoreDataEntry::HandlePreyDeathImmidiate()
	{
		aAlive = false;
		pyDigestProgress = 0.0;
		pyElimLocus = pyLocus;
		pyLocusMovement = mStill;
		CalcSlow();
	}

	
		
	void VoreDataEntry::UpdatePredScale()
	{
		if (!get()->Is3DLoaded()) {
			return;
		}
		if (!aScaleDefault) {
			aScaleDefault = GetModelScale(get());
			if (!aScaleDefault || pdSizegrowth <= 0) {
				return;
			}
		} 
		float sizeConv = (float)std::pow(pdSizegrowth, VoreSettings::slider_pow) / VoreGlobals::slider_one;
		// get final scale
		sizeConv += aScaleDefault;
		if (!(sizeConv)) {
			return;
		}
		SetModelScale(get(), sizeConv);
		aSizeScale = std::pow(aScaleDefault / sizeConv, 1 / VoreSettings::slider_pow);
		aSize = aSizeDefault * aSizeScale;
		//flog::info("SIZE SCALE {}", this->aSizeScale);

	}


	void VoreDataEntry::HandleDamage(const double& delta, RE::Actor* asActor, VoreDataEntry& predData)
	{
		if (!predData.PredU) {
			predData.PredU = &VoreDataEntry::PredSlow;
		}
		if (delta > 10 && pyStruggle != sStill) {
			//can the prey struggle out while he lives
			//this is not entirely accurate because it doesn't account for the struggles of other prey,
			//and acid damage increase
			//but it also doesn't account stamina drain and regen, so it's fine for the time being

			predData.pdAcid[pyLocus] = 100;
			double ttl = AV::GetAV(asActor, RE::ActorValue::kHealth) / VoreSettings::acid_damage;
			if (ttl <= delta && predData.pdIndigestion[pyLocus] + VoreSettings::struggle_amount * ttl > 100) {
				Core::Regurgitate(predData.get()->As<RE::Actor>(), get()->GetFormID(), Core::RegType::rAll);
				return;
			}
		}
		predData.pdAcid[pyLocus] = std::min(predData.pdAcid[pyLocus] + VoreSettings::acid_gain * delta, 100.0);
		double acidMulti = predData.pdAcid[pyLocus] / 100.0;
		if (AV::GetAV(asActor, RE::ActorValue::kHealth) - VoreSettings::acid_damage * delta <= 0) {
			// LATER implement entrapment for essential/protected instead of regurgitation NPCs

			if (asActor->IsEssential()) {
				if (VoreSettings::digest_essential) {
					//asActor->boolFlags = (unsigned int)(asActor->boolFlags) & ~(unsigned int)RE::Actor::BOOL_FLAGS::kEssential;
					asActor->GetActorBase()->actorData.actorBaseFlags.reset(RE::ACTOR_BASE_DATA::Flag::kEssential);
					AV::DamageAV(asActor, RE::ActorValue::kHealth, AV::GetAV(asActor, RE::ActorValue::kHealth));
					//this is necessary for long deltas to work
					HandlePreyDeathImmidiate();
				} else {
					Regurgitate(predData.get()->As<RE::Actor>(), get()->GetFormID(), Core::RegType::rAll);
				}
			} else if (asActor->IsProtected()) {
				if (VoreSettings::digest_protected) {
					asActor->GetActorBase()->actorData.actorBaseFlags.reset(RE::ACTOR_BASE_DATA::Flag::kProtected);
					AV::DamageAV(asActor, RE::ActorValue::kHealth, AV::GetAV(asActor, RE::ActorValue::kHealth));
					//this is necessary for long deltas to work
					HandlePreyDeathImmidiate();
				} else {
					Regurgitate(predData.get()->As<RE::Actor>(), get()->GetFormID(), Core::RegType::rAll);
				}
			} else {
				AV::DamageAV(asActor, RE::ActorValue::kHealth, AV::GetAV(asActor, RE::ActorValue::kHealth));
				//asActor->KillImmediate();
				HandlePreyDeathImmidiate();
			}

			predData.pdUpdateStruggleGoal = true;
			return;
			//
		} else {
			AV::DamageAV(asActor, RE::ActorValue::kHealth, VoreSettings::acid_damage * delta * acidMulti);
		}
	}

	double VoreDataEntry::GetStomachSpace(uint64_t /*locus*/)
	{
		return 0;
	}

	RE::TESObjectREFR* VoreDataEntry::get() const
	{
		return this->me.get().get();
	}

	void VoreDataEntry::Belly(const double& delta)
	{
		std::map<const char*, float> slidervalues = {};
		//
		float scaleScale = aSizeScale ? aSizeScale : 1.0f;

		// if we need to update main sliders
		if (pdUpdateSlider) {
			// if we need to update again
			pdUpdateSlider = false;

			std::set<uint8_t> slidersToChange = {};
			// G1 is group 1, aka all belly sliders
			bool changedG1 = false;
			float totalG1 = 0.0f;
			float volumeG1 = 0.0f;

			auto& voresliders = aSex == RE::SEX::kFemale ? VoreSettings::sliders_bodypart_female : aSex == RE::SEX::kMale ? VoreSettings::sliders_bodypart_male :
			                                                                                                                        VoreSettings::sliders_bodypart_creature;
			//change
			for (uint8_t i = 0; i < LocusSliders::NUMOFSLIDERS; i++) {
				if (pdGoalStep[i] == 0.0f) {
					continue;
				}
				float diff = pdGoal[i] - pdSliders[i];
				float step = (float)delta * pdGoalStep[i];

				if (diff > 0) {
					if (diff <= step) {
						pdSliders[i] += diff;
						pdGoalStep[i] = 0.0f;
					} else {
						pdSliders[i] += step;
						pdUpdateSlider = true;
					}
					slidersToChange.insert(i);
					if (i < 4) {
						changedG1 = true;
					}
				} else if (diff < 0) {
					if (-diff <= step) {
						pdSliders[i] += diff;
						pdGoalStep[i] = 0.0f;
					} else {
						pdSliders[i] += -step;
						pdUpdateSlider = true;
					}
					slidersToChange.insert(i);
					if (i < 4) {
						changedG1 = true;
					}
				} else {
					pdGoalStep[i] = 0.0f;
					continue;
				}
			}

			if (changedG1) {
				for (uint8_t i = 0; i < 4; i++) {
					if (pdSliders[i] > 0) {
						totalG1 += pdSliders[i];
						slidersToChange.insert(i);
					}
				}
				if (totalG1 > 0) {
					volumeG1 = std::pow(totalG1 / VoreGlobals::slider_one, VoreSettings::slider_pow) * VoreGlobals::slider_one;
				}
			}

			for (const uint8_t& i : slidersToChange) {
				//move sliders towards the goal
				//convert volume to slider value
				float sliderValue = (i < 4 && totalG1 > 0) ? pdSliders[i] * volumeG1 / totalG1 :
				                                             std::pow(pdSliders[i] / VoreGlobals::slider_one, VoreSettings::slider_pow) * VoreGlobals::slider_one;
				//update sliders
				for (auto& [name, one, max] : voresliders[i]) {
					float finalval = sliderValue * scaleScale * one / VoreGlobals::slider_one;
					// final val is more that max and they have the same sign
					if (std::abs(finalval) > std::abs(max) && finalval * max > 0.0f) {
						finalval = max;
					}
					//flog::warn("{}, {}", name, finalval);
					if (slidervalues.contains(name.c_str())) {
						slidervalues[name.c_str()] += finalval;
					} else {
						slidervalues[name.c_str()] = finalval;
					}
				}
			}
		}
		//update struggle sliders
		if (pdUpdateStruggleSlider) {
			pdUpdateStruggleSlider = false;

			auto& ssliders = VoreSettings::struggle_sliders;

			for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
				//skip sliders that are not defined by user

				for (int j = 0; j < ssliders[i].size(); j++) {
					//the id of this slider in predData
					int sliderId = i * Locus::NUMOFLOCI + j;

					if (pdStruggleGoalStep[sliderId] == 0.0f) {
						continue;
					}

					float diff = pdStruggleGoal[sliderId] - pdStruggleSliders[sliderId];
					float step = (float)delta * pdStruggleGoalStep[sliderId];
					if (diff > 0) {
						if (diff <= step) {
							pdStruggleSliders[sliderId] += diff;
							pdStruggleGoalStep[sliderId] = 0.0f;
						} else {
							pdStruggleSliders[sliderId] += step;
							pdUpdateStruggleSlider = true;
						}

					} else if (diff < 0) {
						if (-diff <= step) {
							pdStruggleSliders[sliderId] += diff;
							pdStruggleGoalStep[sliderId] = 0.0f;
						} else {
							pdStruggleSliders[sliderId] += -step;
							pdUpdateStruggleSlider = true;
						}
					} else {
						pdStruggleGoalStep[sliderId] = 0.0f;
						continue;
					}

					float sliderValue = std::pow(pdStruggleSliders[sliderId] / VoreGlobals::slider_one, VoreSettings::slider_pow) * VoreGlobals::slider_one;

					auto& [name, one, max] = ssliders[i][j];
					float finalval = sliderValue * scaleScale * one / VoreGlobals::slider_one;
					if (std::abs(finalval) > std::abs(max) && finalval * max > 0) {
						finalval = max;
					}
					if (slidervalues.contains(name.c_str())) {
						slidervalues[name.c_str()] += finalval;
					} else {
						slidervalues[name.c_str()] = finalval;
					}
				}
			}
		}
		//commit all sliders to skee
		for (auto& [sname, svalue] : slidervalues) {
			//flog::trace("Setting slider: char {}, slider {}, value {}", Name::GetName(val.get()), sname, svalue);
			VoreGlobals::body_morphs->SetMorph(get(), sname, VoreGlobals::MORPH_KEY, svalue);
		}
		if (!slidervalues.empty()) {
			VoreGlobals::body_morphs->UpdateModelWeight(get());
		}
		if (!(pdUpdateSlider || pdUpdateStruggleSlider)) {
			BellyU = nullptr;
		}
	}

	void VoreDataEntry::SlowF(const double& delta)
	{
		VoreDataEntry& predData = VoreData::Data[pred];
		if (pyLocusMovement == VoreState::mIncrease) {
			pyLocusProcess += VoreSettings::locus_process_speed * delta;
		} else if (pyLocusMovement == VoreState::mDecrease) {
			pyLocusProcess -= VoreSettings::locus_process_speed * delta;
		}
		if (pyLocusMovement != VoreState::mStill) {
			predData.pdUpdateGoal = true;
		}
		if (pyLocusProcess >= 100) {
			pyLocusProcess = 100;
			pyLocusMovement = VoreState::mStill;
			CalcSlow();
		} else if (pyLocusProcess <= 0) {
			pyLocusProcess = 0;
			pyLocusMovement = VoreState::mStill;
			CalcSlow();
		}
	}

	
	static void FinishDigestion([[maybe_unused]] const RE::FormID& prey, VoreDataEntry* preyData)
	{
		//play some sound
		//(possibly) turn into bones?
		flog::info("{} was fully digested", Name::GetName(preyData->get()));
		//RE::Actor* preyA = preyData.get()->As<RE::Actor>();
		//preyA->SetCollision
	}

	void VoreDataEntry::SlowD(const double& delta)
	{
		double digestBase = VoreSettings::digestion_amount_base * delta;
		VoreDataEntry& predData = VoreData::Data[pred];
		//aWeight can increase / decrease digestion time
		//calculate speed -> (100 - val.pyDigestProgress) / VoreSettings::digestion_amount_base * 100 / val.aSize
		//get actual digestion time
		// multiply it by speed
		double digestMod = 1 / std::max(std::pow(aSize / VoreGlobals::slider_one, 0.5), 0.6);
		double toDigest = (100 - pyDigestProgress) / digestMod;
		const double& newBase = digestBase > toDigest ? toDigest : digestBase;
		pyDigestProgress += newBase * digestMod;
		if (pyDigestProgress >= 100) {
			pyDigestProgress = 100;
			FinishDigestion(get()->GetFormID(), this);
			CalcSlow();
		}
		if (pyLocus == Locus::lBowel) {
			pyLocusProcess = 100 - pyDigestProgress;
		}
		if (pyLocus == lStomach && pyDigestProgress > 30) {
			Core::MoveToLocus(pred, get()->GetFormID(), Locus::lBowel);
			/*RE::TESObjectREFR* stomachDeadCell = RE::TESForm::LookupByEditorID<RE::TESObjectREFR>("BellyDMarker");
					val.get()->MoveTo(stomachDeadCell);*/
		}

		// weight gain

		// locus increase
		// fat
		// fat growth
		// height (size) increase; (for me) check gts for info
		for (uint8_t i = 0; i < 4; i++) {
			predData.pdGrowthLocus[i] += newBase * VoreSettings::voretypes_partgain[pyElimLocus][i] * VoreSettings::wg_locusgrowth;
		}

		predData.pdFat += newBase * VoreSettings::wg_fattemp;
		predData.pdFatgrowth += newBase * VoreSettings::wg_fatlong;
		predData.pdSizegrowth += newBase * VoreSettings::wg_sizegrowth;
		predData.pdUpdateGoal = true;
		if (!predData.PredU) {
			predData.PredU = &VoreDataEntry::PredSlow;
		}
	}

	void VoreDataEntry::Struggle(const double& delta, RE::Actor* asActor, VoreDataEntry& predData)
	{
		if (pyStruggle == VoreState::sStruggling) {
			if (AV::GetAV(asActor, RE::ActorValue::kStamina) > 0) {
				//damage prey's stamina and pred's struggle bar for this locus
				AV::DamageAV(asActor, RE::ActorValue::kStamina, VoreSettings::struggle_stamina * delta);
				predData.pdIndigestion[pyLocus] += VoreSettings::struggle_amount * delta;

				if (predData.pdIndigestion[pyLocus] >= 100) {
					Core::RegurgitateAll(predData.get()->As<RE::Actor>(), pyLocus);
				}
			} else {
				pyStruggle = VoreState::sExhausted;
			}
			//pred is top pred
			if (!predData.pred) {
				predData.pdUpdateStruggleGoal = true;
			}
			if (!predData.PredU) {
				predData.PredU = &VoreDataEntry::PredSlow;
			}

		} else if (pyStruggle == VoreState::sExhausted) {
			//restore stamina
			AV::DamageAV(asActor, RE::ActorValue::kStamina, -AV::GetAV(asActor, RE::ActorValue::kStaminaRate) * AV::GetAV(asActor, RE::ActorValue::kStaminaRateMult) / 100.0 * delta);
			if (AV::GetPercentageAV(asActor, RE::ActorValue::kStamina) >= 0.9) {
				pyStruggle = VoreState::sStruggling;
			}
		}
	}

	void VoreDataEntry::FastLethalW(const double& delta)
	{
		RE::Actor* asActor = get()->As<RE::Actor>();
		VoreDataEntry& predData = VoreData::Data[pred];
		HandleDamage(delta, asActor, predData);
	}

	void VoreDataEntry::FastLethalU(const double& delta)
	{
		RE::Actor* asActor = get()->As<RE::Actor>();
		VoreDataEntry& predData = VoreData::Data[pred];
		HandleDamage(delta, asActor, predData);
		Struggle(delta, asActor, predData);
	}

	void VoreDataEntry::FastHealW(const double& delta)
	{
		RE::Actor* asActor = get()->As<RE::Actor>();
		AV::DamageAV(asActor, RE::ActorValue::kHealth, -VoreSettings::acid_damage * delta);
	}

	void VoreDataEntry::FastHealU(const double& delta)
	{
		RE::Actor* asActor = get()->As<RE::Actor>();
		VoreDataEntry& predData = VoreData::Data[pred];
		AV::DamageAV(asActor, RE::ActorValue::kHealth, -VoreSettings::acid_damage * delta);
		Struggle(delta, asActor, predData);
	}

	void VoreDataEntry::FastEndoU(const double& delta)
	{
		RE::Actor* asActor = get()->As<RE::Actor>();
		VoreDataEntry& predData = VoreData::Data[pred];
		Struggle(delta, asActor, predData);
	}

	void VoreDataEntry::Swallow(const double& delta)
	{
		VoreDataEntry& predData = VoreData::Data[pred];

		if (delta > 10) {
			pySwallowProcess = 100;
		}
		if (VoreSettings::swallow_auto) {
			pySwallowProcess += VoreSettings::swallow_auto_speed * 1 / std::max(std::pow(aSize / VoreGlobals::slider_one, 0.3), 0.7) * delta;
			predData.pdUpdateGoal = true;
		} else {
			pySwallowProcess -= VoreSettings::swallow_decrease_speed * delta;
			predData.pdUpdateGoal = true;
		}

		//finish swallow
		if (pySwallowProcess >= 100) {
			pySwallowProcess = 100;
			predData.pdUpdateGoal = true;

			CalcFast();
			if (FastU) {
				(this->*FastU)(delta);
			}

		}
		//regurgitate prey because they escaped
		else if (pySwallowProcess < 0) {
			pySwallowProcess = 0;
			Core::Regurgitate(predData.get()->As<RE::Actor>(), get()->GetFormID(), Core::RegType::rAll);
			return;
		}
	}

	void VoreDataEntry::PredSlow(const double& delta)
	{
		//flog::info("pred slow");
		bool doGoalUpdate = false;
		bool stopSlow = true;
		//reduce wg
		if (pdFat > 0) {
			pdFat = std::max(pdFat - VoreSettings::wg_loss_temp * delta, 0.0);
			doGoalUpdate = true;
			stopSlow = false;
		}
		if (pdFatgrowth > 0) {
			pdFatgrowth = std::max(pdFatgrowth - VoreSettings::wg_loss_long * delta, 0.0);
			doGoalUpdate = true;
			stopSlow = false;
		}
		if (pdSizegrowth > 0) {
			pdSizegrowth = std::max(pdFat - VoreSettings::wg_loss_size * delta, 0.0);
			stopSlow = false;
			// DO SIZE UPDATE
			UpdatePredScale();
		}
		for (auto& el : pdGrowthLocus) {
			if (el > 0) {
				el = std::max(el - VoreSettings::wg_loss_locus * delta, 0.0);
				doGoalUpdate = true;
				stopSlow = false;
			}
		}
		//reduce acid and indigestion
		for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
			if (pdAcid[i] > 0 && pdLoci[i] != VoreState::hLethal) {
				pdAcid[i] = std::max(pdAcid[i] - VoreSettings::acid_loss * delta, 0.0);
				stopSlow = false;
			}
			if (pdIndigestion[i] > 0) {
				pdIndigestion[i] = std::max(pdIndigestion[i] - VoreSettings::indigestion_loss * delta, 0.0);
				stopSlow = false;
			}
		}
		//only update goal for top preds
		if (doGoalUpdate && !pred) {
			pdUpdateGoal = true;
		}
		if (stopSlow) {
			PredU = nullptr;
		}
	}

	void VoreDataEntry::CalcFast()
	{
		//calculate next state
		if (!pred) {
			FastU = nullptr;
			return;
		}
		if (pySwallowProcess < 100) {
			FastU = &VoreDataEntry::Swallow;
			return;
		}
		if (aAlive) {
			if (pyDigestion == VoreState::hSafe) {
				if (pyStruggle == VoreState::sStill) {
					FastU = nullptr;
				} else {
					FastU = &VoreDataEntry::FastEndoU;
				}
			} else if (pyDigestion == VoreState::hLethal) {
				if (pyStruggle == VoreState::sStill) {
					FastU = &VoreDataEntry::FastLethalW;
				} else {
					FastU = &VoreDataEntry::FastLethalU;
				}
			} else if (pyDigestion == VoreState::hReformation) {
				if (pyStruggle == VoreState::sStill) {
					FastU = &VoreDataEntry::FastHealW;
				} else {
					FastU = &VoreDataEntry::FastHealU;
				}
			} else {
				FastU = nullptr;
			}
		} else {
			FastU = nullptr;
		}
	}

	void VoreDataEntry::CalcSlow()
	{
		if (!pred) {
			SlowU = nullptr;
			return;
		}
		if (aAlive) {
			if (pyLocus == Locus::lBowel && pyLocusMovement != VoreState::mStill) {
				SlowU = &VoreDataEntry::SlowF;
			} else {
				SlowU = nullptr;
			}
		} else if (pyDigestProgress < 100) {
			SlowU = &VoreDataEntry::SlowD;
		} else {
			SlowU = nullptr;
		}
	}

	bool VoreData::IsValid(RE::FormID character)
	{
		if (!character) {
			return false;
		}
		if (!Data.contains(character)) {
			return false;
		}
		if (!Data[character].get()) {
			flog::trace("Not a valid Reference handle! {}", Name::GetName(character));
			return false;
		}
		return true;
	}

	bool VoreData::IsPred(RE::FormID character, bool onlyActive)
	{
		if (!IsValid(character)) {
			return false;
		}
		uint8_t flag = 0;
		flag += !Data[character].prey.empty();
		if (onlyActive) {
			return flag;
		}

		flag += Data[character].pdFat > 0;
		flag += Data[character].pdFatgrowth > 0;
		flag += Data[character].pdSizegrowth > 0;

		for (auto& el : Data[character].pdGrowthLocus) {
			if (el > 0) {
				flag++;
				break;
			}
		}
		if (flag > 0) {
			return true;
		}
		return false;
	}

	bool VoreData::IsPrey(RE::FormID character)
	{
		if (!IsValid(character)) {
			return false;
		}

		auto& pred = Data[character].pred;
		if (!IsValid(pred)) {
			return false;
		}
		if (!Data[pred].prey.contains(character)) {
			flog::trace("Link between prey {} and pred {} is broken!", Name::GetName(character), Name::GetName(pred));
			return false;
		}

		if (Data[character].pyDigestion == VoreState::hNone) {
			flog::warn("No digestion type for {}!", Name::GetName(character));
			return false;
		} else if (Data[character].pyLocus >= Locus::NUMOFLOCI) {
			flog::warn("Wrong locus for {}!", Name::GetName(character));
			return false;
		}
		return true;
	}

	/// <summary>
	/// calculates the size and weight of a prey, including all the preys inside them
	/// </summary>
	/// <param name="size"></param>
	void VoreDataEntry::GetSize(double& size)
	{
		double digestionMod = std::max(1 - this->pyDigestProgress / 100.0, 0.2);
		size += this->aSize * digestionMod;
		for (const RE::FormID& p : this->prey) {
			if (VoreData::IsValid(p)) {
				VoreData::Data[p].GetSize(size);
			}
		}
	}

	RE::FormID VoreData::MakeData(RE::TESObjectREFR* target)
	{
		if (!target) {
			flog::warn("Trying to make data for null character");
			return 0;
		}
		VoreDataEntry value = {};
		RE::FormID chid = target->GetFormID();
		if (Data.contains(chid)) {
			flog::trace("Using existing entry for {}", Name::GetName(target));
		} else {
			value.aIsChar = target->GetFormType() == RE::FormType::ActorCharacter;
			if (value.aIsChar) {
				RE::Actor* asActor = target->As<RE::Actor>();

				//character->ref
				value.aIsPlayer = asActor->IsPlayerRef();
				value.aAlive = !(asActor->IsDead());
				value.aSex = asActor->GetActorBase()->GetSex();
				value.aEssential = asActor->IsEssential();
				value.aProtected = asActor->IsProtected();

				value.aScaleDefault = GetModelScale(asActor);

				VM::GetSingleton()->CreateObject2("Actor", value.meVm);
				VM::GetSingleton()->BindObject(value.meVm, GetHandle(target), false);
			} else {
				VM::GetSingleton()->CreateObject2("ObjectReference", value.meVm);
				VM::GetSingleton()->BindObject(value.meVm, GetHandle(target), false);
			}
			float mySize = GetObjectSize(target);
			// this might happen when vore happens outside of render distance?
			// idk how 3d and actor processing is connected
			// still, better have a fallback algorithm
			// size should never be 0, because we divide by it
			if (!mySize) {
				if (!value.aIsChar) {
					value.aSizeDefault = 5.0;
				} else {
					if (target->IsDragon()) {
						value.aSizeDefault = 3000.0;
					} else {
						value.aSizeDefault = 100.0;
					}
				}
			} else {
				value.aSizeDefault = (double)mySize;
			}
			value.aSize = value.aSizeDefault;

			value.me = target->GetHandle();

			for (auto& el : value.pdLoci) {
				el = VoreState::hSafe;
			}
			flog::trace("Making new vore data entry for {}", Name::GetName(target));
			Data.insert(std::make_pair(chid, value));
		}
		return chid;
	}

	void VoreData::SoftDelete(RE::FormID character, bool leaveOnlyActive)
	{
		if (!character) {
			flog::warn("Invalid character for deletion");
			return;
		} else if (!Data.contains(character)) {
			flog::trace("Character not in vore: {}", Name::GetName(character));
			return;
		}
		if (IsPred(character, leaveOnlyActive)) {
			flog::trace("{} still a pred", Name::GetName(character));
		} else if (IsPrey(character)) {
			flog::trace("{} still a prey", Name::GetName(character));
		} else {
			flog::trace("Adding {} to delete queue", Name::GetName(character));

			Data[character].pdGoal.fill(0.0f);
			Data[character].pdStruggleGoal.fill(0.0f);
			VoreGlobals::delete_queue.insert(character);
		}
	}
	void VoreData::HardDelete(RE::FormID character)
	{
		if (!character) {
			flog::warn("Character does not exist");
		} else if (!Data.contains(character)) {
			flog::trace("Character not in vore: {}", Name::GetName(character));
			return;
		} else {
			flog::trace("HARD Deleting character {}", Name::GetName(character));
			Data[character].pdGoal.fill(0.0f);
			Data[character].pdStruggleGoal.fill(0.0f);
			VoreGlobals::delete_queue.insert(character);
		}
	}

	//todo - iterate and fix every character, setup loci
	void VoreData::DataSetup()
	{
		// Fix characters
		std::vector<RE::FormID> bad = {};
		for (auto& [key, val] : Data) {
			if (!IsPred(key, false) && !IsPrey(key)) {
				bad.push_back(key);
			}
		}
		for (auto& el : bad) {
			Data.erase(el);
		}

		// Setup vore data
		for (auto& [key, val] : Data) {
			if (val.pred) {
				val.CalcFast();
				val.CalcSlow();
			}
			if (IsPred(key, false)) {
				val.PredU = &VoreDataEntry::PredSlow;
				val.UpdatePredScale();
			}
			if (val.get() && val.get()->Is3DLoaded()) {
				VoreGlobals::body_morphs->ClearBodyMorphKeys(val.get(), VoreGlobals::MORPH_KEY);
			}
		}
		UI::VoreMenu::SetMenuMode(UI::VoreMenuMode::kDefault);
		// Final
		flog::trace("Printing VoreData after loading is finished");
		Log::PrintVoreData();
	}

	inline bool static SaveFormIdFromRefr(RE::TESForm* character, SKSE::SerializationInterface* a_intfc)
	{
		RE::FormID fId = 0;
		if (!character) {
			fId = 0;
			flog::info("Form is a null reference");
			return false;
		} else {
			fId = character->GetFormID();
			flog::info("FormID: {}", fId);
			return a_intfc->WriteRecordData(&fId, sizeof(fId));
		}
	}

	bool static SaveFormId(RE::FormID character, SKSE::SerializationInterface* a_intfc)
	{
		if (!character) {
			flog::info("Form is 0!");
		} else {
			flog::info("Name: {}, FormID: {}", Name::GetName(character), character);
		}
		return a_intfc->WriteRecordData(&character, sizeof(character));
	}

	void VoreData::OnSave(SKSE::SerializationInterface* a_intfc)
	{
		// To write data open a record with a given name. The name must be unique within your mod, and has a version number
		// assigned (in this case 0). You can increase the version number if making breaking format change from a previous
		// version of your mod, so that the load handler can know which version of the format is being used.
		if (!a_intfc->OpenRecord('VORE', 0)) {
			flog::critical("Unable to open record to write cosave data.");
			return;
		}

		// successfully written
		bool s = true;

		flog::info("Starting save procedure");

		//cosave version
		s = s && a_intfc->WriteRecordData(&VORE_VERSION, sizeof(VORE_VERSION));

		s = s && a_intfc->WriteRecordData(&PlayerPrefs::voreLoc, sizeof(PlayerPrefs::voreLoc));
		s = s && a_intfc->WriteRecordData(&PlayerPrefs::regLoc, sizeof(PlayerPrefs::regLoc));
		s = s && a_intfc->WriteRecordData(&PlayerPrefs::voreType, sizeof(PlayerPrefs::voreType));

		//size of Data
		size_t size = Data.size();
		flog::info("Vore Data entries, size: {}", size);
		s = s && a_intfc->WriteRecordData(&size, sizeof(size));

		for (auto& [fid, vde] : Data) {
			flog::trace("\n\n");

			//save entity itself
			flog::info("Saving Entry");
			s = s && SaveFormId(fid, a_intfc);

			//save it's pred
			flog::info("Saving entry's Pred");
			s = s && SaveFormId(vde.pred, a_intfc);

			//save list of prey
			size = vde.prey.size();
			flog::info("Saving List of Prey, size: {}", size);
			s = s && a_intfc->WriteRecordData(&size, sizeof(size));
			for (auto& prey : vde.prey) {
				flog::info("Saving Prey");
				s = s && SaveFormId(prey, a_intfc);
			}

			//save list of loci
			size = vde.pdLoci.size();
			flog::info("Saving List of loci, size: {} (should be 6)", size);
			s = s && a_intfc->WriteRecordData(&size, sizeof(size));
			for (int8_t i = 0; i < Locus::NUMOFLOCI; i++) {
				uint8_t locus = vde.pdLoci[i];
				flog::info("Locus {}: {}", i, locus);
				s = s && a_intfc->WriteRecordData(&locus, sizeof(locus));
			}
			//save list of loci acid levels
			size = vde.pdAcid.size();
			flog::info("Saving List of loci acid levels, size: {} (should be 6)", size);
			s = s && a_intfc->WriteRecordData(&size, sizeof(size));
			for (int8_t i = 0; i < Locus::NUMOFLOCI; i++) {
				double locus = vde.pdAcid[i];
				flog::info("Locus {}: {}", i, locus);
				s = s && a_intfc->WriteRecordData(&locus, sizeof(locus));
			}
			//save list of loci struggle
			size = vde.pdIndigestion.size();
			flog::info("Saving List of loci struggle process, size: {} (should be 6)", size);
			s = s && a_intfc->WriteRecordData(&size, sizeof(size));
			for (int8_t i = 0; i < Locus::NUMOFLOCI; i++) {
				double locus = vde.pdIndigestion[i];
				flog::info("Locus {}: {}", i, locus);
				s = s && a_intfc->WriteRecordData(&locus, sizeof(locus));
			}
			// save body part growth
			size = vde.pdGrowthLocus.size();
			flog::info("Saving List of growt per body part, size: {} (should be 4)", size, (uint8_t)(4));
			s = s && a_intfc->WriteRecordData(&size, sizeof(size));
			for (int8_t i = 0; i < 4; i++) {
				double locus = vde.pdGrowthLocus[i];
				flog::info("Body Growth {}: {}", i, locus);
				s = s && a_intfc->WriteRecordData(&locus, sizeof(locus));
			}

			// save size, important
			s = s && a_intfc->WriteRecordData(&vde.aSizeDefault, sizeof(vde.aSizeDefault));

			//universal stats, not saved
			flog::info("Char type: {}, player {}, alive {}, size {}", (int)vde.aIsChar, vde.aIsPlayer, vde.aAlive, vde.aSize);

			//save pred stats
			flog::info("Pred");
			flog::info("Fat: {}, Fat Growth: {}, Size Growth: {}", vde.pdFat, vde.pdFatgrowth, vde.pdSizegrowth);
			s = s && a_intfc->WriteRecordData(&vde.pdFat, sizeof(vde.pdFat));
			s = s && a_intfc->WriteRecordData(&vde.pdFatgrowth, sizeof(vde.pdFatgrowth));
			s = s && a_intfc->WriteRecordData(&vde.pdSizegrowth, sizeof(vde.pdSizegrowth));

			//save prey stats
			flog::info("Prey");
			flog::info("Locus: {}, ElimLocus: {}, Digestion: {}, Struggle: {}, Movement: {}",
				(uint8_t)vde.pyLocus, (uint8_t)vde.pyElimLocus, (uint8_t)vde.pyDigestion, (uint8_t)vde.pyStruggle, (uint8_t)vde.pyLocusMovement);
			s = s && a_intfc->WriteRecordData(&vde.pyLocus, sizeof(vde.pyLocus));
			s = s && a_intfc->WriteRecordData(&vde.pyElimLocus, sizeof(vde.pyElimLocus));
			s = s && a_intfc->WriteRecordData(&vde.pyDigestion, sizeof(vde.pyDigestion));
			s = s && a_intfc->WriteRecordData(&vde.pyStruggle, sizeof(vde.pyStruggle));
			s = s && a_intfc->WriteRecordData(&vde.pyLocusMovement, sizeof(vde.pyLocusMovement));
			flog::info("Digestion Process: {}, Swallow Process: {}, Locus Process: {}", vde.pyDigestProgress, vde.pySwallowProcess, vde.pyLocusProcess);
			s = s && a_intfc->WriteRecordData(&vde.pyDigestProgress, sizeof(vde.pyDigestProgress));
			s = s && a_intfc->WriteRecordData(&vde.pySwallowProcess, sizeof(vde.pySwallowProcess));
			s = s && a_intfc->WriteRecordData(&vde.pyLocusProcess, sizeof(vde.pyLocusProcess));

			flog::trace("\n{}\n", s);
		}

		flog::warn("Saved successful? {}", s);
	}

	inline static std::string& ReadString(SKSE::SerializationInterface* serde, std::string& outStr)
	{
		int sz;
		serde->ReadRecordData(sz);
		outStr.resize_and_overwrite(sz, [&](char* buf, std::size_t /* bufSize*/) {
			serde->ReadRecordData(buf, sz);
			return sz;
		});
		return outStr;
	}

	[[nodiscard]] inline static RE::FormID ReadFormID(SKSE::SerializationInterface* a_intfc)
	{
		RE::FormID formId;
		a_intfc->ReadRecordData(formId);
		if (!formId) {
			flog::warn("Form ID is empty", formId);
			return 0;
		}
		// When reading back a form ID from a save, that form ID may no longer be valid because the user's
		// load order may have changed. During the load call, it is possible to use
		// <code>ResolveFormID</code> to attempt to find the new form ID, based on the user's current load
		// order and the load order that was recorded in the save file.
		RE::FormID newFormId = 0;
		if (!a_intfc->ResolveFormID(formId, newFormId)) {
			flog::warn("Form ID {:X} could not be found after loading the save.", formId);
			newFormId = 0;
		}
		return newFormId;
	}

	[[nodiscard]] inline static RE::TESForm* GetFormPtr(SKSE::SerializationInterface* a_intfc)
	{
		RE::TESForm* newObject = nullptr;
		RE::FormID objectForm = ReadFormID(a_intfc);
		if (objectForm != 0) {
			newObject = RE::TESForm::LookupByID(objectForm);
			if (!newObject) {
				flog::warn("Failed to lookup form");
			}
		}
		return newObject;
	}

	[[nodiscard]] inline static RE::TESObjectREFR* GetObjectPtr(SKSE::SerializationInterface* a_intfc)
	{
		return skyrim_cast<RE::TESObjectREFR*>(GetFormPtr(a_intfc));
	}

	void VoreData::OnLoad(SKSE::SerializationInterface* a_intfc)
	{
		std::uint32_t type;
		std::uint32_t size1;
		std::uint32_t version;

		// To load records from a cosave, use <code>GetNextRecordInfo</code> to iterate from one record to the next.
		// You will be given records in the order they were written, but otherwise you do not look up a record by its name.
		// Instead check the result of each iteration to find out which record it is and handle it appropriately.
		//
		// If you make breaking changes to your data format, you can increase the version number used when writing the data
		// out and check that number here to handle previous versions.

		flog::info("Begin Loading");
		while (a_intfc->GetNextRecordInfo(type, version, size1)) {
			flog::info("Next Record");
			if (type == 'VORE') {
				flog::info("Staring loading procedure");

				//cosave version
				int V_VERSION;

				a_intfc->ReadRecordData(V_VERSION);
				flog::warn("SAVE VERSION: {}", V_VERSION);

				//read player prefs

				a_intfc->ReadRecordData(PlayerPrefs::voreLoc);
				a_intfc->ReadRecordData(PlayerPrefs::regLoc);
				a_intfc->ReadRecordData(PlayerPrefs::voreType);

				//size of Data
				size_t size;
				a_intfc->ReadRecordData(size);
				flog::info("Vore Data entries, size: {}", size);

				for (; size > 0; --size) {
					flog::trace("\n\n");

					flog::info("New Entry");

					// in case we read a broken ref, data goes here
					Vore::VoreDataEntry entryEmpty = {};

					RE::TESObjectREFR* entryPtr = GetObjectPtr(a_intfc);
					flog::info("Name {}", Name::GetName(entryPtr));
					// we make a new VoreData entry if we get a valid pointer, otherwise we read the data but never use it
					if (entryPtr) {
						MakeData(entryPtr);
						flog::info("Making a new data entry");
					} else {
						flog::info("Skipping entry");
					}
					Vore::VoreDataEntry& entry = entryPtr ? Data[entryPtr->GetFormID()] : entryEmpty;

					// start reading actual vore data entry

					auto* tpred = GetObjectPtr(a_intfc);
					if (tpred) {
						entry.pred = tpred->GetFormID();
						flog::info("Pred {}", Name::GetName(tpred));
					} else {
						entry.pred = 0U;
						flog::info("No Pred");
					}

					size_t sizePrey;
					a_intfc->ReadRecordData(sizePrey);
					flog::info("List of Prey, size: {}", sizePrey);
					for (; sizePrey > 0; --sizePrey) {
						RE::TESObjectREFR* prey = nullptr;
						prey = GetObjectPtr(a_intfc);
						if (prey) {
							entry.prey.insert(prey->GetFormID());
							flog::info("Prey {}", Name::GetName(prey));
						}
					}

					size_t sizeLoci;
					a_intfc->ReadRecordData(sizeLoci);
					flog::info("List of Loci, size: {}", sizeLoci);
					for (int i = 0; i < sizeLoci; i++) {
						VoreState locus = VoreState::hNone;
						a_intfc->ReadRecordData(locus);
						entry.pdLoci[i] = locus;
						flog::info("Locus {} {}", i, (int)locus);
					}

					size_t sizeLocAcid;
					a_intfc->ReadRecordData(sizeLocAcid);
					flog::info("List of Loci acid, size: {}", sizeLocAcid);
					for (int i = 0; i < sizeLocAcid; i++) {
						double locus = 0;
						a_intfc->ReadRecordData(locus);
						entry.pdAcid[i] = locus;
						flog::info("Locus {} {}", i, locus);
					}

					size_t sizeLocStruggle;
					a_intfc->ReadRecordData(sizeLocStruggle);
					flog::info("List of Loci Struggle process, size: {}", sizeLocStruggle);
					for (int i = 0; i < sizeLocStruggle; i++) {
						double locus = 0;
						a_intfc->ReadRecordData(locus);
						entry.pdIndigestion[i] = locus;
						flog::info("Locus {} {}", i, locus);
					}

					size_t sizeGrowth;
					a_intfc->ReadRecordData(sizeGrowth);
					flog::info("List of Growth per body part, size: {}", sizeGrowth);
					for (int i = 0; i < sizeGrowth; i++) {
						double growth = 0;
						a_intfc->ReadRecordData(growth);
						entry.pdGrowthLocus[i] = growth;
						flog::info("Growth {} {}", i, growth);
					}

					// default values that are assigned when character's 3d is not loaded
					if (entry.aSizeDefault == 5.0 || entry.aSizeDefault == 100.0 || entry.aSizeDefault == 0.0 || entry.aSizeDefault == 3000.0) {
						a_intfc->ReadRecordData(entry.aSizeDefault);
						entry.aSize = entry.aSizeDefault;
					} else {
						double oldSize = 0;
						a_intfc->ReadRecordData(oldSize);
					}

					flog::info("Char type: {}, is player: {}, alive {}, size {}", (int)entry.aIsChar, entry.aIsPlayer, entry.aAlive, entry.aSize);

					flog::info("Pred");
					a_intfc->ReadRecordData(entry.pdFat);
					a_intfc->ReadRecordData(entry.pdFatgrowth);
					a_intfc->ReadRecordData(entry.pdSizegrowth);
					flog::info("Fat: {}, Fat Growth: {}, Size Growth: {}", entry.pdFat, entry.pdFatgrowth, entry.pdSizegrowth);

					flog::info("Prey");
					a_intfc->ReadRecordData(entry.pyLocus);
					a_intfc->ReadRecordData(entry.pyElimLocus);
					a_intfc->ReadRecordData(entry.pyDigestion);
					a_intfc->ReadRecordData(entry.pyStruggle);
					a_intfc->ReadRecordData(entry.pyLocusMovement);
					flog::info("Locus: {}, ElimLocus: {}, Digestion: {}, Struggle: {}, Movement: {}",
						(uint8_t)entry.pyLocus, (uint8_t)entry.pyElimLocus, (uint8_t)entry.pyDigestion, (uint8_t)entry.pyStruggle, (uint8_t)entry.pyLocusMovement);
					a_intfc->ReadRecordData(entry.pyDigestProgress);
					a_intfc->ReadRecordData(entry.pySwallowProcess);
					a_intfc->ReadRecordData(entry.pyLocusProcess);
					flog::info("Digestion Process: {}, Swallow Process: {}, Locus Process: {}", entry.pyDigestProgress, entry.pySwallowProcess, entry.pyLocusProcess);

					if (entryPtr) {
						flog::info("Finished loading {}", Name::GetName(entryPtr));
					} else {
						flog::warn("Skipping this character");
					}

					flog::trace("\n\n");
				}
			} else {
				flog::warn("Unknown record type in cosave.");
			}
		}

		flog::info("Loaded cosave, setting up VoreData");
		DataSetup();
		//flog::info("arr {}", arr);
	}
	void VoreData::OnRevert(SKSE::SerializationInterface* /*a_intfc*/)
	{
		PlayerPrefs::clear();
		VoreGlobals::delete_queue.clear();
		UI::VoreMenu::_menuMode = UI::kNone;
		UI::VoreMenu::_setModeAfterShow = UI::kNone;
		UI::VoreMenu::_infoTarget = {};
		flog::info("reverting, clearing data");
		Data.clear();
	}
}
