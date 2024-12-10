#include "headers/nutils.h"
#include "headers/papyrusUtil.h"
#include "headers/util.h"

std::vector<RE::Actor*> Vore::Utils::FindActors()
{
	std::vector<RE::Actor*> result;

	auto process_list = RE::ProcessLists::GetSingleton();
	for (RE::ActorHandle actor_handle : process_list->highActorHandles) {
		if (!actor_handle) {
			continue;
		}
		auto actor_smartptr = actor_handle.get();
		if (!actor_smartptr) {
			continue;
		}

		RE::Actor* actor = actor_smartptr.get();
		// auto actor = actor_handle.get().get();
		if (actor && actor->Is3DLoaded()) {
			result.push_back(actor);
		}
	}
	auto player = RE::PlayerCharacter::GetSingleton();
	if (player && player->Is3DLoaded()) {
		result.push_back(player);
	}
	std::sort(result.begin(), result.end());
	return result;
}

std::vector<RE::TESObjectREFR*> Vore::Utils::GetFrontObjects(RE::Actor* source, size_t maxActors)
{
	if (!source) {
		return {};
	}
	RE::bhkCharacterController* charController = source->GetCharController();
	if (!charController) {
		flog::warn("No controller found for {}", Name::GetName(source));
		return {};
	}

	std::vector<RE::Actor*> targets = FindActors();
	RE::NiPoint3 sourcePos = source->GetPosition();

	// Sort prey by distance
	sort(targets.begin(), targets.end(),
		[sourcePos](const RE::Actor* targetA, const RE::Actor* targetB) -> bool {
			float distanceToA = (targetA->GetPosition() - sourcePos).Length();
			float distanceToB = (targetB->GetPosition() - sourcePos).Length();
			return distanceToA < distanceToB;
		});
	// Filter out actors not in front
	//auto sourceAngle = source->data.angle.z;
	//RE::NiPoint3 forwardVector{ 0.f, 1.f, 0.f };
	//::NiPoint3 actorForward = REL::RotateAngleAxis(forwardVector, -sourceAngle, { 0.f, 0.f, 1.f });

	std::vector<RE::TESObjectREFR*> results;
	size_t i = 0;
	for (auto& target : targets) {
		if (i >= maxActors) {
			break;
		}
		flog::info("Found target {}", Name::GetName(target));
		results.push_back(target);
		i++;
	}

	return results;
}

RE::TESObjectREFR* Vore::Utils::GetCrosshairObject()
{
	RE::CrosshairPickData* crosshairPick = RE::CrosshairPickData::GetSingleton();
	if (!crosshairPick) {
		flog::critical("No CrosshairPickData!!!");
		return nullptr;
	}
	if (!crosshairPick->target.get()) {
		
		return nullptr;
	}

	return crosshairPick->target.get().get();
}

	/*
	if pred == playerRef
		return prey.getRelationshipRank(pred) > 0 \
		|| prey.IsPlayerTeammate() \
		|| prey.IsPlayersLastRiddenHorse()
	elseif prey == playerRef
		return pred.getRelationshipRank(prey) > 0 \
		|| prey.IsPlayerTeammate() \
		|| pred.IsPlayersLastRiddenHorse()
	else
		return prey.getRelationshipRank(pred) > 0
	endIf
	*/

bool Vore::Utils::AreFriends(RE::Actor* a_first, RE::Actor* a_second, RE::BGSRelationship::RELATIONSHIP_LEVEL min_rel)
{
	if (a_first == a_second) {
		return true;
	}
	if (a_first->IsPlayerRef()) {
		RE::BGSRelationship* rel = RE::BGSRelationship::GetRelationship(a_second->GetActorBase(), a_first->GetActorBase());
		RE::BGSRelationship::RELATIONSHIP_LEVEL relLevel = rel->level.get();
		flog::info("REL LEVEL {}", (int)relLevel);
		return a_second->IsPlayerTeammate() || relLevel <= min_rel;
	} else if (a_second->IsPlayerRef()) {
		RE::BGSRelationship* rel = RE::BGSRelationship::GetRelationship(a_first->GetActorBase(), a_second->GetActorBase());
		RE::BGSRelationship::RELATIONSHIP_LEVEL relLevel = rel->level.get();
		flog::info("REL LEVEL {}", (int)relLevel);
		return a_first->IsPlayerTeammate() || relLevel <= min_rel;
	} else {
		RE::BGSRelationship* rel = RE::BGSRelationship::GetRelationship(a_second->GetActorBase(), a_first->GetActorBase());
		RE::BGSRelationship::RELATIONSHIP_LEVEL relLevel = rel->level.get();
		flog::info("REL LEVEL {}", (int)relLevel);
		return relLevel <= min_rel;
	}
}

namespace Vore::AV
{
	double GetMaxAV(Actor* actor, ActorValue av)
	{
		auto baseValue = actor->GetBaseActorValue(av);
		auto permMod = actor->GetActorValueModifier(ACTOR_VALUE_MODIFIERS::kPermanent, av);
		auto tempMod = actor->GetActorValueModifier(ACTOR_VALUE_MODIFIERS::kTemporary, av);
		return baseValue + permMod + tempMod;
	}
	double GetAV(Actor* actor, ActorValue av)
	{
		// actor->GetActorValue(av); returns a cached value so we calc directly from mods
		double max_av = GetMaxAV(actor, av);
		auto damageMod = actor->GetActorValueModifier(ACTOR_VALUE_MODIFIERS::kDamage, av);
		return max_av + damageMod;
	}
	void ModAV(Actor* actor, ActorValue av, double amount)
	{
		actor->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kTemporary, av, static_cast<float>(amount));
	}
	void SetAV(Actor* actor, ActorValue av, double amount)
	{
		double currentValue = GetAV(actor, av);
		double delta = amount - currentValue;
		ModAV(actor, av, delta);
	}

	void DamageAV(Actor* actor, ActorValue av, double amount)
	{
		actor->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, static_cast<float>(-amount));
	}

	double GetPercentageAV(Actor* actor, ActorValue av)
	{
		return GetAV(actor, av) / GetMaxAV(actor, av);
	}

	void SetPercentageAV(Actor* actor, ActorValue av, double target)
	{
		double currentValue = GetAV(actor, av);
		double maxValue = GetMaxAV(actor, av);
		double targetValue = target / 100 * maxValue;
		double delta = targetValue - currentValue;
		actor->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, av, static_cast<float>(delta));
	}
}

namespace Vore::Funcs
{
	void SetGhost(RE::Actor* actor, bool param)
	{
		/*if (!actor) {
			return;
		}
		using func_t = decltype(&SetGhost);
		REL::Relocation<func_t> func{ RELOCATION_ID(36287, 37276) };
		func(actor, param);*/
		CallFunctionOn(actor, "Actor", "SetGhost", param);
	}

	void SendAssaultAlarm(RE::Actor* actor)
	{
		CallFunctionOn(actor, "Actor", "SendAssaultAlarm");
	}

	//void KillActor(RE::Actor* actor)
	//{
	//}

	void StartCombat(RE::Actor* agressor, RE::Actor* victim)
	{
		// This function starts combat and adds bounty. Sadly adds 40 bounty since it's not a murder, needs other hook for murder bounty.
		typedef void (*DefStartCombat)(RE::Actor* act_1, RE::Actor* act_2, RE::Actor* act_3, RE::Actor* act_4);
		REL::Relocation<DefStartCombat> SkyrimStartCombat{ RELOCATION_ID(36430, 37425) };  // sub_1405DE870 : 36430  (SE) ; 1406050c0 : 37425 (AE)
		SkyrimStartCombat(victim, agressor, agressor, agressor);                           // Called from Attacked above at some point
	}

	///from gts plugin, DOESN'T WORK!!!!!!!!!!!!
	void ApplyDamage([[maybe_unused]] RE::Actor* giant, RE::Actor* tiny, float damage)
	{  // applies correct amount of damage and kills actors properly
		typedef void (*DefApplyDamage)(RE::Actor* a_this, float dmg, RE::Actor* aggressor, RE::HitData* maybe_hitdata, RE::TESObjectREFR* damageSrc);
		REL::Relocation<DefApplyDamage> Skyrim_ApplyDamage{ RELOCATION_ID(36345, 37335) };  // 5D6300 (SE)
		Skyrim_ApplyDamage(tiny, damage, nullptr, nullptr, nullptr);
	}

	void Attacked(RE::Actor* victim, RE::Actor* agressor)
	{
		typedef void (*DefAttacked)(RE::Actor* victim, RE::Actor* agressor);
		REL::Relocation<DefAttacked> SkyrimAttacked{ RELOCATION_ID(37672, 38626) };  // 6285A0 (SE) ; 64EE60 (AE)
		SkyrimAttacked(victim, agressor);
	}

	//void DealDamage(RE::Actor* a_this, RE::Actor* a_target, RE::Projectile* a_sourceProjectile, bool a_bLeftHand)
	//{
	//	typedef void (*DeDealDamage)(RE::Actor* a_this, RE::Actor* a_target, RE::Projectile* a_sourceProjectile, bool a_bLeftHand);
	//	REL::Relocation<DeDealDamage> Skyrim_DealDamage{ RELOCATION_ID(37673, 38627) };  // 5D6300 (SE)
	//	Skyrim_DealDamage(a_this, a_target, a_sourceProjectile, a_bLeftHand);
	//}

	void SetRestrained(RE::Actor* actor, bool param)
	{
		CallFunctionOn(actor, "Actor", "SetRestrained", param);
	}

	void SetAlpha(RE::Actor* actor, float alpha, bool fade)
	{
		CallFunctionOn(actor, "Actor", "SetAlpha", alpha, fade);
	}

	void StopCombat(RE::Actor* actor)
	{
		CallFunctionOn(actor, "Actor", "StopCombat");
	}

	void StopCombatAlarm(RE::Actor* actor)
	{
		CallFunctionOn(actor, "Actor", "StopCombatAlarm");
	}
	bool IsInDialogueWithPlayer(RE::Actor* actor)
	{
		if (actor->GetHighProcess() && actor->GetHighProcess()->talkingToPC) {
			return true;
		}
		//isInDialogue.head->data.runOnRef = actor->GetHandle();

		return false;
	}
	void ForceQuestAlias(RE::TESQuest* quest, std::string_view alias, RE::Actor* target)
	{
		if (!quest) {
			return;
		}

		uint32_t aliasId = 0;

		for (auto el : quest->aliases) {
			if (el->aliasName == alias) {

				aliasId = el->aliasID;
				flog::info("Found alias {}", el->aliasName.c_str());
				break;
			}
		}

		if (target) {

			using DefForceAlias = uint32_t (RE::TESQuest::*)(uint32_t aliasId, TESObjectREFR * reference);
			// id for SE is probably wrong!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			static REL::Relocation<DefForceAlias> func{ RELOCATION_ID(24523, 25052) };
			uint32_t res = func(quest, aliasId, target);
			flog::info("res {}", res);
		} else {
			using DefClearAlias = void (RE::TESQuest::*)(uint32_t aliasId);
			// id for SE is probably wrong!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			static REL::Relocation<DefClearAlias> func{ RELOCATION_ID(24521, 25050) };
			func(quest, aliasId);
			flog::info("clearing alias");
		}

		for (auto el : quest->aliases) {
			if (el->aliasName == alias) {
				RE::BGSRefAlias* elRef = skyrim_cast<RE::BGSRefAlias*>(el);
				flog::info("Alias refr {}", Name::GetName(elRef->GetReference()));
				break;
			}
		}

	}
	void MoveTo(RE::TESObjectREFR* melmoth, RE::TESObjectREFR* moveTo)
	{
		if (!melmoth) {
			return;
		}
		
		if (moveTo && (melmoth->GetParentCell() != moveTo->GetParentCell())) {
			melmoth->MoveTo(moveTo);
		} else {
			NiPoint3 point{ 1.0f, 1.0f, 1.0f };
			if (moveTo) {
				point = moveTo->GetPosition();
			}
			if (melmoth->GetFormType() == RE::FormType::ActorCharacter) {
				//true here is necessary to prevent a lag spike when moving player
				melmoth->As<RE::Actor>()->SetPosition(point, true);
			} else {
				melmoth->SetPosition(point);
			}
			
		}
		//toMove->SetPosition(moveTo->GetPosition());
	}
}

namespace Vore::Name
{
	std::string_view GetName(RE::TESObjectREFR* form)
	{
		if (!form) {
			return "NULL";
		}
		return form->GetDisplayFullName();
	}

	std::string_view GetName(RE::FormID form)
	{
		if (!form) {
			return "NULL";
		}
		RE::TESObjectREFR* formPtr = RE::TESForm::LookupByID<RE::TESObjectREFR>(form);
		if (formPtr) {
			return GetName(formPtr);
		} else {
			return "ERROR";
		}
	}
}

float Vore::Math::randfloat(float min, float max)
{
	/*if (first) {
		srand((unsigned int)time(NULL));
		first = false;
	}
	if (min > max) {
		flog::warn("trying to generate a random number where min > max");
		return 0.0f;
	} else if (min == max) {
		return min;
	}
	return min + (float)rand() / ((float)RAND_MAX * (max - min));*/
	std::uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}

size_t Vore::Math::randInt(size_t min, size_t max)
{
	/*if (first) {
		srand((unsigned int)time(NULL));
		first = false;
	}
	if (min > max) {
		flog::warn("trying to generate a random number where min > max");
		return 0;
	} else if (min == max) {
		return min;
	}
	return (size_t)(rand() / RAND_MAX * (max - min + 1) + min);*/
	std::uniform_int_distribution dist(min, max);
	return dist(gen);
}
