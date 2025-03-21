#include "headers/papyrusUtil.h"
#include "headers/dialogue.h"
#include "headers/nutils.h"
#include "headers/settings.h"
#include "headers/voredata.h"
#include "headers/voremain.h"
#include "headers/vutils.h"

namespace
{
	using VM = RE::BSScript::Internal::VirtualMachine;
	using ObjectPtr = RE::BSTSmartPointer<RE::BSScript::Object>;
}

namespace Vore
{
	void PapyrusAPI::TalkToPred(RE::StaticFunctionTag*)
	{
		flog::info("PPAPI TalkToPred");
		if (VoreDataEntry* playerData = VoreData::IsValidGet(RE::PlayerCharacter::GetSingleton()->GetFormID())) {
			if (playerData->pred) {
				if (VoreDataEntry* predData = VoreData::IsValidGet(playerData->pred)) {
					if (predData->aIsChar) {
						Dialogue::TalkToA(predData->get()->As<RE::Actor>());
					}
				}
			}
		}
	}
	void PapyrusAPI::TalkToPrey(RE::StaticFunctionTag*)
	{
		flog::info("PPAPI TalkToPrey");
		if (VoreDataEntry* playerData = VoreData::IsValidGet(RE::PlayerCharacter::GetSingleton()->GetFormID())) {
			for (auto& a_prey : playerData->prey) {
				if (VoreDataEntry* preyData = VoreData::IsValidGet(a_prey)) {
					if (preyData->aIsChar && preyData->aAlive) {
						Dialogue::TalkToA(preyData->get()->As<RE::Actor>());
						return;
					}
				}
			}
		}
	}

	void PapyrusAPI::TalkToActor(RE::StaticFunctionTag*, RE::Actor* aToTalk)
	{
		flog::info("PPAPI TalkToActor");
		Dialogue::TalkToA(aToTalk);
	}

	void PapyrusAPI::Swallow(RE::StaticFunctionTag*, RE::Actor* pred, RE::TESObjectREFR* prey, float locus, float digestionType, bool fullSwallow)
	{
		flog::info("PPAPI Swallow");
		Locus loc = lNone;
		if (locus < 0) {
			loc = Dialogue::GetLocusForSwallow(pred, prey);
			if (loc >= NUMOFLOCI) {
				loc = lStomach;
			}
		} else if (locus >= (float)Locus::lStomach && locus < (float)Locus::NUMOFLOCI) {
			loc = (Locus)locus;
		} else {
			loc = lStomach;
		}

		VoreDataEntry::VoreState dig = VoreDataEntry::hSafe;

		if (digestionType > (float)VoreDataEntry::hNone && digestionType <= (float)VoreDataEntry::hReformation) {
			dig = (VoreDataEntry::VoreState)digestionType;
		}
		
		Core::Swallow(pred, prey, loc, dig, fullSwallow, true);
	}

	void PapyrusAPI::Regurgitate(RE::StaticFunctionTag*, RE::Actor* pred, RE::TESObjectREFR* prey)
	{
		flog::info("PPAPI Regurgitate");
		Core::Regurgitate(pred, prey->GetFormID(), Core::rNormal);
	}

	void PapyrusAPI::RegurgitateAll(RE::StaticFunctionTag*, RE::Actor* pred, float locus)
	{
		flog::info("PPAPI RegurgitateAll");
		uint8_t intLoc = static_cast<uint8_t>(locus);
		if (intLoc >= Locus::lStomach && intLoc < Locus::NUMOFLOCI || intLoc == Locus::lNone) {
			Core::RegurgitateAll(pred, (Locus)intLoc, Core::rNormal);
		}
	}

	void PapyrusAPI::Consent(RE::StaticFunctionTag*, RE::Actor* pred, RE::TESObjectREFR* prey, bool willing, bool lethal)
	{
		flog::info("PPAPI Consent: Yes {} Lethal {}", willing, lethal);
		if (!prey || prey->GetFormType() != RE::FormType::ActorCharacter) {
			return;
		}
		RE::Actor* preyA = prey->As<RE::Actor>();
		if (VoreDataEntry* targetData = VoreData::IsValidGet(preyA->GetFormID())) {
			targetData->SetConsent(willing, lethal);
		} else if (pred) {
			Dialogue::SetConsent(pred, preyA, willing, lethal);
		}
	}

	void PapyrusAPI::DisableEscape(RE::StaticFunctionTag*, RE::TESObjectREFR* target)
	{
		flog::info("PPAPI DisableEscape");
		if (VoreDataEntry* targetData = VoreData::IsValidGet(target->GetFormID())) {
			targetData->pyStruggleResource = 0;
			targetData->pyStruggling = false;
		}
	}

	void PapyrusAPI::SwitchTo(RE::StaticFunctionTag*, RE::TESObjectREFR* target, float digestionType)
	{
		flog::info("PPAPI SwitchTo");
		if (VoreDataEntry* targetData = VoreData::IsValidGet(target->GetFormID())) {
			uint8_t intDig = static_cast<uint8_t>(digestionType);
			if (intDig > VoreDataEntry::hNone && intDig <= VoreDataEntry::hReformation) {
				if (VoreDataEntry* predData = VoreData::IsValidGet(targetData->pred)) {
					predData->SetDigestionAsPred(targetData->pyLocus, (VoreDataEntry::VoreState)intDig, true);
				}
			}
		}
	}

	void PapyrusAPI::SwitchPredTo(RE::StaticFunctionTag*, RE::Actor* target, float digestionType, bool forceNoEscape)
	{
		flog::info("PPAPI SwitchPredTo");
		if (VoreDataEntry* targetData = VoreData::IsValidGet(target->GetFormID())) {
			uint8_t intDig = static_cast<uint8_t>(digestionType);
			if (intDig > VoreDataEntry::hNone && intDig <= VoreDataEntry::hReformation) {
				targetData->SetDigestionAsPred(lNone, (VoreDataEntry::VoreState)intDig, true);
				if (forceNoEscape) {
					for (auto& prey : targetData->prey) {
						if (VoreDataEntry* preyData = VoreData::IsValidGet(prey)) {
							preyData->pyStruggleResource = 0;
							preyData->pyStruggling = false;
						}
					}
				}
			}
		}
	}

	void PapyrusAPI::SetPredControl(RE::StaticFunctionTag*, [[maybe_unused]] bool allowControl)
	{
		flog::info("PPAPI SetPredControl");
		//placeholder
		//placeholder
		//placeholder
		//placeholder
		//placeholder
	}

	void PapyrusAPI::KillPrey(RE::StaticFunctionTag*, RE::Actor* prey)
	{
		flog::info("PPAPI KillPrey");
		if (VoreDataEntry* preyData = VoreData::IsValidGet(prey->GetFormID())) {
			preyData->DigestLive();
		}
	}

	void PapyrusAPI::PlanReformation(RE::StaticFunctionTag*, RE::Actor* pred, RE::Actor* prey)
	{
		flog::info("PPAPI PlanReformation");
		VoreCharStats* preyStats = VoreData::GetStatOrMake(prey);
		if (preyStats) {
			preyStats->reformer = pred->GetFormID();
		}
	}

	void PapyrusAPI::UnplanReformation(RE::StaticFunctionTag*, [[maybe_unused]] RE::Actor* pred, RE::Actor* prey)
	{
		if (VoreCharStats* preyStats = VoreData::IsValidStatGet(prey->GetFormID())) {
			preyStats->reformer = 0;
		}
	}

	void PapyrusAPI::InventoryVore(RE::StaticFunctionTag*, RE::Actor* pred)
	{
		Core::InventoryVore(pred);
	}

	RE::TESObjectREFR* PapyrusAPI::MakeSC(RE::StaticFunctionTag*)
	{
		RE::TESObjectREFR* itemCell = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectREFR>(0x28556, "SkyrimVorePP.esp");
		RE::TESObjectCONT* scForm = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectCONT>(0x28569, "SkyrimVorePP.esp");

		auto ic = itemCell->PlaceObjectAtMe(scForm, false).get();

		return ic;
	}

	void PapyrusAPI::CommitSC(RE::StaticFunctionTag*, RE::Actor* pred, RE::TESObjectREFR* sc)
	{
		flog::info("Item vore End");
		if (sc->GetInventory().size() == 0) {
			sc->SetDelete(true);
			flog::warn("Empty stomach container - won't swallow");
			return;
		}
		VoreDataEntry* scData = VoreData::GetDataOrMake(sc);
		if (!scData) {
			flog::error("Can't make voredata entry for stomach container");
			return;
		}
		scData->aSize = 0;
		scData->aDeleteWhenDone = true;
		for (auto& [item, el] : sc->GetInventory()) {
			scData->aSize += GetItemSize(item);
			//sc->RemoveItem(item, el.first, RE::ITEM_REMOVE_REASON::kStoreInContainer, nullptr, bones);
		}
		scData->aSizeDefault = scData->aSize;
		Locus loc = Dialogue::GetLocusForSwallow(pred, sc);
		if (loc >= NUMOFLOCI) {
			loc = lStomach;
		}
		Core::Swallow(pred, sc, loc, VoreDataEntry::hSafe, false, false);
	}

	void PapyrusAPI::Strip(RE::StaticFunctionTag*, RE::Actor* target)
	{
		flog::info("PPAPI Strip");
		//devourmentsexlab.instance().strip(prey)
		UnequipAll(target);
	}

	float PapyrusAPI::GetDefaultLocus(RE::StaticFunctionTag*)
	{
		flog::info("PPAPI GetDefaultLocus");
		return (float)PlayerPrefs::voreLoc;
	}

	float PapyrusAPI::GetRandomLocus(RE::StaticFunctionTag*, RE::Actor* target)
	{
		flog::info("PPAPI GetRandomLocus");
		std::vector<Locus> loci{};
		loci.reserve(6);
		loci.push_back(Locus::lStomach);
		loci.push_back(Locus::lBowel);
		if (PlayerPrefs::hasB(target)) {
			loci.push_back(Locus::lBreastl);
			if (VoreSettings::dual_boobs) {
				loci.push_back(Locus::lBreastr);
			}
		}
		if (PlayerPrefs::hasV(target)) {
			loci.push_back(Locus::lWomb);
		}
		if (PlayerPrefs::hasC(target)) {
			loci.push_back(Locus::lBalls);
		}
		return (float)loci[Math::randInt(0, loci.size() - 1)];
	}


	//static bool InitializeOffsets()
	//{
	//	void* MyAddress = NULL;
	//	unsigned long long MyId = 0;
	//	unsigned long long MyOffset = 0;
	//	// Allocate on stack so it will be unloaded when we exit this function.
	//	// No need to have the whole database loaded and using up memory for no reason.
	//	VersionDb db;

	//	// Load database with current executable version.
	//	if (!db.Load()) {
	//		flog::critical("Failed to load version database for current executable!");
	//		return false;
	//	} else {
	//		// "SkyrimSE.exe", "1.5.97.0"
	//		flog::info("Loaded database for %s version %s.", db.GetModuleName().c_str(), db.GetLoadedVersionString().c_str());
	//	}

	//	// This address already includes the base address of module so we can use the address directly.
	//	/*MyAddress = db.FindAddressById(123);
	//	if (MyAddress == NULL) {
	//		flog::critical("Failed to find address!");
	//		return false;
	//	}*/

	//	void* ptr = reinterpret_cast<void*>(0x003CDEE0);

	//	// This offset does not include base address. Actual address would be ModuleBase + MyOffset.
	//	/*if (!db.FindIdByAddress(ptr, MyOffset)) {
	//		flog::critical("Failed to find offset for my thing!");
	//		return false;
	//	}
	//	flog::info("My id", MyOffset);
	//
	//	*/

	//	MyOffset = 25052;
	//	if (!db.FindIdByOffset(MyOffset, MyId)) {
	//		flog::critical("Failed to find id for my offset!");
	//		return false;
	//	}
	//	flog::critical("my id {:x}", MyId);
	//	MyAddress = db.FindAddressById(MyId);

	//	flog::critical("my address {:x}", (unsigned long long)MyAddress);

	//	// Everything was successful.
	//	return true;
	//}

	void PapyrusAPI::Register()
	{
		VM::GetSingleton()->RegisterFunction("PreyDialogue", "SVPP_API", TalkToPrey);
		VM::GetSingleton()->RegisterFunction("PredDialogue", "SVPP_API", TalkToPred);
		VM::GetSingleton()->RegisterFunction("ActorDialogue", "SVPP_API", TalkToActor);

		VM::GetSingleton()->RegisterFunction("Swallow", "SVPP_API", Swallow);
		VM::GetSingleton()->RegisterFunction("Regurgitate", "SVPP_API", Regurgitate);
		VM::GetSingleton()->RegisterFunction("RegurgitateAll", "SVPP_API", RegurgitateAll);

		VM::GetSingleton()->RegisterFunction("Consent", "SVPP_API", Consent);
		VM::GetSingleton()->RegisterFunction("DisableEscape", "SVPP_API", DisableEscape);
		VM::GetSingleton()->RegisterFunction("SwitchTo", "SVPP_API", SwitchTo);
		VM::GetSingleton()->RegisterFunction("SwitchPredTo", "SVPP_API", SwitchPredTo);
		VM::GetSingleton()->RegisterFunction("SetPredControl", "SVPP_API", SetPredControl);
		VM::GetSingleton()->RegisterFunction("KillPrey", "SVPP_API", KillPrey);
		VM::GetSingleton()->RegisterFunction("PlanReformation", "SVPP_API", PlanReformation);
		VM::GetSingleton()->RegisterFunction("UnplanReformation", "SVPP_API", UnplanReformation);
		VM::GetSingleton()->RegisterFunction("InventoryVore", "SVPP_API", InventoryVore);
		VM::GetSingleton()->RegisterFunction("MakeSC", "SVPP_API", MakeSC);
		VM::GetSingleton()->RegisterFunction("CommitSC", "SVPP_API", CommitSC);

		VM::GetSingleton()->RegisterFunction("Strip", "SVPP_API", Strip);
		VM::GetSingleton()->RegisterFunction("GetDefaultLocus", "SVPP_API", GetDefaultLocus);
		VM::GetSingleton()->RegisterFunction("GetRandomLocus", "SVPP_API", GetRandomLocus);
		/*REL::IDDatabase::Offset2ID t;
		flog::critical("{:x}", t(0x61dc));
		InitializeOffsets();*/
	}
}
