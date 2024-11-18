#include "headers/voredata.h"
#include "headers/nutils.h"
#include "headers/settings.h"
#include "headers/ui.h"
#include "headers/vutils.h"
#include "headers/papyrusUtil.h"

namespace Vore
{

	double VoreDataEntry::GetStomachSpace(uint64_t /*locus*/)
	{
		return 0;
	}

	RE::TESObjectREFR* VoreDataEntry::get() const
	{
		return this->me.get().get();
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

				VM::GetSingleton()->CreateObject2("Actor", value.meVm);
				VM::GetSingleton()->BindObject(value.meVm, GetHandle(target), false);
			} else {
				VM::GetSingleton()->CreateObject2("ObjectReference", value.meVm);
				VM::GetSingleton()->BindObject(value.meVm, GetHandle(target), false);
			}
			value.aSize = (double)GetObjectSize(target);
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

		// Setup bellies
		for (auto& [key, val] : Data) {
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
			s = s && a_intfc->WriteRecordData(&vde.aSize, sizeof(vde.aSize));

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

					if (entry.aSize <= 1.0) {
						a_intfc->ReadRecordData(entry.aSize);
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
