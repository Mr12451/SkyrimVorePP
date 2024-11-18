#pragma once
#include "headers/racemenu.h"
#include "headers/voredata.h"

namespace Vore
{
	class VoreSettings
	{
		//settings will be loaded here
	public:
		// update speed
		static inline double slow_update = 5;
		static inline double fast_update = 0.2;
		static inline float belly_fast_update = 0.05f;

		// keycodes: look here https://ck.uesp.net/wiki/Input_Script

		// main
		static inline uint32_t k_vore_key = 59;
		static inline uint32_t k_regurg_key = 62;

		// menus
		static inline uint32_t k_i_menu = 60;
		static inline uint32_t k_sw_menu = 61;

		// menu actions

		static inline uint32_t k_menu_1 = 64;
		static inline uint32_t k_menu_2 = 65;
		static inline uint32_t k_menu_3 = 66;
		static inline uint32_t k_menu_4 = 68;

		static inline uint32_t k_test = 87;

		// ui debug

		static inline bool ui_show_struggle_sliders = false;
		static inline bool ui_show_wg = true;
		static inline bool ui_show_more = true;

		// main

		static inline bool dual_boobs = true;
		static inline bool digest_protected = true;
		static inline bool digest_essential = true;
		static inline bool swallow_auto = true;
		static inline float size_softcap = 700.0f;
		static inline float size_softcap_power = 0.45f;

		//static inline double digestion_amount_base = 0.03;
		static inline double digestion_amount_base = 1.0;
		static inline double struggle_amount = 5;
		static inline double struggle_stamina = 15;
		static inline double indigestion_loss = 0.5;
		static inline double acid_damage = 25;
		static inline double acid_gain = 2;
		static inline double acid_loss = 0.5;
		static inline double swallow_auto_speed = 30;
		static inline double swallow_decrease_speed = 20;
		//full tour speed
		static inline double locus_process_speed = 1;

		//wg
		static inline double wg_fattemp = 0.02;
		static inline double wg_fatlong = 0.005;
		static inline double wg_locusgrowth = 0.008;
		static inline double wg_sizegrowth = 0.005;
		/*static inline double wg_fattemp = 1.0;
		static inline double wg_fatlong = 0;
		static inline double wg_locusgrowth = 0;
		static inline double wg_sizegrowth = 0;*/

		// 6 humans/hour
		// 120
		// 30/hour
		// 1/2 minute
		// 1/120 second ~ 0.008
		static inline double wg_loss_temp = 0.0001;
		static inline double wg_loss_long = 0.00001;
		static inline double wg_loss_locus = 0.00001;
		static inline double wg_loss_size = 0.00001;

		//sliders

		//changes the graph of the function from cubic volume to linear slider increase
		static inline float slider_pow = 0.5f;
		//weight of a single person

		//max slider step per second
		static inline float slider_maxstep = 150.0f;

		/*
		uStomach,
		uThroat,
		uBowel,
		uBreastl,
		uBreastr,
		uWomb,
		uCock,
		uBalls,

		uFatBelly,
		uFatAss,
		uFatBreasts,
		uFatCock,

		uFatLow,
		uFatHigh,
		uGrowthLow,
		uGrowthHigh,

		NUMOFSLIDERS
		*/

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//
		// if you encounter a bug where sliders freeze and stop changing, change bEnableEarlyRegistration to true in racemenu's skee.ini
		//
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		// !!!!!!!!!!!!!!!!!!!!!!!!! one person size for sliders in the belly group should be equal
		static inline std::array<std::vector<std::tuple<std::string, float, float>>, LocusSliders::NUMOFSLIDERS> sliders_bodypart_female = { { // gr1 (belly)
			{ { "Vore prey belly", 0.7f, 10.0f }, { "Giant belly up", 0.4f, 0.4f } },
			{ { "Giant belly (coldsteelj)", 1.0f, 5.0f } },
			{ { "Vore prey belly", 0.7f, 10.0f } },
			{ { "FatBelly", 2.4f, 4.0f } },
			//other vore
			{ { "ChestDepth", 3.0f, 6.0f } },
			{ { "BVoreL", 1.0f, 10.0f } },
			{ { "BVoreR", 1.0f, 10.0f } },
			{ { "SOS - SchlongLength", 4.0f, 10.0f }, { "SOS - SchlongGirth", 4.0f, 10.0f }, { "SOS - GlansSize", -1.6f, -4.0f }, { "SOS - GlansMushroom", 0.8f, 2.0f } },
			{ { "CVore", 1.0f, 10.0f } },
			//wg
			{ { "ChubbyLegs", 0.25f, 2.5f }, { "Thighs", 0.5f, 5.0f }, { "AppleCheeks", 1.0f, 10.0f }, { "Butt", 1.0f, 10.0f }, { "BigButt", 1.0f, 10.0f }, { "ChubbyButt", 1.0f, 10.0f } },
			{ { "BreastsNewSH", 1.0f, 4.0f }, { "Breasts", 1.0f, 4.0f }, { "DoubleMelon", 0.25f, 1.0f }, { "BreastWidth", 1.0f, 4.0f } },
			{ { "SOS - SchlongLength", 1.0f, 4.0f }, { "SOS - SchlongGirth", 1.0f, 4.0f }, { "SOS - GlansSize", -0.4f, -4.0f }, { "SOS - GlansMushroom", 0.2f, 0.8f }, { "SOS - BallsSize", 1.0f, 4.0f },
				{ "SOS - BallsGravity", 1.0f, 4.0f }, { "SOS - BallsSpherify", 0.5f, 2.0f } },
			//fat
			{ { "SSBBW3 body", 1.0f, 4.0f } },
			{ { "SSBBW3 body", 1.0f, 4.0f } },
			{ { "SSBBW2 body", 1.0f, 4.0f } },
			{ { "SSBBW2 body", 1.0f, 4.0f } } } };

		static inline std::array<std::vector<std::tuple<std::string, float, float>>, LocusSliders::NUMOFSLIDERS> sliders_bodypart_male = { { // gr1 (belly)
			{ { "Vore prey belly", 0.7f, 10.0f }, { "Giant belly up", 0.4f, 0.4f } },
			{ { "Giant belly (coldsteelj)", 1.0f, 5.0f } },
			{ { "Vore prey belly", 0.7f, 10.0f } },
			{ { "FatBelly", 1.0f, 4.0f } },
			//other vore
			{ { "ChestDepth", 1.0f, 2.0f } },
			{ { "BVoreL", 1.0f, 10.0f } },
			{ { "BVoreR", 1.0f, 10.0f } },
			{ { "SOS - SchlongLength", 4.0f, 10.0f }, { "SOS - SchlongGirth", 4.0f, 10.0f }, { "SOS - GlansSize", -1.6f, -4.0f }, { "SOS - GlansMushroom", 0.8f, 2.0f } },
			{ { "CVore", 1.0f, 10.0f } },
			//wg
			{ { "ChubbyLegs", 0.25f, 2.5f }, { "Thighs", 0.5f, 5.0f }, { "AppleCheeks", 1.0f, 10.0f }, { "Butt", 1.0f, 10.0f }, { "BigButt", 1.0f, 10.0f }, { "ChubbyButt", 1.0f, 10.0f } },
			{ { "BreastsNewSH", 1.0f, 4.0f }, { "Breasts", 1.0f, 4.0f }, { "DoubleMelon", 0.25f, 1.0f }, { "BreastWidth", 1.0f, 4.0f } },
			{ { "SOS - SchlongLength", 1.0f, 4.0f }, { "SOS - SchlongGirth", 1.0f, 4.0f }, { "SOS - GlansSize", -0.4f, -4.0f }, { "SOS - GlansMushroom", 0.2f, 0.8f }, { "SOS - BallsSize", 1.0f, 4.0f },
				{ "SOS - BallsGravity", 1.0f, 4.0f }, { "SOS - BallsSpherify", 0.5f, 2.0f } },
			//fat
			{ { "SSBBW3 body", 1.0f, 4.0f } },
			{ { "SSBBW3 body", 1.0f, 4.0f } },
			{ { "SSBBW2 body", 1.0f, 4.0f } },
			{ { "SSBBW2 body", 1.0f, 4.0f } } } };

		static inline std::array<std::vector<std::tuple<std::string, float, float>>, LocusSliders::NUMOFSLIDERS> sliders_bodypart_creature = { { // gr1 (belly)
			{ { "Vore prey belly", 0.7f, 10.0f } },
			{ { "Vore prey belly", 0.7f, 10.0f } },
			{ { "Vore prey belly", 0.7f, 10.0f } },
			{},
			{},
			{},
			{},
			{},
			{ { "CVore", 1.0f, 10.0f } },
			//wg
			{},
			{},
			{},
			//fat
			{},
			{},
			{},
			{} } };

		static inline std::array<std::vector<std::tuple<std::string, float, float>>, Locus::NUMOFLOCI> struggle_sliders = { { { { "StruggleSlider1", 1.0f, 5.0f }, { "StruggleSlider2", 1.0f, 5.0f }, { "StruggleSlider3", 1.0f, 5.0f } },
			{ { "StruggleSlider1", 1.0f, 5.0f }, { "StruggleSlider2", 1.0f, 5.0f }, { "StruggleSlider3", 1.0f, 5.0f } },
			{ { "BVoreStruggleL1", 1.0f, 5.0f }, { "BVoreStruggleL2", 1.0f, 5.0f }, { "BVoreStruggleL3", 1.0f, 5.0f } },
			{ { "BVoreStruggleR1", 1.0f, 5.0f }, { "BVoreStruggleR2", 1.0f, 5.0f }, { "BVoreStruggleR3", 1.0f, 5.0f } },
			{ { "StruggleSlider1", 1.0f, 5.0f }, { "StruggleSlider2", 1.0f, 5.0f }, { "StruggleSlider3", 1.0f, 5.0f } },
			{} } };
		/*
		stomach,
		bowel,
		breastl,
		breastr,
		womb,
		balls,

		NUMOFLOCI

		In this order:
		uFatBelly,
		uFatAss,
		uFatBreasts,
		uFatCock,

		*/
		static inline std::array<std::array<double, 4>, Locus::NUMOFLOCI> voretypes_partgain = { { { 0.6, 0.2, 0.2, 0.0 },
			{ 0.2, 0.5, 0.5, 0.0 },
			{ 0.0, 0.0, 1.0, 0.0 },
			{ 0.0, 0.0, 1.0, 0.0 },
			{ 0.0, 0.5, 0.5, 0.0 },
			{ 0.0, 0.1, 0.1, 0.5 } } };

		static void LoadIniSettings();
	};

	class VoreGlobals
	{
		//probably a shitty way to do this
	public:
		static constexpr float slider_one = 100.0f;
		static constexpr const char* MORPH_KEY = "SkyrimVorePP.esp";
		static const inline std::set<RE::FormType> allowed_pickup{
			RE::FormType::Scroll,
			RE::FormType::Armor,
			RE::FormType::Book,
			RE::FormType::Ingredient,
			RE::FormType::Misc,
			RE::FormType::Weapon,
			RE::FormType::Ammo,
			RE::FormType::AlchemyItem,
			//RE::FormType::Note,
			RE::FormType::SoulGem
		};
		static inline IBodyMorphInterface* body_morphs = nullptr;
		//static inline std::mt19937 randomMT;
		static inline const std::array<Locus, Locus::NUMOFLOCI> locus_transfer_destination = { Locus::lBowel,
			Locus::lStomach,
			Locus::lStomach,
			Locus::lStomach,
			Locus::lStomach,
			Locus::lStomach };

		static inline std::set<RE::FormID> delete_queue{};

		static inline std::unordered_map<std::string_view, std::string_view> race_remains{
			//humans
			{ "NordRace", "VoreSkeletonHuman" },
			{ "NordRaceVampire", "VoreSkeletonHuman" },
			{ "NordRaceAstrid", "VoreSkeletonHuman" },
			{ "BretonRace", "VoreSkeletonHuman" },
			{ "BretonRaceVampire", "VoreSkeletonHuman" },
			{ "ElderRace", "VoreSkeletonHuman" },
			{ "ElderRaceVampire", "VoreSkeletonHuman" },
			{ "ImperialRace", "VoreSkeletonHuman" },
			{ "ImperialRaceVampire", "VoreSkeletonHuman" },
			{ "RedguardRace", "VoreSkeletonHuman" },
			{ "RedguardRaceVampire", "VoreSkeletonHuman" },
			//argonians
			{ "ArgonianRace", "VoreSkeletonArg" },
			{ "ArgonianRaceVampire", "VoreSkeletonArg" },
			//khajit
			{ "KhajitRace", "VoreSkeletonKha" },
			{ "KhajitRaceVampire", "VoreSkeletonKha" },
			//elf
			{ "DarkElfRace", "VoreSkeletonMer" },
			{ "DarkElfRaceVampire", "VoreSkeletonMer" },
			{ "HighElfRace", "VoreSkeletonMer" },
			{ "HighElfRaceVampire", "VoreSkeletonMer" },
			{ "WoodElfRace", "VoreSkeletonMer" },
			{ "WoodElfRaceVampire", "VoreSkeletonMer" },
			//orc
			{ "OrcRace", "VoreSkeletonOrc" },
			{ "OrcRaceVampire", "VoreSkeletonOrc" },
			//dragon
			{ "DragonRace", "VoreSkeletonDragon" },
			{ "AlduinRace", "VoreSkeletonDragon" },
			{ "UndeadDragonRace", "VoreSkeletonDragon" }
		};
	};

	class PlayerPrefs
	{
	public:
		static inline Locus voreLoc = Locus::lStomach;
		static inline Locus regLoc = Locus::lNone;
		static inline VoreState voreType = VoreState::hSafe;

		static void clear()
		{
			voreLoc = Locus::lStomach;
			regLoc = Locus::lNone;
			voreType = VoreState::hSafe;
		}

		static std::string_view GetTypeStr(VoreState vType)
		{
			switch (vType) {
			case VoreState::hSafe:
				return "Safe";
			case VoreState::hLethal:
				return "Lethal";
			default:
				return "Error!";
			}
		}

		static std::string_view GetLocStr(Locus loc)
		{
			switch (loc) {
			case Locus::lStomach:
				return "Oral";
			case Locus::lBowel:
				return "Anal";
			case Locus::lBreastl:
				return "Breast L";
			case Locus::lBreastr:
				return "Breast R";
			case Locus::lWomb:
				return "Unbirth";
			case Locus::lBalls:
				return "Cock";
			case Locus::lNone:
				return "Any";
			default:
				return "???";
			}
		}

		static bool hasC(RE::Actor* character = nullptr)
		{
			if (!character) {
				character = RE::PlayerCharacter::GetSingleton();
			}

			RE::TESNPC* actorbase = character->GetActorBase();
			if (actorbase->GetSex() == RE::SEX::kMale) {
				return true;
			} else {
				return false;
			}
		}

		static bool hasB(RE::Actor* character = nullptr)
		{
			if (!character) {
				character = RE::PlayerCharacter::GetSingleton();
			}

			RE::TESNPC* actorbase = character->GetActorBase();
			if (actorbase->GetSex() == RE::SEX::kFemale) {
				return true;
			} else {
				return false;
			}
		}

		static bool hasV(RE::Actor* character = nullptr)
		{
			if (!character) {
				character = RE::PlayerCharacter::GetSingleton();
			}

			RE::TESNPC* actorbase = character->GetActorBase();
			if (actorbase->GetSex() == RE::SEX::kFemale) {
				return true;
			} else {
				return false;
			}
		}

		static Locus LIter(Locus loc, bool allowAll)
		{
			switch (loc) {
			case Locus::lNone:
				return Locus::lStomach;
			case Locus::lStomach:
				return Locus::lBowel;
			case Locus::lBowel:
				if (hasB())
					return Locus::lBreastl;
				else if (hasV())
					return Locus::lWomb;
				else if (hasC())
					return Locus::lBalls;
				else if (allowAll)
					return Locus::lNone;
				else
					return Locus::lStomach;
			case Locus::lBreastl:
				if (VoreSettings::dual_boobs)
					return Locus::lBreastr;
				else if (hasV())
					return Locus::lWomb;
				else if (hasC())
					return Locus::lBalls;
				else if (allowAll)
					return Locus::lNone;
				else
					return Locus::lStomach;
			case Locus::lBreastr:
				if (hasV())
					return Locus::lWomb;
				else if (hasC())
					return Locus::lBalls;
				else if (allowAll)
					return Locus::lNone;
				else
					return Locus::lStomach;
			case Locus::lWomb:
				if (hasC())
					return Locus::lBalls;
				else if (allowAll)
					return Locus::lNone;
				else
					return Locus::lStomach;
			case Locus::lBalls:
				if (allowAll)
					return Locus::lNone;
				else
					return Locus::lStomach;
			default:
				return Locus::lStomach;
			}
		}
	};
}