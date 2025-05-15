#include "headers/voredata.h"
#include "headers/nutils.h"
#include "headers/papyrusUtil.h"
#include "headers/settings.h"
#include "headers/ui.h"
#include "headers/voremain.h"
#include "headers/vutils.h"
#include "headers/distribute.h"

namespace Vore
{

	void VoreCharStats::CalcThreshold(bool isPred)
	{
		//get level
		//calc threshold for level

		if (isPred) {
			predThreshold = 2.0f * std::pow(predLevel, 1.95f) + 100.0f;
		} else {
			preyThreshold = 2.0f * std::pow(preyLevel, 1.95f) + 100.0f;
		}
	}

	void VoreCharStats::AdvSkill(bool isPred, float xp)
	{
		if (xp < 0) {
			return;
		}
		if (isPred) {
			if (predLevel == 100.0f) {
				predXp = 0.0;
				return;
			}
			predXp += xp;
			if (predThreshold <= predXp) {
				predXp -= predThreshold;
				predLevel++;
				CalcThreshold(isPred);
				AdvSkill(isPred, 0);
			}
		} else {
			if (preyLevel == 100.0f) {
				preyXp = 0.0;
				return;
			}
			preyXp += xp;
			if (preyThreshold <= preyXp) {
				preyXp -= preyThreshold;
				preyLevel++;
				CalcThreshold(isPred);
				AdvSkill(isPred, 0);
			}
		}
	}

	void VoreCharStats::SetLvl(bool isPred, float lvl)
	{
		if (isPred) {
			predLevel = std::clamp(std::trunc(lvl) ,0.0f, 100.0f);
			predXp = 0.0f;
			CalcThreshold(isPred);
		} else {
			preyLevel = std::clamp(std::trunc(lvl), 0.0f, 100.0f);
			preyXp = 0.0f;
			CalcThreshold(isPred);
		}
	}

	void VoreCharStats::SyncWithActor(bool isPred, RE::Actor* actor) const
	{
		float level = 0.0f;
		if (isPred) {
			level = predLevel;
		} else {
			level = preyLevel;
		}
		if (level >= 100) {
			Dist::SetPRank(isPred, actor, 6);
		} else if (level >= 80) {
			Dist::SetPRank(isPred, actor, 5);
		} else if (level >= 60) {
			Dist::SetPRank(isPred, actor, 4);
		} else if (level >= 40) {
			Dist::SetPRank(isPred, actor, 3);
		} else if (level >= 25) {
			Dist::SetPRank(isPred, actor, 2);
		} else if (level >= 10) {
			Dist::SetPRank(isPred, actor, 1);
		} else {
			Dist::SetPRank(isPred, actor, 0);
		}
	}

	void VoreCharStats::AdvSkillAndSync(bool isPred, float xp, RE::Actor* actor)
	{
		AdvSkill(isPred, xp);
		SyncWithActor(isPred, actor);
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

	VoreDataEntry* VoreData::IsValidGet(RE::FormID character)
	{
		auto it{ VoreData::Data.find(character) };
		if (it != std::end(VoreData::Data)) {
			return &(it->second);
		} else {
			return nullptr;
		}
	}

	VoreDataEntry* VoreData::GetDataOrMake(RE::TESObjectREFR* character)
	{
		if (!character) {
			flog::warn("Trying to make data for null character");
			return 0;
		}
		auto it{ VoreData::Data.find(character->GetFormID()) };
		if (it != std::end(VoreData::Data)) {
			return &(it->second);
		} else {
			VoreDataEntry value = {};
			value.aIsChar = character->GetFormType() == RE::FormType::ActorCharacter;
			if (value.aIsChar) {
				RE::Actor* asActor = character->As<RE::Actor>();

				value.aIsPlayer = asActor->IsPlayerRef();
				value.aAlive = !(asActor->IsDead());
				if (character->IsHumanoid()) {
					value.aSex = asActor->GetActorBase()->GetSex();
				} else {
					value.aSex = RE::SEX::kNone;
				}
				value.aEssential = asActor->IsEssential();
				value.aProtected = asActor->IsProtected();

				value.aScaleDefault = GetModelScale(asActor);

				// calculate if wg is allowed

				value.pdWGAllowed = CalcWgEnabled(asActor);

				VM::GetSingleton()->CreateObject2("Actor", value.meVm);
				VM::GetSingleton()->BindObject(value.meVm, GetHandle(character), false);
			} else {
				VM::GetSingleton()->CreateObject2("ObjectReference", value.meVm);
				VM::GetSingleton()->BindObject(value.meVm, GetHandle(character), false);
			}
			float mySize = GetObjectSize(character);
			// this might happen when vore happens outside of render distance?
			// idk how 3d and actor processing is connected
			// still, better have a fallback algorithm
			// size should never be 0, because we divide by it (I think)
			if (!mySize) {
				if (!value.aIsChar) {
					value.aSizeDefault = 5.0;
				} else {
					if (character->IsDragon()) {
						value.aSizeDefault = 3000.0;
					} else {
						value.aSizeDefault = 100.0;
					}
				}
			} else {
				value.aSizeDefault = (double)mySize;
			}
			value.aSize = value.aSizeDefault;

			value.me = character->GetHandle();
			// get vore stats (level, perks, etc.)
			// should be updated on each swallow and regurgitation
			value.GetVStats();

			for (auto& el : value.pdLoci) {
				el = VoreDataEntry::VoreState::hSafe;
			}
			flog::trace("Making new vore data entry for {}", Name::GetName(character));
			Data.emplace(character->GetFormID(), value);
			return &Data[character->GetFormID()];
		}
	}

	VoreCharStats* VoreData::IsValidStatGet(RE::FormID character)
	{
		auto it{ VoreData::Stats.find(character) };
		if (it != std::end(VoreData::Stats)) {
			return &(it->second);
		} else {
			return nullptr;
		}
	}

	static void SetPLevelFromRank(bool isPred, int rank, VoreCharStats &value) {
		switch (rank) {
		case 0:
			value.SetLvl(isPred, 0);
			break;
		case 1:
			value.SetLvl(isPred, 10);
			break;
		case 2:
			value.SetLvl(isPred, 25);
			break;
		case 3:
			value.SetLvl(isPred, 40);
			break;
		case 4:
			value.SetLvl(isPred, 60);
			break;
		case 5:
			value.SetLvl(isPred, 80);
			break;
		case 6:
			value.SetLvl(isPred, 100);
			break;
		default:
			value.SetLvl(isPred, 0);
			break;
		}
	}

	VoreCharStats* VoreData::GetStatOrMake(RE::TESObjectREFR* character)
	{
		if (!character) {
			flog::warn("Trying to make stats for null character");
			return nullptr;
		}
		auto it{ VoreData::Stats.find(character->GetFormID()) };
		if (it != std::end(VoreData::Stats)) {
			return &(it->second);
		} else {
			VoreCharStats value = {};
			RE::Character* chara = character->As<RE::Character>();
			if (!chara) {
				return nullptr;
			}
			//first distr factions for base stats
			Dist::DistrNPC(chara);
			//rank to skill
			int8_t pRank = Dist::GetPRank(true, chara);
			SetPLevelFromRank(true, pRank, value);
			pRank = Dist::GetPRank(false, chara);
			SetPLevelFromRank(false, pRank, value);
			Stats.emplace(character->GetFormID(), value);
			return &Stats[character->GetFormID()];
		}
	}

	bool VoreData::IsPred(RE::FormID character, bool onlyActive)
	{
		if (VoreDataEntry* characterData = VoreData::IsValidGet(character)) {
			uint8_t flag = 0;
			flag += !characterData->prey.empty();
			if (onlyActive) {
				return flag;
			}

			flag += characterData->pdFat > 0;
			flag += characterData->pdFatgrowth > 0;
			flag += characterData->pdSizegrowth > 0;

			for (auto& el : characterData->pdGrowthLocus) {
				if (el > 0) {
					flag++;
					break;
				}
			}
			if (flag > 0) {
				return true;
			}
		}
		return false;
	}

	bool VoreData::IsPrey(RE::FormID character)
	{
		if (VoreDataEntry* characterData = VoreData::IsValidGet(character)) {
			if (VoreDataEntry* predData = VoreData::IsValidGet(characterData->pred)) {
				if (!predData->prey.contains(character)) {
					flog::trace("Link between prey {} and pred {} is broken!", Name::GetName(character), Name::GetName(characterData->pred));
					return false;
				}
				if (characterData->pyDigestion == VoreDataEntry::VoreState::hNone) {
					flog::warn("No digestion type for {}!", Name::GetName(character));
					return false;
				} else if (characterData->pyLocus >= Locus::NUMOFLOCI) {
					flog::warn("Wrong locus for {}!", Name::GetName(character));
					return false;
				}
				return true;
			}
		}
		return false;
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
			if (VoreDataEntry* pData = VoreData::IsValidGet(p)) {
				pData->GetSize(size);
			}
		}
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
			Data[character].ClearAllUpdates();
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
			Data[character].ClearAllUpdates();
			VoreGlobals::delete_queue.insert(character);
		}
	}

	//todo - iterate and fix every character, setup loci
	void VoreData::DataSetup()
	{
		// Fix weird stuck characters in voredata (never happened to me, but who knows)
		std::vector<RE::FormID> bad = {};
		for (auto& [key, val] : Data) {
			if (!val.pdWGAllowed) {
				val.pdFat = 0.0;
				val.pdFatgrowth = 0.0;
				val.pdSizegrowth = 0.0;
				val.pdGrowthLocus.fill(0.0);
			}
			if (!IsPred(key, false) && !IsPrey(key)) {
				bad.push_back(key);
			}
		}
		if (bad.size() > 0) {
			flog::warn("Found {} bad VoreDataEntries!", bad.size());
			for (auto& el : bad)
			{
				Data.erase(el);
			}
		}
		bad.clear();

		// clear vore stats for irrelevant or broken npcs
		if (VoreSettings::dist_version != stats_dist_version) {
			stats_dist_version = VoreSettings::dist_version;
			Stats.clear();
		}
		for (auto& [key, val] : Stats) {
			RE::Actor* actor = RE::TESForm::LookupByID<RE::Actor>(key);
			if (!actor) {
				bad.push_back(key);
				continue;
			}
			// if it's a dead non-unique or respawning npc
			// player might resurrect non-unique npcs, so I don't reset their stats
			if (actor->IsDead() && (actor->GetActorBase()->Respawns() || !actor->GetActorBase()->IsUnique())) {
				bad.push_back(key);
				continue;
			}
		}
		if (bad.size() > 0) {
			flog::info("Found {} bad or irrlevant Vore Stats (in most cases this is normal and should be ignored)", bad.size());
			for (auto& el : bad) {
				Stats.erase(el);
			}
		}

		// Setup vore data
		for (auto& [key, val] : Data) {
			if (val.pred) {
				val.CalcFast();
				val.CalcSlow();
				if (val.aIsPlayer) {
					Core::HidePrey(val.get()->As<RE::Actor>());
				}
			}
			if (IsPred(key, false)) {
				val.SetPredUpdate(true);
				val.UpdatePredScale();
			}
			if (val.get() && val.get()->Is3DLoaded()) {
				VoreGlobals::body_morphs->ClearBodyMorphKeys(val.get(), VoreGlobals::MORPH_KEY);
			}
		}
		UI::VoreMenu::SetMenuMode(UI::VoreMenuMode::kDefault);
		// Final
		//flog::trace("Printing VoreData after loading is finished");
		//Log::PrintVoreData();
		flog::info("VoreData final size {}", VoreData::Data.size());
		flog::info("VoreStats final size {}", VoreData::Stats.size());
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

		s = s && a_intfc->WriteRecordData(&stats_dist_version, sizeof(stats_dist_version));

		size_t size = Stats.size();
		flog::info("Vore Stats, size: {}", size);
		s = s && a_intfc->WriteRecordData(&size, sizeof(size));

		for (auto& [fid, vds] : Stats) {
			s = s && SaveFormId(fid, a_intfc);
			s = s && SaveFormId(vds.reformer, a_intfc);
			s = s && a_intfc->WriteRecordData(&vds.predLevel, sizeof(vds.predLevel));
			s = s && a_intfc->WriteRecordData(&vds.predXp, sizeof(vds.predXp));
			s = s && a_intfc->WriteRecordData(&vds.predThreshold, sizeof(vds.predThreshold));
			s = s && a_intfc->WriteRecordData(&vds.preyLevel, sizeof(vds.preyLevel));
			s = s && a_intfc->WriteRecordData(&vds.preyXp, sizeof(vds.preyXp));
			s = s && a_intfc->WriteRecordData(&vds.preyThreshold, sizeof(vds.preyThreshold));
		}

		//size of Data
		size = Data.size();
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
			flog::info("Saving List of loci indigestion process, size: {} (should be 6)", size);
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
			s = s && a_intfc->WriteRecordData(&vde.aDeleteWhenDone, sizeof(vde.aDeleteWhenDone));

			//universal stats, not saved
			flog::info("Char type: {}, player {}, alive {}, size {}", (int)vde.aIsChar, vde.aIsPlayer, vde.aAlive, vde.aSize);

			//save pred stats
			flog::info("Pred");
			flog::info("XP: {}, Fat: {}, Fat Growth: {}, Size Growth: {}", vde.pdXP, vde.pdFat, vde.pdFatgrowth, vde.pdSizegrowth);
			s = s && a_intfc->WriteRecordData(&vde.pdXP, sizeof(vde.pdXP));
			s = s && a_intfc->WriteRecordData(&vde.pdFat, sizeof(vde.pdFat));
			s = s && a_intfc->WriteRecordData(&vde.pdFatgrowth, sizeof(vde.pdFatgrowth));
			s = s && a_intfc->WriteRecordData(&vde.pdSizegrowth, sizeof(vde.pdSizegrowth));

			//save prey stats
			flog::info("Prey");
			flog::info("XP: {}, Locus: {}, ElimLocus: {}, Digestion: {}, Struggle {} {} {}, Movement: {}",
				vde.pyXP, (uint8_t)vde.pyLocus, (uint8_t)vde.pyElimLocus, (uint8_t)vde.pyDigestion, vde.pyStruggleResource, vde.pyConsentEndo, vde.pyConsentLethal, (uint8_t)vde.pyLocusMovement);
			s = s && a_intfc->WriteRecordData(&vde.pyXP, sizeof(vde.pyXP));
			s = s && a_intfc->WriteRecordData(&vde.pyLocus, sizeof(vde.pyLocus));
			s = s && a_intfc->WriteRecordData(&vde.pyElimLocus, sizeof(vde.pyElimLocus));
			s = s && a_intfc->WriteRecordData(&vde.pyDigestion, sizeof(vde.pyDigestion));

			s = s && a_intfc->WriteRecordData(&vde.pyStruggleResource, sizeof(vde.pyStruggleResource));
			s = s && a_intfc->WriteRecordData(&vde.pyConsentEndo, sizeof(vde.pyConsentEndo));
			s = s && a_intfc->WriteRecordData(&vde.pyConsentLethal, sizeof(vde.pyConsentLethal));

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

				a_intfc->ReadRecordData(stats_dist_version);

				size_t size;
				a_intfc->ReadRecordData(size);
				flog::info("Stats, size: {}", size);

				//read reforms
				for (; size > 0; --size) {
					//write stats to VoreData or to an empty local variable
					RE::TESObjectREFR* statOwner = GetObjectPtr(a_intfc);
					Vore::VoreCharStats statsEmpty = {};
					Vore::VoreCharStats* charStats = GetStatOrMake(statOwner);
					flog::info("Name {}", Name::GetName(statOwner));
					if (!charStats) {
						charStats = &statsEmpty;
					}
					RE::TESObjectREFR* reformer = GetObjectPtr(a_intfc);
					if (reformer) {
						charStats->reformer = reformer->GetFormID();
						flog::info("Reformer {}", Name::GetName(reformer));
					} else {
						charStats->reformer = 0U;
						flog::info("No Reformer");
					}
					a_intfc->ReadRecordData(charStats->predLevel);
					a_intfc->ReadRecordData(charStats->predXp);
					a_intfc->ReadRecordData(charStats->predThreshold);
					a_intfc->ReadRecordData(charStats->preyLevel);
					a_intfc->ReadRecordData(charStats->preyXp);
					a_intfc->ReadRecordData(charStats->preyThreshold);
				}

				//size of Data
				a_intfc->ReadRecordData(size);
				flog::info("Vore Data entries, size: {}", size);

				for (; size > 0; --size) {
					flog::trace("\n\n");

					flog::info("New Entry");

					//write entry to VoreData or to an empty local variable
					RE::TESObjectREFR* entryOwner = GetObjectPtr(a_intfc);
					Vore::VoreDataEntry entryEmpty = {};
					Vore::VoreDataEntry* entry = GetDataOrMake(entryOwner);
					flog::info("Name {}", Name::GetName(entryOwner));
					if (!entry) {
						entry = &entryEmpty;
						flog::info("Skipping data entry");
					}

					// start reading actual vore data entry

					auto* tpred = GetObjectPtr(a_intfc);
					if (tpred) {
						entry->pred = tpred->GetFormID();
						flog::info("Pred {}", Name::GetName(tpred));
					} else {
						entry->pred = 0U;
						flog::info("No Pred");
					}

					size_t sizePrey;
					a_intfc->ReadRecordData(sizePrey);
					flog::info("List of Prey, size: {}", sizePrey);
					for (; sizePrey > 0; --sizePrey) {
						RE::TESObjectREFR* prey = nullptr;
						prey = GetObjectPtr(a_intfc);
						if (prey) {
							entry->prey.insert(prey->GetFormID());
							flog::info("Prey {}", Name::GetName(prey));
						}
					}

					size_t sizeLoci;
					a_intfc->ReadRecordData(sizeLoci);
					flog::info("List of Loci, size: {}", sizeLoci);
					for (int i = 0; i < sizeLoci; i++) {
						VoreDataEntry::VoreState locus = VoreDataEntry::VoreState::hNone;
						a_intfc->ReadRecordData(locus);
						entry->pdLoci[i] = locus;
						flog::info("Locus {} {}", i, (uint8_t)locus);
					}

					size_t sizeLocAcid;
					a_intfc->ReadRecordData(sizeLocAcid);
					flog::info("List of Loci acid, size: {}", sizeLocAcid);
					for (int i = 0; i < sizeLocAcid; i++) {
						double locus = 0;
						a_intfc->ReadRecordData(locus);
						entry->pdAcid[i] = locus;
						flog::info("Locus {} {}", i, locus);
					}

					size_t sizeIndigestion;
					a_intfc->ReadRecordData(sizeIndigestion);
					flog::info("List of Loci indigestion process, size: {}", sizeIndigestion);
					for (int i = 0; i < sizeIndigestion; i++) {
						double locus = 0;
						a_intfc->ReadRecordData(locus);
						entry->pdIndigestion[i] = locus;
						flog::info("Locus {} {}", i, locus);
					}

					size_t sizeGrowth;
					a_intfc->ReadRecordData(sizeGrowth);
					flog::info("List of Growth per body part, size: {}", sizeGrowth);
					for (int i = 0; i < sizeGrowth; i++) {
						double growth = 0;
						a_intfc->ReadRecordData(growth);
						entry->pdGrowthLocus[i] = growth;
						flog::info("Growth {} {}", i, growth);
					}

					a_intfc->ReadRecordData(entry->aSizeDefault);
					entry->aSize = entry->aSizeDefault;
					a_intfc->ReadRecordData(entry->aDeleteWhenDone);

					flog::info("Char type: {}, is player: {}, alive {}, size {}", (int)entry->aIsChar, entry->aIsPlayer, entry->aAlive, entry->aSize);

					flog::info("Pred");
					a_intfc->ReadRecordData(entry->pdXP);
					a_intfc->ReadRecordData(entry->pdFat);
					a_intfc->ReadRecordData(entry->pdFatgrowth);
					a_intfc->ReadRecordData(entry->pdSizegrowth);
					flog::info("XP: {}, Fat: {}, Fat Growth: {}, Size Growth: {}", entry->pdXP, entry->pdFat, entry->pdFatgrowth, entry->pdSizegrowth);

					flog::info("Prey");
					a_intfc->ReadRecordData(entry->pyXP);
					a_intfc->ReadRecordData(entry->pyLocus);
					a_intfc->ReadRecordData(entry->pyElimLocus);
					a_intfc->ReadRecordData(entry->pyDigestion);

					a_intfc->ReadRecordData(entry->pyStruggleResource);
					a_intfc->ReadRecordData(entry->pyConsentEndo);
					a_intfc->ReadRecordData(entry->pyConsentLethal);

					a_intfc->ReadRecordData(entry->pyLocusMovement);
					flog::info("XP: {}, Locus: {}, ElimLocus: {}, Digestion: {}, Struggle: {} {} {}, Movement: {}",
						entry->pyXP, (uint8_t)entry->pyLocus, (uint8_t)entry->pyElimLocus, (uint8_t)entry->pyDigestion, entry->pyStruggleResource, entry->pyConsentEndo, entry->pyConsentLethal, (uint8_t)entry->pyLocusMovement);
					a_intfc->ReadRecordData(entry->pyDigestProgress);
					a_intfc->ReadRecordData(entry->pySwallowProcess);
					a_intfc->ReadRecordData(entry->pyLocusProcess);
					flog::info("Digestion Process: {}, Swallow Process: {}, Locus Process: {}", entry->pyDigestProgress, entry->pySwallowProcess, entry->pyLocusProcess);

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
		Stats.clear();
		Data.clear();
	}
}
