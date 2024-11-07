#include "headers/vutils.h"
#include "headers/nutils.h"
#include "headers/settings.h"

namespace Vore::Log
{
	void PrintVoreDataSingle(RE::FormID voreNpc, VoreDataEntry& entry)
	{
		flog::info("\n");
		flog::info("Entry {}, Name {}, Type {}, Is Player {}, Alive {}, Size {}, Weight {}",
			std::format("{:x}", voreNpc),
			Name::GetName(voreNpc), (int)entry.aCharType,
			entry.aIsPlayer, entry.aAlive,
			entry.aSize, entry.aWeight);
		if (entry.aCharType == RE::FormType::ActorCharacter) {
			RE::Actor* entryActor = entry.get()->As<RE::Actor>();
			flog::info("Stats: H {}/{}, S {}/{}, M {}/{}",
				AV::GetAV(entryActor, RE::ActorValue::kHealth), AV::GetMaxAV(entryActor, RE::ActorValue::kHealth),
				AV::GetAV(entryActor, RE::ActorValue::kStamina), AV::GetMaxAV(entryActor, RE::ActorValue::kStamina),
				AV::GetAV(entryActor, RE::ActorValue::kMagicka), AV::GetMaxAV(entryActor, RE::ActorValue::kMagicka));
		}

		flog::info("Pred: {}", (entry.pred ? Name::GetName(entry.pred) : "None"));
		flog::info("Pred Stats: Fat {}, Growth {}, Size {}, Prey Count {}, Update Goal {}, Update Slider {}, Burden {}",
			entry.pdFat, entry.pdFatgrowth,
			entry.pdSizegrowth, entry.prey.size(),
			entry.pdUpdateGoal, entry.pdUpdateSlider, entry.pdFullBurden);

		flog::info("Prey Stats: Locus {}, ElimLocus {}, Digestion {}, Struggle {}, Movement {}; Digestion P {}, Swallow P {}, Locus P {}",
			(uint8_t)entry.pyLocus, (uint8_t)entry.pyElimLocus,
			(uint8_t)entry.pyDigestion, (uint8_t)entry.pyStruggle,
			(uint8_t)entry.pyLocusMovement, entry.pyDigestProgress,
			entry.pySwallowProcess, entry.pyLocusProcess);
		std::string pr = "";
		for (auto& el : entry.prey) {
			pr += Name::GetName(el);
			pr += "; ";
		}
		flog::info("Prey: {}", pr);

		pr = "";
		for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
			pr += std::to_string(entry.pdLoci[i]);
			pr += " ";
		}
		flog::info("Locus digestion: {}", pr);

		pr = "";
		for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
			pr += std::to_string(entry.pdIndigestion[i]);
			pr += " ";
		}
		flog::info("Locus indigestion: {}", pr);

		pr = "";
		for (uint8_t i = 0; i < (LocusSliders::uFatLow - LocusSliders::uFatBelly); i++) {
			pr += std::to_string(entry.pdGrowthLocus[i]);
			pr += " ";
		}
		flog::info("Locus growth: {}", pr);

		pr = "";
		for (uint8_t i = 0; i < LocusSliders::NUMOFSLIDERS; i++) {
			pr += std::to_string(entry.pdSliders[i]);
			pr += " ";
		}
		flog::info("Locus sliders: {}", pr);
		flog::info("\n\n");
	}
	void PrintVoreData(RE::FormID voreNpc)
	{
		if (voreNpc && !VoreData::IsValid(voreNpc)) {
			voreNpc = 0;
		}
		flog::info("\n\nPrinting Vore Data");
		if (voreNpc) {
			PrintVoreDataSingle(voreNpc, VoreData::Data[voreNpc]);
		} else {
			for (auto& el : VoreData::Data) {
				PrintVoreDataSingle(el.first, el.second);
			}
		}
	}
}
namespace Vore
{

	std::vector<RE::FormID> FilterPrey(RE::FormID pred, Locus locus, bool noneIsAny)
	{
		std::vector<RE::FormID> preys = {};
		for (auto& el : VoreData::Data[pred].prey) {
			if (VoreData::IsPrey(el) && (VoreData::Data[el].pyLocus == locus || locus == Locus::lNone && noneIsAny)) {
				preys.push_back(el);
			}
		}
		return preys;
	}

	float randfloat(float min, float max)
	{
		static bool first = true;
		if (first) {
			srand((unsigned int)time(NULL));
			first = false;
		}
		if (min > max) {
			flog::warn("trying to generate a random number where min > max");
			return 0.0f;
		} else if (min == max) {
			return min;
		}
		return min + (float)rand() / ((float)RAND_MAX / (max - min));
	}
	void UpdateBelly()
	{
		static float& delta = VoreSettings::belly_fast_update;
		for (auto& [key, val] : VoreData::Data) {
			if (val.aCharType != RE::FormType::ActorCharacter || !val.pdUpdateSlider && !val.pdUpdateStruggleSlider) {
				continue;
			}
			std::unordered_map<const char*, float> slidervalues;

			// if we need to update main sliders
			if (val.pdUpdateSlider) {
				// if we need to update again
				val.pdUpdateSlider = false;

				//get sliders for correct sex
				//get sliders for correct sex
				//get sliders for correct sex
				//get sliders for correct sex

				auto& voresliders = val.aSex == RE::SEX::kFemale ? VoreSettings::sliders_bodypart_female : val.aSex == RE::SEX::kMale ? VoreSettings::sliders_bodypart_male :
				                                                                                                              VoreSettings::sliders_bodypart_creature;

				for (uint8_t i = 0; i < LocusSliders::NUMOFSLIDERS; i++) {
					//move sliders towards the goal
					float diff = val.pdGoal[i] - val.pdSliders[i];
					if (diff > 0) {
						val.pdSliders[i] += std::min(diff, delta * VoreSettings::slider_maxstep);
					} else if (diff < 0) {
						val.pdSliders[i] += std::max(diff, -delta * VoreSettings::slider_maxstep);
					} else {
						//skip this set of sliders if it wasn't changed
						continue;
					}
					if (val.pdSliders[i] != val.pdGoal[i]) {
						val.pdUpdateSlider = true;
					}
					//convert volume to slider value
					float sliderValue = std::pow(val.pdSliders[i] / VoreSettings::slider_100, 1.0f / VoreSettings::slider_pow_divider) * VoreSettings::slider_100;
					//update sliders
					for (auto& [name, one, max] : voresliders[i]) {
						float finalval = sliderValue * one / 100.0f;
						// final val is more that max and they have the same sign
						if (std::abs(finalval) > std::abs(max) && finalval * max > 0.0f) {
							finalval = max;
						}
						//flog::warn("{}, {}", name, finalval);
						if (slidervalues.contains(name)) {
							slidervalues[name] += finalval;
						} else {
							slidervalues[name] = finalval;
						}
					}
				}
			}
			//update struggle sliders
			if (val.pdUpdateStruggleSlider) {
				val.pdUpdateStruggleSlider = false;

				auto& ssliders = VoreSettings::struggle_sliders;

				for (uint8_t i = 0; i < Locus::NUMOFLOCI * struggle_sliders_per_locus; i++) {
					//slider does not exist in VoreSettings::struggle_sliders (it's not set by user)
					if (ssliders[i / struggle_sliders_per_locus].size() <= i % struggle_sliders_per_locus) {
						continue;
					}
					//move sliders towards the goal
					float diff = val.pdStruggleGoal[i] - val.pdStruggleSliders[i];
					if (diff > 0) {
						val.pdStruggleSliders[i] += std::min(diff, delta * VoreSettings::slider_struggle_maxstep);
					} else if (diff < 0) {
						val.pdStruggleSliders[i] += std::max(diff, -delta * VoreSettings::slider_struggle_maxstep);
					} else {
						//skip this set of sliders if it wasn't changed
						continue;
					}
					if (val.pdStruggleSliders[i] != val.pdStruggleGoal[i]) {
						val.pdUpdateStruggleSlider = true;
					}
					//convert volume to slider value
					float sliderValue = std::pow(val.pdStruggleSliders[i] / VoreSettings::slider_100, 1.0f / VoreSettings::slider_pow_divider) * VoreSettings::slider_100;

					//get the slider (we know it exists because we checked previously)
					auto& [name, one, max] = ssliders[i / struggle_sliders_per_locus][i % struggle_sliders_per_locus];
					float finalval = sliderValue * one / 100.0f;
					if (std::abs(finalval) > std::abs(max) && finalval * max > 0) {
						finalval = max;
					}
					if (slidervalues.contains(name)) {
						slidervalues[name] += finalval;
					} else {
						slidervalues[name] = finalval;
					}
				}
			}
			//commit all sliders to skee
			for (auto& [sname, svalue] : slidervalues) {
				//flog::trace("Setting slider: char {}, slider {}, value {}", Name::GetName(val.get()), sname, svalue);
				VoreGlobals::body_morphs->SetMorph(val.get(), sname, VoreGlobals::MORPH_KEY, svalue);
			}
			VoreGlobals::body_morphs->UpdateModelWeight(val.get());
		}
	}
}