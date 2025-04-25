#include "headers/vutils.h"
#include "headers/nutils.h"
#include "headers/settings.h"
#include "headers/staticforms.h"

namespace Vore::Log
{
	static void PrintVoreDataSingle(RE::FormID voreNpc, VoreDataEntry& entry)
	{
		std::string s{};
		s.reserve(500);
		s += std::format("\n\nEntry {:x}, Name {}, Type {}, Is Player {}, Alive {}, Size {:.2f}",
			voreNpc,
			Name::GetName(voreNpc), (int)entry.aIsChar,
			entry.aIsPlayer, entry.aAlive,
			entry.aSize);
		if (entry.aIsChar) {
			RE::Actor* entryActor = entry.get()->As<RE::Actor>();
			s += std::format("\nStats: H {:.2f}/{:.2f}, S {:.2f}/{:.2f}, M {:.2f}/{:.2f}",
				AV::GetAV(entryActor, RE::ActorValue::kHealth), AV::GetMaxAV(entryActor, RE::ActorValue::kHealth),
				AV::GetAV(entryActor, RE::ActorValue::kStamina), AV::GetMaxAV(entryActor, RE::ActorValue::kStamina),
				AV::GetAV(entryActor, RE::ActorValue::kMagicka), AV::GetMaxAV(entryActor, RE::ActorValue::kMagicka));
		}

		s += std::format("\nPred: {}", (entry.pred ? Name::GetName(entry.pred) : "None"));
		s += std::format("\nPred Stats: Fat {:.2f}, Growth {:.2f}, Size {:.2f}, Update Goal {}, Update Slider {}, Burden {:.2f}",
			entry.pdFat, entry.pdFatgrowth,
			entry.pdSizegrowth,
			entry.pdUpdateGoal, entry.pdUpdateSlider, entry.pdFullBurden);

		s += std::format("\nPrey Stats: Locus {}, ElimLocus {}, Digestion {}, Struggle {} {} {}, Movement {}; Digestion P {:.2f}, Swallow P {:.2f}, Locus P {:.2f}",
			(uint8_t)entry.pyLocus, (uint8_t)entry.pyElimLocus,
			(uint8_t)entry.pyDigestion, entry.pyStruggling, entry.pyConsentEndo, entry.pyConsentLethal,
			(uint8_t)entry.pyLocusMovement, entry.pyDigestProgress,
			entry.pySwallowProcess, entry.pyLocusProcess);
		s += "\nPrey: ";
		for (auto& el : entry.prey) {
			s += Name::GetName(el);
			s += " ";
		}

		s += "\nLocus digestion: ";
		for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
			s += std::format("{}", (uint8_t)entry.pdLoci[i]);
			s += " ";
		}

		s += "\nLocus indigestion: ";
		for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
			s += std::format("{:.2f}", entry.pdIndigestion[i]);
			s += " ";
		}

		s += "\nLocus growth: ";
		for (uint8_t i = 0; i < 4; i++) {
			s += std::format("{:.2f}", entry.pdGrowthLocus[i]);
			s += " ";
		}

		s += "\nLocus sliders: ";
		for (uint8_t i = 0; i < LocusSliders::NUMOFSLIDERS; i++) {
			s += std::format("{:.2f}", entry.pdSliders[i]);
			s += " ";
		}
		s += std::format("\nXP Pred {:.2f} Prey {:.2f}\n\n", entry.pdXP, entry.pyXP);
		flog::info("{}", s);
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
	float GetObjectSize(RE::TESObjectREFR* target)
	{

		if (!target->Is3DLoaded()) {
			return 0.0f;
		}
		auto model = target->Get3D1(false);
		if (!model) {
			return 0.0f;
		}
		float totalSize = 0.0f;
		bool useBounds = false;

		bool isChar = target->GetFormType() == RE::FormType::ActorCharacter;

		RE::BSVisit::TraverseScenegraphCollision(model, [&](RE::bhkNiCollisionObject* a_col) -> RE::BSVisit::BSVisitControl {
			if (auto hkpBody = a_col->body ? static_cast<RE::hkpRigidBody*>(a_col->body->referencedObject.get()) : nullptr) {
				const RE::hkpShape* shape = hkpBody->GetShape();  //mass += hkpBody->motion.GetMass();
				if (isChar && shape->type != RE::hkpShapeType::kCapsule) {
					return RE::BSVisit::BSVisitControl::kContinue;
				}
				switch (shape->type) {
				case (RE::hkpShapeType::kCapsule):
					{
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
						break;
					}
				case (RE::hkpShapeType::kBox):
					{
						const RE::hkpBoxShape* cShape = static_cast<const RE::hkpBoxShape*>(shape);
						//flog::trace("Shape: box");
						union
						{
							__m128 v;
							float a[4];
						} converter{};
						converter.v = cShape->halfExtents.quad;
						totalSize += converter.a[0] * converter.a[1] * converter.a[2] * 8.0f;
						break;
					}
				case (RE::hkpShapeType::kConvexVertices):
					{
						//const RE::hkpConvexShape* cShape = static_cast<const RE::hkpConvexShape*>(shape);
						//flog::trace("Shape: convex");
						useBounds = true;
						//idk how to do this properly, so I'll leave it at this
						//totalSize += 4.0f / 3.0f * 3.14159f * std::pow(cShape->radius, 3.0f);
						/*RE::hkAabb aabb{};
						model->AsBhkRigidBody()->GetAabbWorldspace(aabb);
						auto cords = aabb.max - aabb.min;
						union
						{
							__m128 v;
							float a[4];
						} converter{};
						converter.v = cords.quad;
						flog::info("VERT {:.2f} {:.2f} {:.2f} {:.2f}", converter.a[0], converter.a[1], converter.a[2], converter.a[3]);
						totalSize += converter.a[0] * converter.a[1] * converter.a[2];*/
						break;
					}
				case (RE::hkpShapeType::kMOPP):
					{
						//const RE::hkpMoppBvTreeShape* cShape = static_cast<const RE::hkpMoppBvTreeShape*>(shape);
						//flog::trace("Shape: MOPP");
						useBounds = true;
						/*RE::hkTransform trans{};
						RE::hkAabb aabb{};
						cShape->;
						auto cords = aabb.max - aabb.min;
						union
						{
							__m128 v;
							float a[4];
						} converter{};
						converter.v = cords.quad;
						flog::info("VERT {:.2f} {:.2f} {:.2f} {:.2f}", converter.a[0], converter.a[1], converter.a[2], converter.a[3]);
						totalSize += converter.a[0] * converter.a[1] * converter.a[2];*/
						break;
					}
				case (RE::hkpShapeType::kList):
					{
						const RE::hkpListShape* cShape = static_cast<const RE::hkpListShape*>(shape);
						//flog::trace("Shape: list");
						union
						{
							__m128 v;
							float a[4];
						} converter{};
						converter.v = cShape->aabbHalfExtents.quad;
						totalSize += converter.a[0] * converter.a[1] * converter.a[2] * 8.0f;
						break;
					}
				case (RE::hkpShapeType::kSphere):
					{
						const RE::hkpSphereShape* cShape = static_cast<const RE::hkpSphereShape*>(shape);
						totalSize += 4.0f / 3.0f * 3.14159f * std::pow(cShape->radius, 3.0f);
						//flog::trace("Shape: sphere");
						break;
					}
				default:
					flog::error("Found an unknown shape type: {}", (int)shape->type);
					break;
				}
			}
			return RE::BSVisit::BSVisitControl::kContinue;
		});
		if (useBounds) {
			RE::NiPoint3 bounds = target->GetBoundMax() - target->GetBoundMin();
			//flog::trace("Using bound {:.4f} {:.4f} {:.4f}", bounds.x, bounds.y, bounds.z);
			//bounds / 70 = shape measurements
			totalSize = bounds.x * bounds.y * bounds.z / 343000.0f;
		}
		totalSize = totalSize / 0.1538f * 100.0f;

		if (totalSize > VoreSettings::size_softcap) {
			totalSize = std::pow(totalSize / VoreSettings::size_softcap, VoreSettings::size_softcap_power) * VoreSettings::size_softcap;
		}

		return totalSize;
	}

	double GetItemSize(RE::TESBoundObject* target)
	{
		RE::NiPoint3 bounds = {
			(float)(target->boundData.boundMax.x - target->boundData.boundMin.x),
			(float)(target->boundData.boundMax.y - target->boundData.boundMin.y),
			(float)(target->boundData.boundMax.z - target->boundData.boundMin.z),
		};
		// this number converts one type of coordinates to another (bounds to havok)
		float totalSize = bounds.x * bounds.y * bounds.z / 343000.0f;
		// this adjusts size so 1 person with 1.0 height is 100 in size
		totalSize = totalSize / 0.1538f * 100.0f;

		if (totalSize > VoreSettings::size_softcap) {
			totalSize = std::pow(totalSize / VoreSettings::size_softcap, VoreSettings::size_softcap_power) * VoreSettings::size_softcap;
		}
		return (double)totalSize;
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


	void UpdateBelly(const double& delta)
	{
		for (auto& [key, val] : VoreData::Data) {
			if (val.Belly() && val.get()->Is3DLoaded()) {
				(val.*(val.Belly()))(delta);
			}
			
		}
	}

	float GetModelScale(RE::TESObjectREFR* target)
	{
		if (!target->Is3DLoaded()) {
			return 0.0f;
		}
		auto model = target->Get3D1(false);
		if (!model) {
			return 0.0f;
		}
		return model->local.scale;
	}
	void SetModelScale(RE::TESObjectREFR* target, float scale)
	{
		auto model = target->Get3D(false);
		if (model) {
			model->local.scale = scale;
			RE::NiUpdateData ctx;
			model->UpdateWorldData(&ctx);
		}

		auto first_model = target->Get3D(true);
		if (first_model) {
			first_model->local.scale = scale;
			RE::NiUpdateData ctx;
			model->UpdateWorldData(&ctx);
		}
	}
	void UnequipAll(RE::Actor* target)
	{
		// code from GTS Plugin
		static const std::vector<std::string_view> KeywordBlackList = {
			"SOS_Genitals",  //Fix Slot 52 Genitals while still keeping the ability to unequip slot 52 underwear
			"ArmorJewelry",
			"VendorItemJewelry"
			"ClothingRing",
			"ClothingNecklace",
			"SexLabNoStrip",  //This is the keyword 3BA uses for the SMP addons?, it doesnt even originate from SL.
			"GTSDontStrip"
		};
		static const std::vector<RE::BGSBipedObjectForm::BipedObjectSlot> ValidSlots = {
			RE::BGSBipedObjectForm::BipedObjectSlot::kHead,  // 30
			// RE::BGSBipedObjectForm::BipedObjectSlot::kHair,					// 31
			RE::BGSBipedObjectForm::BipedObjectSlot::kBody,      // 32
			RE::BGSBipedObjectForm::BipedObjectSlot::kHands,     // 33
			RE::BGSBipedObjectForm::BipedObjectSlot::kForearms,  // 34
			// RE::BGSBipedObjectForm::BipedObjectSlot::kAmulet,					// 35
			// RE::BGSBipedObjectForm::BipedObjectSlot::kRing,					// 36
			RE::BGSBipedObjectForm::BipedObjectSlot::kFeet,    // 37
			RE::BGSBipedObjectForm::BipedObjectSlot::kCalves,  // 38
			RE::BGSBipedObjectForm::BipedObjectSlot::kShield,  // 39
			// RE::BGSBipedObjectForm::BipedObjectSlot::kTail,					// 40
			// RE::BGSBipedObjectForm::BipedObjectSlot::kLongHair,				// 41
			RE::BGSBipedObjectForm::BipedObjectSlot::kCirclet,           // 42
			//RE::BGSBipedObjectForm::BipedObjectSlot::kEars,              // 43
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModMouth,          // 44
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModNeck,           // 45
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModChestPrimary,   // 46
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModBack,           // 47
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModMisc1,          // 48
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModPelvisPrimary,  // 49
			// RE::BGSBipedObjectForm::BipedObjectSlot::kDecapitateHead,			// 50
			// RE::BGSBipedObjectForm::BipedObjectSlot::kDecapitate,				// 51
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModPelvisSecondary,  // 52
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModLegRight,         // 53
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModLegLeft,          // 54
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModFaceJewelry,      // 55
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModChestSecondary,   // 56
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModShoulder,         // 57
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModArmLeft,          // 58
			//RE::BGSBipedObjectForm::BipedObjectSlot::kModArmRight,         // 59
																		   // BGSBipedObjectForm::BipedObjectSlot::kModMisc2,				// 60
																		   // BGSBipedObjectForm::BipedObjectSlot::kFX01,					// 61
		};
		if (!target->IsHumanoid()) {
			return;
		}
		
		std::vector<RE::TESObjectARMO*> ArmorList{};
		for (auto Slot : ValidSlots) {
			RE::TESObjectARMO* Armor = target->GetWornArmor(Slot);
			// If armor is null skip
			if (!Armor) {
				continue;
			}
			bool allowed = true;
			for (const auto& BKwd : KeywordBlackList) {
				if (Armor->HasKeywordString(BKwd)) {
					allowed = false;
					break;  //If blacklisted keyword is found skip
				}
			}
			if (allowed) {
				ArmorList.push_back(Armor);
			}
		}
		for (auto& armor : ArmorList) {
			if (armor) {
				RE::ActorEquipManager* manager = RE::ActorEquipManager::GetSingleton();
				manager->UnequipObject(target, armor, nullptr, 1, nullptr, true, false, false);
			}
		}
	}
	bool CalcWgEnabled(RE::Actor* target)
	{
		if (VoreSettings::wg_allowed) {
			bool sexAllowed = false;

			if (VoreSettings::wg_creature && !target->IsHumanoid()) {
				sexAllowed = true;
			} else if (VoreSettings::wg_female && target->GetActorBase()->GetSex() == RE::SEX::kFemale) {
				sexAllowed = true;
			} else if (VoreSettings::wg_male && target->GetActorBase()->GetSex() == RE::SEX::kMale) {
				sexAllowed = true;
			}
			if (sexAllowed) {
				if (VoreSettings::wg_player && target->IsPlayerRef()) {
					return true;
				} else if (VoreSettings::wg_followers && (target->IsPlayerTeammate() || target->IsInFaction(StaticForms::potential_follower))) {
					return true;
				} else if (VoreSettings::wg_unique && target->GetActorBase()->IsUnique()) {
					return true;
				} else if (VoreSettings::wg_other) {
					return true;
				}
			}
		}
		return false;
	}
}