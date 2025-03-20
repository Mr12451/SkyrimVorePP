#include "headers/voredataentry.h"
#include "headers/voredata.h"
#include "headers/dialogue.h"
#include "headers/nutils.h"
#include "headers/settings.h"
#include "headers/sounds.h"
#include "headers/voremain.h"
#include "headers/vutils.h"

#include <chrono>
//#include <future>
#include <thread>

namespace Vore
{
	void VoreDataEntry::DigestLive()
	{
		if (pred && aAlive && aIsChar) {
			RE::Actor* asActor = get()->As<RE::Actor>();
			VoreDataEntry* predData = VoreData::IsValidGet(pred);
			if (!predData) {
				return;
			}
			if (aIsPlayer) {
				AV::DamageAV(asActor, RE::ActorValue::kHealth, AV::GetAV(asActor, RE::ActorValue::kHealth) - 5.0f);
				HandlePreyDeathImmidiate();
			} else if (asActor->IsEssential()) {
				if (VoreSettings::digest_essential) {
					//asActor->boolFlags = (unsigned int)(asActor->boolFlags) & ~(unsigned int)RE::Actor::BOOL_FLAGS::kEssential;
					asActor->GetActorBase()->actorData.actorBaseFlags.reset(RE::ACTOR_BASE_DATA::Flag::kEssential);
					AV::DamageAV(asActor, RE::ActorValue::kHealth, AV::GetAV(asActor, RE::ActorValue::kHealth) + 1.0f);
					//this is necessary for long deltas to work
					HandlePreyDeathImmidiate();
				} else {
					Regurgitate(predData->get()->As<RE::Actor>(), get()->GetFormID(), Core::RegType::rAll);
				}
			} else if (asActor->IsProtected()) {
				if (VoreSettings::digest_protected) {
					asActor->GetActorBase()->actorData.actorBaseFlags.reset(RE::ACTOR_BASE_DATA::Flag::kProtected);
					AV::DamageAV(asActor, RE::ActorValue::kHealth, AV::GetAV(asActor, RE::ActorValue::kHealth) + 1.0f);
					//this is necessary for long deltas to work
					HandlePreyDeathImmidiate();
				} else {
					Regurgitate(predData->get()->As<RE::Actor>(), get()->GetFormID(), Core::RegType::rAll);
				}
			} else {
				AV::DamageAV(asActor, RE::ActorValue::kHealth, AV::GetAV(asActor, RE::ActorValue::kHealth) + 1.0f);
				//asActor->KillImmediate();
				HandlePreyDeathImmidiate();
			}
		} else if (aAlive) {
			HandlePreyDeathImmidiate();
		}
	}
	void VoreDataEntry::HandlePreyDeathImmidiate()
	{
		aAlive = false;
		pyDigestProgress = 0.0;
		pyElimLocus = pyLocus;
		pyLocusMovement = mStill;
		pyDigestion = hLethal;
		CalcFast();
		CalcSlow();

		if (aIsChar) {
			if (VoreDataEntry* predData = VoreData::IsValidGet(pred)) {
				predData->EmoteSmile(5000);
			}
			if (aIsPlayer) {
				Dialogue::PlayerDied();
				// add hide player ui
				Core::SwitchToDigestion(pred, pyLocus, VoreState::hSafe, false);
			} else {
				// add skill equal to highest prey skill + stamina
				// nah I'll do it on digestion end
			}
		}
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
		float scaleConv = (float)std::pow(pdSizegrowth, VoreSettings::slider_pow) / VoreGlobals::slider_one;
		// get final scale
		scaleConv += aScaleDefault;
		if (!(scaleConv)) {
			return;
		}
		SetModelScale(get(), scaleConv);
		float sizeScale = std::pow(aScaleDefault / scaleConv, 1 / VoreSettings::slider_pow);
		aSize = aSizeDefault * sizeScale;
		//flog::info("SIZE SCALE {}", this->aSizeScale);
	}

	void VoreDataEntry::HandleDamage(const double& delta, RE::Actor* asActor, VoreDataEntry* predData)
	{
		//predData->SetPredUpdate(true);
		if (delta > 10 && pyStruggling) {
			//can the prey struggle out while he lives
			//this is not entirely accurate because it doesn't account for the struggles of other prey,
			//and acid damage increase
			//but it also doesn't account stamina drain and regen, so it's fine for the time being

			predData->pdAcid[pyLocus] = 100;
			double ttl = AV::GetAV(asActor, RE::ActorValue::kHealth) / VoreSettings::acid_damage;
			if (ttl <= delta && predData->pdIndigestion[pyLocus] + VoreSettings::struggle_amount * ttl > 100) {
				Core::Regurgitate(predData->get()->As<RE::Actor>(), get()->GetFormID(), Core::RegType::rAll);
				return;
			}
		}
		predData->pdAcid[pyLocus] = std::min(predData->pdAcid[pyLocus] + VoreSettings::acid_gain * delta, 100.0);
		predData->pdHasDigestion = true;
		double acidMulti = predData->pdAcid[pyLocus] / 100.0;
		if (AV::GetAV(asActor, RE::ActorValue::kHealth) - VoreSettings::acid_damage * delta <= 0) {
			// LATER implement entrapment for essential/protected instead of regurgitation NPCs
			DigestLive();

			predData->pdUpdateStruggleGoal = true;
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

	void VoreDataEntry::BellyUpdate(const double& delta)
	{
		std::map<const char*, float> slidervalues = {};
		//
		float scaleScale = VoreGlobals::slider_one / (float)aSize;

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
		VoreDataEntry* predData = VoreData::IsValidGet(pred);
		if (!predData) {
			return;
		}
		if (pyLocusMovement == VoreDataEntry::FullTour::mIncrease) {
			pyLocusProcess += VoreSettings::locus_process_speed * delta;
		} else if (pyLocusMovement == VoreDataEntry::FullTour::mDecrease) {
			pyLocusProcess -= VoreSettings::locus_process_speed * delta;
		}
		if (pyLocusMovement != VoreDataEntry::FullTour::mStill) {
			predData->pdUpdateGoal = true;
		}
		if (pyLocusProcess >= 100) {
			pyLocusProcess = 100;
			pyLocusMovement = VoreDataEntry::FullTour::mStill;
			CalcSlow();
		} else if (pyLocusProcess <= 0) {
			pyLocusProcess = 0;
			pyLocusMovement = VoreDataEntry::FullTour::mStill;
			CalcSlow();
		}
		// pred code
		if (aIsPlayer && !aDialogue) {
			VoreDataEntry* apexData = Core::GetApex(this);
			Funcs::MoveTo(get(), apexData->get());
		}
	}

	static void StatSkillGain(VoreDataEntry* preyData, VoreDataEntry* predData)
	{
		if (VoreSettings::gain_stats_base > 0) {
			static const std::array<RE::ActorValue, 3> avStats = { RE::ActorValue::kHealth, RE::ActorValue::kMagicka, RE::ActorValue::kStamina };
			RE::ActorValue maxStat = RE::ActorValue::kNone;
			double maxdiff = 0;
			for (auto& av : avStats) {
				double thisdiff = AV::GetMaxAV(preyData->get()->As<RE::Actor>(), av) - AV::GetMaxAV(predData->get()->As<RE::Actor>(), av);
				if (thisdiff > maxdiff) {
					maxStat = av;
					maxdiff = thisdiff;
				}
			}
			if (maxdiff > 0) {
				AV::ModAV(predData->get()->As<RE::Actor>(), maxStat, (double)VoreSettings::gain_stats_base);
			}
		}
		if (VoreSettings::gain_skill_base > 0) {
			static const std::array<RE::ActorValue, 18> avSkills = { RE::ActorValue::kIllusion, RE::ActorValue::kConjuration, RE::ActorValue::kDestruction, RE::ActorValue::kRestoration, RE::ActorValue::kAlteration, RE::ActorValue::kEnchanting,
				RE::ActorValue::kSmithing, RE::ActorValue::kHeavyArmor, RE::ActorValue::kBlock, RE::ActorValue::kTwoHanded, RE::ActorValue::kOneHanded, RE::ActorValue::kArchery,
				RE::ActorValue::kLightArmor, RE::ActorValue::kSneak, RE::ActorValue::kLockpicking, RE::ActorValue::kPickpocket, RE::ActorValue::kSpeech, RE::ActorValue::kAlchemy };
			RE::ActorValue maxSkill = RE::ActorValue::kNone;
			double maxdiff = 0;
			for (auto& av : avSkills) {
				double thisdiff = AV::GetMaxAV(preyData->get()->As<RE::Actor>(), av) - AV::GetMaxAV(predData->get()->As<RE::Actor>(), av);
				if (thisdiff > maxdiff) {
					maxSkill = av;
					maxdiff = thisdiff;
				}
			}
			if (maxdiff > 0) {
				AV::ModAV(predData->get()->As<RE::Actor>(), maxSkill, (double)VoreSettings::gain_skill_base);
			}
		}

		//if (AV::GetMaxAV(preyData->get()->As<RE::Actor>(), RE::ActorValue::kHealth) > AV::GetMaxAV(preyData->get()->As<RE::Actor>(), RE::ActorValue::kHealth))
	}

	static void FinishDigestionProcess(VoreDataEntry* preyData, VoreDataEntry* predData)
	{
		//play some sound
		flog::info("{} was fully digested", Name::GetName(preyData->get()));
		preyData->pyDigestProgress = 100.0;
		preyData->CalcSlow();
		preyData->CalcFast();
		if (!preyData->aIsChar) {
			return;
		}

		StatSkillGain(preyData, predData);

		predData->PlaySound(Sounds::Gurgle);
		predData->EmoteSmile(5000);
		if (VoreCharStats* preyStats = VoreData::IsValidStatGet(preyData->get()->GetFormID())) {
			if (preyStats->reformer == predData->get()->GetFormID()) {
				preyStats->reformer = 0;
				Core::StartReformation(preyData, predData);
				return;
			}
		}
		if (!predData->aIsPlayer && !preyData->aIsPlayer) {
			Core::AutoRelease(preyData, predData);
		}
	}

	void VoreDataEntry::SlowD(const double& delta)
	{
		VoreDataEntry* predData = VoreData::IsValidGet(pred);
		if (!predData) {
			return;
		}
		// digest delta is the precentage of digesting process that happens this tick
		// mods digestion time based on size. The bigger the size, the longer digestion is
		// however pow here is used to make digestion of very big prey no to absurdly long
		// and 0.6 is for small prey. Even if they are very small, they shouldn't be digested instantly because that's not how biology works
		double digestDelta = VoreSettings::digestion_amount_base * delta / std::max(std::pow(aSize / VoreGlobals::slider_one, 0.5), 0.6);
		if (aIsPlayer) {
			digestDelta *= VoreSettings::player_digest_multi;
		}
		if (digestDelta > 100.0 - pyDigestProgress) {
			digestDelta = 100.0 - pyDigestProgress;
		}

		pyDigestProgress += digestDelta;
		if (pyDigestProgress >= 100.0) {
			FinishDigestionProcess(this, predData);
		}
		if (pyLocus == Locus::lBowel) {
			pyLocusProcess = 100.0 - pyDigestProgress;
		}
		if (pyLocus == lStomach && pyDigestProgress > 30.0) {
			Core::MoveToLocus(pred, get()->GetFormID(), Locus::lBowel);
			predData->PlaySound(Sounds::Gurgle);
		}

		// weight gain
		if (predData->pdWGAllowed) {
			double wgBase = digestDelta * aSize / 10000.0;
			for (uint8_t i = 0; i < 4; i++) {
				predData->pdGrowthLocus[i] += wgBase * VoreSettings::voretypes_partgain[pyElimLocus][i] * VoreSettings::wg_locusgrowth;
			}

			predData->pdFat += wgBase * VoreSettings::wg_fattemp;
			predData->pdFatgrowth += wgBase * VoreSettings::wg_fatlong;
			flog::info("SIZE GAIN {}", VoreSettings::wg_sizegrowth);
			predData->pdSizegrowth += wgBase * VoreSettings::wg_sizegrowth;
		}
		predData->pdUpdateGoal = true;
		predData->pdHasDigestion = true;
		predData->SetPredUpdate(true);

		//player code
		if (aIsPlayer && !aDialogue) {
			VoreDataEntry* apexData = Core::GetApex(this);
			Funcs::MoveTo(get(), apexData->get(), true);
		}
	}

	static void FinishReformProcess(VoreDataEntry* preyData, VoreDataEntry* predData)
	{
		//play some sound
		flog::info("{} was reformed", Name::GetName(preyData->get()));
		if (!preyData->aIsChar) {
			return;
		}
		preyData->pyDigestProgress = 0.0;
		predData->PlaySound(Sounds::Gurgle);
		predData->EmoteSmile(5000);

		preyData->pyDigestion = predData->pdLoci[preyData->pyLocus];
		preyData->aAlive = true;
		if (preyData->aIsChar && !preyData->aIsPlayer) {
			preyData->get()->As<RE::Actor>()->Resurrect(false, false);
		}
		preyData->CalcSlow();
		preyData->CalcFast();
		if (!predData->aIsPlayer && !preyData->aIsPlayer) {
			Core::AutoRelease(preyData, predData);
		}
	}

	void VoreDataEntry::SlowR(const double& delta)
	{
		VoreDataEntry* predData = VoreData::IsValidGet(pred);
		if (!predData) {
			return;
		}

		/*double reformMod = 1 / std::max(std::pow(aSize / VoreGlobals::slider_one, 0.5), 0.6);
		double toReform = pyDigestProgress / reformMod;
		const double& newBase = reformBase > toReform ? toReform : reformBase;
		pyDigestProgress -= newBase * reformMod;*/

		double reformDelta = VoreSettings::digestion_amount_base * delta / std::max(std::pow(aSize / VoreGlobals::slider_one, 0.5), 0.6);
		if (aIsPlayer) {
			reformDelta *= VoreSettings::player_digest_multi;
		}
		if (reformDelta > pyDigestProgress) {
			reformDelta = pyDigestProgress;
		}

		pyDigestProgress -= reformDelta;
		if (pyDigestProgress <= 0.0) {
			FinishReformProcess(this, predData);
		}
		if (pyLocus == Locus::lBowel) {
			pyLocusProcess = 100.0 - pyDigestProgress;
		}

		// weight loss
		if (predData->pdWGAllowed) {
			double wlBase = reformDelta * aSize / 10000.0;
			for (uint8_t i = 0; i < 4; i++) {
				predData->pdGrowthLocus[i] -= wlBase * VoreSettings::voretypes_partgain[pyElimLocus][i] * VoreSettings::wg_locusgrowth;
				if (predData->pdGrowthLocus[i] < 0) {
					predData->pdGrowthLocus[i] = 0;
				}
			}

			predData->pdFat -= wlBase * VoreSettings::wg_fattemp;
			if (predData->pdFat < 0) {
				predData->pdFat = 0;
			}
			predData->pdFatgrowth -= wlBase * VoreSettings::wg_fatlong;
			if (predData->pdFatgrowth < 0) {
				predData->pdFatgrowth = 0;
			}
			predData->pdSizegrowth -= wlBase * VoreSettings::wg_sizegrowth;
			if (predData->pdSizegrowth < 0) {
				predData->pdSizegrowth = 0;
			}
		}
		predData->pdUpdateGoal = true;
		predData->pdHasDigestion = true;
		predData->SetPredUpdate(true);

		//player code
		if (aIsPlayer && !aDialogue) {
			VoreDataEntry* apexData = Core::GetApex(this);
			Funcs::MoveTo(get(), apexData->get(), true);
		}
	}

	void VoreDataEntry::SlowP(const double&)
	{
		VoreDataEntry* predData = VoreData::IsValidGet(pred);
		if (!predData) {
			return;
		}
		if (aIsPlayer && !aDialogue) {
			VoreDataEntry* apexData = Core::GetApex(this);
			Funcs::MoveTo(get(), apexData->get(), true);
		}
	}

	void VoreDataEntry::Struggle(const double& delta, RE::Actor* asActor, VoreDataEntry* predData)
	{
		if (pyStruggleResource > 0) {
			if (pyStruggling) {
				if (AV::GetAV(asActor, RE::ActorValue::kStamina) > 0) {
					//damage prey's stamina and pred's struggle bar for this locus
					AV::DamageAV(asActor, RE::ActorValue::kStamina, VoreSettings::struggle_stamina * delta);
					predData->pdIndigestion[pyLocus] += VoreSettings::struggle_amount * delta;

					if (predData->pdIndigestion[pyLocus] >= 100) {
						Core::RegurgitateAll(predData->get()->As<RE::Actor>(), pyLocus);
					}
				} else {
					pyStruggling = false;
				}
				//pred is top pred
				if (!predData->pred) {
					predData->pdUpdateStruggleGoal = true;
				}
				predData->SetBellyUpdate(true);
				//predData->SetPredUpdate(true);
				if (aSex != RE::SEX::kNone) {
					predData->pdStrugglePreySex = aSex;
				}

			} else {
				//restore stamina
				AV::DamageAV(asActor, RE::ActorValue::kStamina, -AV::GetAV(asActor, RE::ActorValue::kStaminaRate) * AV::GetAV(asActor, RE::ActorValue::kStaminaRateMult) / 100.0 * delta);
				if (AV::GetPercentageAV(asActor, RE::ActorValue::kStamina) >= 0.9) {
					pyStruggleResource--;
					if (pyStruggleResource > 0) {
						pyStruggling = true;
					}
				}
			}
		}
	}

	void VoreDataEntry::FastLethalW(const double& delta)
	{
		RE::Actor* asActor = get()->As<RE::Actor>();
		if (VoreDataEntry* predData = VoreData::IsValidGet(pred)) {
			HandleDamage(delta, asActor, predData);
		}
	}

	void VoreDataEntry::FastLethalU(const double& delta)
	{
		RE::Actor* asActor = get()->As<RE::Actor>();
		if (VoreDataEntry* predData = VoreData::IsValidGet(pred)) {
			HandleDamage(delta, asActor, predData);
			Struggle(delta, asActor, predData);
		}
	}

	void VoreDataEntry::FastHealW(const double& delta)
	{
		RE::Actor* asActor = get()->As<RE::Actor>();
		if (VoreDataEntry* predData = VoreData::IsValidGet(pred)) {
			//predData->SetPredUpdate(true);
			AV::DamageAV(asActor, RE::ActorValue::kHealth, -VoreSettings::acid_damage * delta);
		}
	}

	void VoreDataEntry::FastHealU(const double& delta)
	{
		RE::Actor* asActor = get()->As<RE::Actor>();
		if (VoreDataEntry* predData = VoreData::IsValidGet(pred)) {
			//predData->SetPredUpdate(true);
			AV::DamageAV(asActor, RE::ActorValue::kHealth, -VoreSettings::acid_damage * delta);
			Struggle(delta, asActor, predData);
		}
	}

	void VoreDataEntry::FastEndoU(const double& delta)
	{
		RE::Actor* asActor = get()->As<RE::Actor>();
		if (VoreDataEntry* predData = VoreData::IsValidGet(pred)) {
			Struggle(delta, asActor, predData);
		}
	}

	void VoreDataEntry::Swallow(const double& delta)
	{
		VoreDataEntry* predData = VoreData::IsValidGet(pred);
		if (!predData) {
			return;
		}

		if (delta > 10) {
			pySwallowProcess = 100;
		}
		if (VoreSettings::swallow_auto || !predData->aIsPlayer) {
			pySwallowProcess += VoreSettings::swallow_auto_speed * 1 / std::max(std::pow(aSize / VoreGlobals::slider_one, 0.3), 0.7) * delta;
			predData->PlaySwallow();
			predData->pdUpdateGoal = true;
		} else {
			pySwallowProcess -= VoreSettings::swallow_decrease_speed * delta;
			predData->pdUpdateGoal = true;
		}

		//finish swallow
		if (pySwallowProcess >= 100) {
			pySwallowProcess = 100;
			predData->pdUpdateGoal = true;

			CalcFast();
			if (FastU) {
				(this->*FastU)(delta);
			}
			//UpdateSliderGoals();
			predData->PlayStomachSounds();

		}
		//regurgitate prey because they escaped
		else if (pySwallowProcess < 0) {
			pySwallowProcess = 0;
			Core::Regurgitate(predData->get()->As<RE::Actor>(), get()->GetFormID(), Core::RegType::rAll);
			return;
		}
	}

	void VoreDataEntry::PredSlow(const double& delta)
	{
		//flog::info("pred slow");
		bool doGoalUpdate = false;
		bool stopSlow = true;
		//reduce fat
		if (pdWGAllowed) {
			RE::Actor* asActor = get()->As<RE::Actor>();
			double wlMulti = 0.0;
			if (asActor->Is3DLoaded()) {
				wlMulti = asActor->IsInCombat() + asActor->IsSprinting() + asActor->IsAttacking() + asActor->IsInMidair();
			}
			if (pdFat > 0) {
				// doing physical activities will reduce weight faster
				pdFat = std::max(pdFat - (1.0 + wlMulti) * VoreSettings::wg_loss_temp * delta, 0.0);
				doGoalUpdate = true;
				stopSlow = false;
			}
			if (pdFatgrowth > 0) {
				pdFatgrowth = std::max(pdFatgrowth - (1.0 + wlMulti / 2.0) * VoreSettings::wg_loss_long * delta, 0.0);
				doGoalUpdate = true;
				stopSlow = false;
			}
			if (pdSizegrowth > 0) {
				pdSizegrowth = std::max(pdSizegrowth - VoreSettings::wg_loss_size * delta, 0.0);
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

		if (prey.size() > 0) {
			stopSlow = false;
		}

		if (get()->Is3DLoaded() && pdFullBurden > 0.0f) {
			PlayBurpRandom();
			PlayGurgleRandom();
		}
		if (doGoalUpdate) {
			pdUpdateGoal = true;
		}
		PlayStomachSounds();
		pdHasDigestion = false;
		pdStrugglePreySex = RE::SEX::kNone;

		//only update goal for top preds
		if (stopSlow) {
			SetPredUpdate(false);
		}
	}

	void VoreDataEntry::CalcFast(bool forceStop)
	{
		//calculate next state
		if (aDialogue) {
			FastU = nullptr;
			return;
		}
		if (!pred || forceStop) {
			pyStruggling = false;
			FastU = nullptr;
			return;
		}
		if (pySwallowProcess < 100) {
			FastU = &VoreDataEntry::Swallow;
			return;
		}
		if (aAlive) {
			if (pyDigestion == VoreState::hSafe) {
				if (pyConsentEndo) {
					pyStruggling = false;
					FastU = nullptr;
				} else {
					if (pyStruggleResource > 0) {
						pyStruggling = true;
					}
					FastU = &VoreDataEntry::FastEndoU;
				}
			} else if (pyDigestion == VoreState::hLethal) {
				if (pyConsentLethal) {
					pyStruggling = false;
					FastU = &VoreDataEntry::FastLethalW;
				} else {
					if (pyStruggleResource > 0) {
						pyStruggling = true;
					}
					FastU = &VoreDataEntry::FastLethalU;
				}
			} else if (pyDigestion == VoreState::hReformation) {
				if (pyConsentEndo) {
					pyStruggling = false;
					FastU = &VoreDataEntry::FastHealW;
				} else {
					if (pyStruggleResource > 0) {
						pyStruggling = true;
					}
					FastU = &VoreDataEntry::FastHealU;
				}
			} else {
				pyStruggling = false;
				FastU = nullptr;
			}
		} else {
			pyStruggling = false;
			FastU = nullptr;
		}
	}

	void VoreDataEntry::CalcSlow(bool forceStop)
	{
		if (!pred || forceStop) {
			SlowU = nullptr;
			return;
		}
		if (aAlive) {
			if (pyLocus == Locus::lBowel && pyLocusMovement != VoreDataEntry::FullTour::mStill) {
				SlowU = &VoreDataEntry::SlowF;
			} else if (aIsPlayer) {
				SlowU = &VoreDataEntry::SlowP;
			} else {
				SlowU = nullptr;
			}
		} else if (pyDigestion == hReformation) {
			SlowU = &VoreDataEntry::SlowR;
		} else if (pyDigestProgress < 100) {
			SlowU = &VoreDataEntry::SlowD;
		} else {
			SlowU = nullptr;
		}
	}

	void VoreDataEntry::SetConsent(bool willing, bool lethal)
	{
		if (!aIsChar) {
			pyConsentLethal = true;
			pyConsentEndo = true;
			CalcFast();
		} else {
			RE::Actor* preyA = get()->As<RE::Actor>();
			VoreDataEntry* predData = VoreData::IsValidGet(pred);
			if (!predData) {
				return;
			}
			RE::Actor* predA = predData->get()->As<RE::Actor>();
			if (!preyA || !predA) {
				return;
			}
			if (lethal) {
				pyConsentLethal = willing;
				pyConsentEndo = willing || pyConsentEndo;
			} else {
				pyConsentEndo = willing;
			}
			CalcFast();
			Dialogue::SetConsent(predA, preyA, willing, lethal);
		}
	}

	void VoreDataEntry::SetBellyUpdate(bool doUpdate)
	{
		if (doUpdate) {
			BellyU = &VoreDataEntry::BellyUpdate;
		} else {
			BellyU = nullptr;
		}
	}

	void VoreDataEntry::SetPredUpdate(bool doUpdate)
	{
		if (doUpdate) {
			PredU = &VoreDataEntry::PredSlow;
		} else {
			flog::trace("Stopped pred update for {}", get()->GetDisplayFullName());
			PredU = nullptr;
			StopAllSounds();
		}
	}

	void VoreDataEntry::ClearAllUpdates()
	{
		FastU = nullptr;
		SlowU = nullptr;
		BellyU = nullptr;
		PredU = nullptr;
		StopAllSounds();
	}

	void VoreDataEntry::UpdateStruggleGoals()
	{
		//if fast update happens too slow or slider max step is too big (aka morphing happens too fast), this may break
		//float deltaMax = std::min(static_cast<float>(VoreSettings::fast_update) * VoreSettings::slider_struggle_maxstep, 100.0f);

		pdUpdateStruggleSlider = true;
		SetBellyUpdate(true);

		std::array<float, Locus::NUMOFLOCI> accum_struggle = { 0.0f };
		bool quitStruggle = true;

		for (const RE::FormID& a_prey : prey) {
			if (VoreDataEntry* preyData = VoreData::IsValidGet(a_prey)) {
				if (preyData->aAlive && preyData->pyStruggling) {
					accum_struggle[preyData->pyLocus] += static_cast<float>(preyData->aSize * AV::GetPercentageAV(preyData->get()->As<RE::Actor>(), RE::ActorValue::kStamina) / 1.5f);
					//accum_struggle[preyData.pyLocus] += (float)preyData.aSize;
				}
			}
		}

		auto& ssliders = VoreSettings::struggle_sliders;

		for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
			//skip sliders that are not defined by user

			if (accum_struggle[i] == 0.0f && pdAccumStruggle[i] == 0.0f) {
				continue;
			} else if (accum_struggle[i] == 0.0f) {
				for (int j = 0; j < ssliders[i].size(); j++) {
					//the id of this slider in predData
					int sliderId = i * Locus::NUMOFLOCI + j;
					pdStruggleGoalStep[sliderId] = pdStruggleSliders[sliderId];
					pdStruggleGoal[sliderId] = 0.0f;
				}
				continue;
			} else {
				quitStruggle = false;
				pdAccumStruggle[i] = accum_struggle[i];
			}

			for (int j = 0; j < ssliders[i].size(); j++) {
				//the id of this slider in predData
				int sliderId = i * Locus::NUMOFLOCI + j;

				// goal diff is reset by updatebelly when slider reaches it's goal
				if (pdStruggleGoalStep[sliderId] == 0.0f) {
					float maxValue = accum_struggle[i];
					if (pdStruggleGoal[sliderId] > maxValue / 2.0f) {
						pdStruggleGoal[sliderId] = Math::randfloat(0.0, maxValue / 2.0f);
					} else {
						pdStruggleGoal[sliderId] = Math::randfloat(maxValue / 2.0f, maxValue);
					}

					pdStruggleGoalStep[sliderId] = VoreSettings::slider_maxstep * std::pow(std::abs(pdStruggleGoal[sliderId] - pdStruggleSliders[sliderId]) / VoreGlobals::slider_one, 0.75f);
				}
				// i got brain damage when writing this lol
			}
		}

		if (quitStruggle) {
			pdUpdateStruggleGoal = false;
			return;
		}
	}

	void VoreDataEntry::UpdateSliderGoals()
	{
		pdUpdateGoal = false;
		pdUpdateSlider = true;
		SetBellyUpdate(true);

		pdGoal.fill(0.0f);

		pdFullBurden = 0.0f;

		//calculate locus size with switch

		for (const RE::FormID& a_prey : prey) {
			VoreDataEntry* preyData = VoreData::IsValidGet(a_prey);
			if (!preyData) {
				flog::error("Found a broken prey {}", Name::GetName(a_prey));
				continue;
			}
			//calculate prey full size
			//calculate prey full weight
			double preySize = 0.0;
			preyData->GetSize(preySize);
			pdFullBurden += (float)preySize;

			//unfortunately sliders are floats, while everything else in this mod is a double
			//doubles can be better for precision at really small wg/digestion steps, that's why I use them
			//so these casts are necessary
			switch (preyData->pyLocus) {
			// for swallowing
			case Locus::lStomach:
				pdGoal[LocusSliders::uThroat] += static_cast<float>(preySize * (1 - preyData->pySwallowProcess / 100.0));
				pdGoal[LocusSliders::uStomach] += static_cast<float>(preySize * preyData->pySwallowProcess / 100.0);
				break;
			// for full tour
			case Locus::lBowel:
				pdGoal[LocusSliders::uBowel] += static_cast<float>(preySize * (1 - preyData->pyLocusProcess / 100.0));
				pdGoal[LocusSliders::uStomach] += static_cast<float>(preySize * preyData->pyLocusProcess / 100.0);
				break;
			case Locus::lWomb:
				pdGoal[LocusSliders::uWomb] += static_cast<float>(preySize);
				break;
			case Locus::lBreastl:
				pdGoal[LocusSliders::uBreastl] += static_cast<float>(preySize);
				break;
			case Locus::lBreastr:
				pdGoal[LocusSliders::uBreastr] += static_cast<float>(preySize);
				break;
			// for swallowing cv
			case Locus::lBalls:
				pdGoal[LocusSliders::uCock] += static_cast<float>(preySize * (1 - preyData->pySwallowProcess / 100.0));
				pdGoal[LocusSliders::uBalls] += static_cast<float>(preySize * preyData->pySwallowProcess / 100.0);
				break;
			}
		}

		//update other sliders

		// locus fat
		if (pdWGAllowed) {
			pdGoal[LocusSliders::uFatBelly] = static_cast<float>(pdGrowthLocus[0]);
			pdGoal[LocusSliders::uFatAss] = static_cast<float>(pdGrowthLocus[1]);
			pdGoal[LocusSliders::uFatBreasts] = static_cast<float>(pdGrowthLocus[2]);
			pdGoal[LocusSliders::uFatCock] = static_cast<float>(pdGrowthLocus[3]);

			//fat and perma fat
			pdGoal[LocusSliders::uFatLow] = static_cast<float>((pdFat < 0) ? pdFat : 0);
			pdGoal[LocusSliders::uFatHigh] = static_cast<float>((pdFat >= 0) ? pdFat : 0);
			pdGoal[LocusSliders::uGrowthLow] = static_cast<float>((pdFatgrowth < 0) ? pdFatgrowth : 0);
			pdGoal[LocusSliders::uGrowthHigh] = static_cast<float>((pdFatgrowth >= 0) ? pdFatgrowth : 0);
		}

		for (uint8_t i = 0; i < LocusSliders::NUMOFSLIDERS; i++) {
			pdGoalStep[i] = VoreSettings::slider_maxstep * std::pow(std::abs(pdGoal[i] - pdSliders[i]) / VoreGlobals::slider_one, 0.75f);
		}
	}

	void VoreDataEntry::PlaySound(RE::BGSSoundDescriptorForm* sound, float volume) const
	{
		if (!Sounds::AllowSounds || !get()->Is3DLoaded()) {
			//flog::error("Can't play sound!");
			return;
		}
		RE::BSAudioManager* audioManager = RE::BSAudioManager::GetSingleton();
		RE::BSSoundHandle handle = RE::BSSoundHandle();
		bool success = audioManager->BuildSoundDataFromDescriptor(handle, sound->soundDescriptor);
		if (success) {
			//flog::trace("Playing sound");
			handle.SetVolume(volume);
			handle.SetObjectToFollow(get()->GetCurrent3D());
			handle.Play();
		} else {
			flog::error("Cannot build sound! {}", sound->GetFormEditorID());
		}
	}

	void VoreDataEntry::PlayRegurgitation(bool ass) const
	{
		if (ass) {
			if (aSex == RE::SEX::kFemale) {
				PlaySound(Sounds::Poop_F);
			} else if (aSex == RE::SEX::kMale) {
				PlaySound(Sounds::Poop_M);
			} else {
				PlaySound(Sounds::Poop);
			}
		} else {
			PlaySound(Sounds::Vomit);
		}
	}

	void VoreDataEntry::PlayScream(const VoreDataEntry* targetPrey) const
	{
		if (VoreSettings::play_scream) {
			if (targetPrey->aSex == RE::SEX::kFemale) {
				PlaySound(Sounds::Scream_F);
			} else if (targetPrey->aSex == RE::SEX::kMale) {
				PlaySound(Sounds::Scream_M);
			} else if (targetPrey->get()->IsAnimal()) {
				PlaySound(Sounds::Scream_A);
			} else if (targetPrey->get()->HasKeywordByEditorID("ActorTypeUndead")) {
				PlaySound(Sounds::Scream_U);
			} else {
				PlaySound(Sounds::Scream_C);
			}
		} else {
			if (targetPrey->aSex == RE::SEX::kFemale) {
				PlaySound(Sounds::Screamless_F);
			} else {
				PlaySound(Sounds::Screamless_M);
			}
		}
	}

	void VoreDataEntry::PlaySwallow()
	{
		if (!Sounds::AllowSounds || !get()->Is3DLoaded()) {
			//flog::error("Can't play sound!");
			return;
		}

		if (soundHandles.swallowHandle.IsPlaying()) {
			return;
		}

		RE::BSAudioManager* audioManager = RE::BSAudioManager::GetSingleton();
		soundHandles.swallowHandle = RE::BSSoundHandle();
		bool success = audioManager->BuildSoundDataFromDescriptor(soundHandles.swallowHandle, Sounds::Swallow->soundDescriptor);
		if (success) {
			//soundHandles.swallowHandle.SetVolume(0.1f);
			soundHandles.swallowHandle.SetObjectToFollow(get()->GetCurrent3D());
			soundHandles.swallowHandle.Play();
		} else {
			flog::error("Cannot build sound!");
		}
	}

	void VoreDataEntry::PlayBurpRandom() const
	{
		if (!Sounds::AllowSounds) {
			return;
		}

		float burpBurden = pdGoal[0] + pdGoal[1] / 3.0f;
		burpBurden *= VoreSettings::burp_rate;
		if (burpBurden <= 0) {
			return;
		}
		if (pdHasDigestion) {
			burpBurden *= 3.0f;
		}
		if (Math::randfloat(0.0f, 100.0f) <= burpBurden) {
			RE::BSFaceGenAnimationData* fData = get()->As<RE::Actor>()->GetFaceGenAnimationData();
			//flog::trace("Burp");
			if (fData && !fData->exprOverride) {
				//flog::trace("Burp1");
				PlaySound(Sounds::Burp);
				fData->SetExpressionOverride(16, 1.0f);
				//fData->expressionKeyFrame.SetValue(16, 1.0f);
				fData->exprOverride = true;
				//fData->phenomeKeyFrame.SetValue(0, 1.0f);
				//fData->phenomeKeyFrame.SetValue(1, 1.0f);
				RE::ActorHandle myHandle = get()->As<RE::Actor>()->GetHandle();

				auto t = std::thread([myHandle]() {
					std::this_thread::sleep_for(std::chrono::milliseconds{ 2000 });
					SKSE::GetTaskInterface()->AddTask([myHandle]() {
						if (myHandle) {
							myHandle.get()->GetFaceGenAnimationData()->ClearExpressionOverride();
							myHandle.get()->GetFaceGenAnimationData()->exprOverride = false;
						}
					});
				});
				t.detach();
			}
		}
	}

	void VoreDataEntry::PlayGurgleRandom() const
	{
		if (!Sounds::AllowSounds) {
			return;
		}

		float gurgleBurden = pdGoal[1] + pdGoal[0] / 3.0f;
		gurgleBurden *= VoreSettings::gurgle_rate;
		if (gurgleBurden <= 0) {
			return;
		}
		if (pdHasDigestion) {
			gurgleBurden *= 2.0f;
		}
		if (Math::randfloat(0.0f, 100.0f) <= gurgleBurden) {
			//flog::info("playing gurgle");
			PlaySound(Sounds::Gurgle);
			//flog::info("finished gurgle");
		}
	}

	void VoreDataEntry::PlayStomachSounds()
	{
		if (!Sounds::AllowSounds) {
			return;
		}
		if (get()->Is3DLoaded() && pdFullBurden > 0.0f) {
			float volume = std::min(pdHasDigestion ? pdFullBurden / 100.0f : pdFullBurden / 200.0f, 1.0f);

			//digestion sounds
			if (soundHandles.digestHandle.IsPlaying()) {
				// the sound is already playing, adjust volume based on digestion and prey size
				soundHandles.digestHandle.SetVolume(volume);
			} else {
				flog::info("starting digestion sounds");
				// start digestion sound
				RE::BSAudioManager* audioManager = RE::BSAudioManager::GetSingleton();
				if (soundHandles.digestHandle.IsPlaying()) {
					soundHandles.digestHandle.Stop();
				}
				soundHandles.digestHandle = RE::BSSoundHandle();
				bool success = audioManager->BuildSoundDataFromDescriptor(soundHandles.digestHandle, Sounds::Stomach->soundDescriptor);
				if (success) {
					soundHandles.digestHandle.SetVolume(volume);
					soundHandles.digestHandle.SetObjectToFollow(get()->GetCurrent3D());
					soundHandles.digestHandle.Play();
				} else {
					flog::error("Cannot build sound!");
				}
			}
			//struggle sounds
			if (pdStrugglePreySex != RE::SEX::kNone) {
				// need new sound
				if (!(soundHandles.struggleHandle.IsPlaying() && pdStrugglePreySex == soundHandles.preySex)) {
					soundHandles.preySex = pdStrugglePreySex;

					RE::BSAudioManager* audioManager = RE::BSAudioManager::GetSingleton();
					if (soundHandles.struggleHandle.IsPlaying()) {
						soundHandles.struggleHandle.Stop();
					}
					soundHandles.struggleHandle = RE::BSSoundHandle();

					RE::BGSSoundDescriptorForm* sound = Sounds::Struggling_CF;

					// get the correct sound
					if (aSex == RE::SEX::kFemale) {
						if (pdStrugglePreySex == RE::SEX::kFemale) {
							sound = Sounds::Struggling_FF;
						} else if (pdStrugglePreySex == RE::SEX::kMale) {
							sound = Sounds::Struggling_FM;
						}
					} else if (aSex == RE::SEX::kMale) {
						if (pdStrugglePreySex == RE::SEX::kFemale) {
							sound = Sounds::Struggling_MF;
						} else if (pdStrugglePreySex == RE::SEX::kMale) {
							sound = Sounds::Struggling_MM;
						}
					} else {
						if (pdStrugglePreySex == RE::SEX::kFemale) {
							sound = Sounds::Struggling_CF;
						} else if (pdStrugglePreySex == RE::SEX::kMale) {
							sound = Sounds::Struggling_CM;
						}
					}

					bool success = audioManager->BuildSoundDataFromDescriptor(soundHandles.struggleHandle, sound->soundDescriptor);
					if (success) {
						//soundHandles.struggleHandle.SetVolume(volume);
						soundHandles.struggleHandle.SetObjectToFollow(get()->GetCurrent3D());
						soundHandles.struggleHandle.Play();
					} else {
						flog::error("Cannot build sound!");
					}
				}
			} else if (soundHandles.struggleHandle.IsPlaying()) {
				soundHandles.struggleHandle.Stop();
			}
		} else {
			if (soundHandles.digestHandle.IsPlaying()) {
				soundHandles.digestHandle.Stop();
				soundHandles.struggleHandle.Stop();
			}
		}
	}

	void VoreDataEntry::StopAllSounds()
	{
		soundHandles.swallowHandle.FadeOutAndRelease(1000);
		soundHandles.digestHandle.FadeOutAndRelease(1000);
		soundHandles.struggleHandle.FadeOutAndRelease(1000);
	}

	void VoreDataEntry::EmoteSmile(int duration_ms) const
	{
		RE::BSFaceGenAnimationData* fData = get()->As<RE::Actor>()->GetFaceGenAnimationData();

		if (fData && !fData->exprOverride) {
			flog::info("Smiling");
			fData->SetExpressionOverride(2, 1.0f);
			//fData->expressionKeyFrame.SetValue(16, 1.0f);
			fData->exprOverride = true;
			//fData->phenomeKeyFrame.SetValue(0, 1.0f);
			//fData->phenomeKeyFrame.SetValue(1, 1.0f);
			RE::ActorHandle myHandle = get()->As<RE::Actor>()->GetHandle();

			auto t = std::thread([myHandle, duration_ms]() {
				std::this_thread::sleep_for(std::chrono::milliseconds{ duration_ms });
				SKSE::GetTaskInterface()->AddTask([myHandle]() {
					if (myHandle) {
						RE::BSFaceGenAnimationData* fgenData = myHandle.get()->GetFaceGenAnimationData();
						if (fgenData) {
							fgenData->ClearExpressionOverride();
							fgenData->exprOverride = false;
						}
					}
				});
			});
			t.detach();
		}
	}

}