#include "headers/vutils.h"
#include "headers/nutils.h"
#include "headers/settings.h"

namespace Vore::Log
{
	void PrintVoreDataSingle(RE::FormID voreNpc, VoreDataEntry& entry)
	{
		flog::info("\n");
		flog::info("Entry {}, Name {}, Type {}, Is Player {}, Alive {}, Size {}",
			std::format("{:x}", voreNpc),
			Name::GetName(voreNpc), (int)entry.aCharType,
			entry.aIsPlayer, entry.aAlive,
			entry.aSize);
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
		for (uint8_t i = 0; i < 4; i++) {
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
	//static RE::NiPoint3 GetBoxBounds(RE::TESObjectREFR* target)
	//{
	//	if (!target) {
	//		flog::warn("Can't find bound box for no target");
	//		return RE::NiPoint3(1.0f, 1.0f, 1.0f);
	//	}
	//	if (!target->Is3DLoaded()) {
	//		flog::warn("No 3d for {}", target->GetDisplayFullName());
	//		return RE::NiPoint3(1.0f, 1.0f, 1.0f); // default actor box
	//		//RE::NiPoint3(22.0f, 14.0f, 64.0f);
	//	}
	//	auto model = target->Get3D1(false);
	//	if (model) {
	//		auto bbx = model->GetExtraData("BBX");
	//		/*auto col = model->GetCollisionObject(); 
	//		if (col) {
	//			auto rgb = col->GetRigidBody();
	//			RE::hkReferencedObject* hkp_rigidbody_ref = rgb->referencedObject.get();
	//			RE::hkpRigidBody* hkp_rigidbody = skyrim_cast<RE::hkpRigidBody*>(hkp_rigidbody_ref);
	//			auto shape = hkp_rigidbody->GetShape();
	//			flog::info("col shape {}", (int)shape->type);
	//			return RE::NiPoint3(1.0f, 1.0f, 1.0f);
	//		}*/
	//		if (bbx) {
	//			return static_cast<RE::BSBound*>(bbx)->extents;
	//		}
	//	}
	//	flog::warn("Can't find bound box for {}", target->GetDisplayFullName());
	//	return RE::NiPoint3(1.0f, 1.0f, 1.0f);
	//}

	float GetObjectSize(RE::TESObjectREFR* target)
	{
		// possible preys - actors, furniture, plants, dropped items
		// if you can't target smt, you shouldn't eat it
		// test random skulls?
		// actor - use height?
		if (!target->Is3DLoaded()) {
			return 1.0f;
		}
		auto model = target->Get3D1(false);
		if (!model) {
			return 1.0f;
		}
		float totalSize = 0.0f;

		RE::BSVisit::TraverseScenegraphCollision(model, [&](RE::bhkNiCollisionObject* a_col) -> RE::BSVisit::BSVisitControl {
			if (auto hkpBody = a_col->body ? static_cast<RE::hkpRigidBody*>(a_col->body->referencedObject.get()) : nullptr) {
				
				const RE::hkpShape* shape = hkpBody->GetShape();  //mass += hkpBody->motion.GetMass();
				if (shape->type == RE::hkpShapeType::kCapsule) {
					const RE::hkpCapsuleShape* capsuleShape = static_cast<const RE::hkpCapsuleShape*>(shape);
					float length = capsuleShape->vertexA.GetDistance3(capsuleShape->vertexB);
					/*union
					{
						__m128 v;
						float a[4];
					} converter{};
					converter.v = capsuleShape->vertexA.quad;
					flog::info("\n\nVERTA {:.2f} {:.2f} {:.2f} {:.2f}", converter.a[0], converter.a[1], converter.a[2], converter.a[3]);
					converter.v = capsuleShape->vertexB.quad;
					flog::info("VERTB {:.2f} {:.2f} {:.2f} {:.2f}", converter.a[0], converter.a[1], converter.a[2], converter.a[3]);
					flog::info("Length {}", length);
					flog::info("Radius {}", capsuleShape->radius);
					*/
					totalSize += 3.14159f * capsuleShape->radius * capsuleShape->radius * (4.0f / 3.0f * capsuleShape->radius + length);
				}
			}
			return RE::BSVisit::BSVisitControl::kContinue;
		});
		return totalSize / 0.1538f * 100.0f;
		
		/*RE::NiPoint3 bounds = target->GetBoundMax() - target->GetBoundMin();
		float volume = std::pow(bounds.x * bounds.y * bounds.z, 0.5f);
		flog::trace("{} bbx {:.2f} {:.2f} {:.2f}", target->GetDisplayFullName(), bounds.x, bounds.y, bounds.z);
		return volume;*/
	}
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

	void UpdateBelly(const double& delta)
	{
		for (auto& [key, val] : VoreData::Data) {
			if (val.aCharType != RE::FormType::ActorCharacter || !val.pdUpdateSlider && !val.pdUpdateStruggleSlider) {
				continue;
			}
			std::map<const char*, float> slidervalues = {};
			//

			// if we need to update main sliders
			if (val.pdUpdateSlider) {
				// if we need to update again
				val.pdUpdateSlider = false;

				std::set<uint8_t> slidersToChange = {};
				// G1 is group 1, aka all belly sliders
				bool changedG1 = false;
				float totalG1 = 0.0f;
				float volumeG1 = 0.0f;

				auto& voresliders = val.aSex == RE::SEX::kFemale ? VoreSettings::sliders_bodypart_female : val.aSex == RE::SEX::kMale ? VoreSettings::sliders_bodypart_male :
				                                                                                                                        VoreSettings::sliders_bodypart_creature;
				//change
				for (uint8_t i = 0; i < LocusSliders::NUMOFSLIDERS; i++) {
					if (val.pdGoalStep[i] == 0.0f) {
						continue;
					}
					float diff = val.pdGoal[i] - val.pdSliders[i];
					float step = (float)delta * val.pdGoalStep[i];


					if (diff > 0) {
						if (diff <= step) {
							val.pdSliders[i] += diff;
							val.pdGoalStep[i] = 0.0f;
						} else {
							val.pdSliders[i] += step;
							val.pdUpdateSlider = true;
						}
						slidersToChange.insert(i);
						if (i < 4) {
							changedG1 = true;
						}
					} else if (diff < 0) {
						if (-diff <= step) {
							val.pdSliders[i] += diff;
							val.pdGoalStep[i] = 0.0f;
						} else {
							val.pdSliders[i] += -step;
							val.pdUpdateSlider = true;
						}
						slidersToChange.insert(i);
						if (i < 4) {
							changedG1 = true;
						}
					} else {
						val.pdGoalStep[i] = 0.0f;
						continue;
					}
				}

				if (changedG1) {
					for (uint8_t i = 0; i < 4; i++) {
						if (val.pdSliders[i] > 0) {
							totalG1 += val.pdSliders[i];
							slidersToChange.insert(i);
						}
					}
					if (totalG1 > 0) {
						volumeG1 = std::pow(totalG1 / VoreSettings::slider_one, VoreSettings::slider_pow) * VoreSettings::slider_one;
					}
				}

				for (const uint8_t& i : slidersToChange) {
					//move sliders towards the goal
					//convert volume to slider value
					float sliderValue = (i < 4 && totalG1 > 0) ? val.pdSliders[i] * volumeG1 / totalG1 :
					                                             std::pow(val.pdSliders[i] / VoreSettings::slider_one, VoreSettings::slider_pow) * VoreSettings::slider_one;
					//update sliders
					for (auto& [name, one, max] : voresliders[i]) {
						float finalval = sliderValue * one / VoreSettings::slider_one;
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
			if (val.pdUpdateStruggleSlider) {
				val.pdUpdateStruggleSlider = false;

				auto& ssliders = VoreSettings::struggle_sliders;

				for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
					//skip sliders that are not defined by user

					for (int j = 0; j < ssliders[i].size(); j++) {
						//the id of this slider in predData
						int sliderId = i * Locus::NUMOFLOCI + j;

						if (val.pdStruggleGoalStep[sliderId] == 0.0f) {
							continue;
						}

						float diff = val.pdStruggleGoal[sliderId] - val.pdStruggleSliders[sliderId];
						float step = (float)delta * val.pdStruggleGoalStep[sliderId];
						if (diff > 0) {
							if (diff <= step) {
								val.pdStruggleSliders[sliderId] += diff;
								val.pdStruggleGoalStep[sliderId] = 0.0f;
							} else {
								val.pdStruggleSliders[sliderId] += step;
								val.pdUpdateStruggleSlider = true;
							}

						} else if (diff < 0) {
							if (-diff <= step) {
								val.pdStruggleSliders[sliderId] += diff;
								val.pdStruggleGoalStep[sliderId] = 0.0f;
							} else {
								val.pdStruggleSliders[sliderId] += -step;
								val.pdUpdateStruggleSlider = true;
							}
						} else {
							val.pdStruggleGoalStep[sliderId] = 0.0f;
							continue;
						}

						float sliderValue = std::pow(val.pdStruggleSliders[sliderId] / VoreSettings::slider_one, VoreSettings::slider_pow) * VoreSettings::slider_one;

						auto& [name, one, max] = ssliders[i][j];
						float finalval = sliderValue * one / VoreSettings::slider_one;
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
			if (val.get()->Is3DLoaded()) {

				//commit all sliders to skee
				for (auto& [sname, svalue] : slidervalues) {
					//flog::trace("Setting slider: char {}, slider {}, value {}", Name::GetName(val.get()), sname, svalue);
					VoreGlobals::body_morphs->SetMorph(val.get(), sname, VoreGlobals::MORPH_KEY, svalue);
				}
				VoreGlobals::body_morphs->UpdateModelWeight(val.get());
			}
		}
	}
}