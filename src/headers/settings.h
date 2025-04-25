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
		static inline uint32_t k_swallow = 12;

		// ui debug

		static inline bool ui_show_struggle_sliders = false;
		static inline bool ui_show_wg = true;
		static inline bool ui_show_more = true;

		// main

		static inline bool dual_boobs = true;
		static inline bool digest_protected = true;
		static inline bool digest_essential = true;
		static inline bool swallow_auto = false;
		static inline bool companion_disposal = true;
		static inline float size_softcap = 700.0f;
		static inline float size_softcap_power = 0.45f;
		static inline int gain_stats_base = 1;
		static inline float gain_skill_base = 1;
		// pred xp per 1 hp damage or healing
		static inline float gain_pd_digestion_live = 4.0f;
		// pred xp per 1 indigestion dealt to pred (multiplied by preY level / 10)
		static inline float gain_pd_struggling = 6.0f;
		// pred xp per 1 weight digested or reformed
		static inline float gain_pd_digestion_dead = 300.0f;
		// pred xp per 1 second of endo per 1 weight, multiplied by physical activities
		static inline float gain_pd_endo = 0.04f;

		// prey xp per 1 hp lost
		static inline float gain_py_digestion = 12.0f;
		// prey xp per 1 stamina lost
		static inline float gain_py_struggling = 6.0f;
		// prey xp on regurgitation, multiplied by pred level / 10
		// reduced for willing release
		static inline float gain_py_release = 800.0f;

		//the idea for the pred is to eat more and more risky prey
		// the idea for the prey is to get eaten by stronger preds and survive

		// sound
		static inline bool enable_sound = true;
		static inline bool play_scream = false;
		static inline float burp_rate = 0.07f;
		static inline float gurgle_rate = 0.1f;

		//static inline double digestion_amount_base = 0.03;
		static inline double digestion_amount_base = 1.0;
		static inline double player_digest_multi = 5.0;
		static inline double struggle_amount = 5;
		static inline double struggle_stamina = 15;
		static inline double indigestion_loss = 0.5;
		static inline double acid_damage = 25;
		static inline double acid_gain = 2;
		static inline double acid_loss = 0.5;
		static inline double swallow_auto_speed = 30;
		static inline double swallow_decrease_speed = 15;
		//full tour speed
		static inline double locus_process_speed = 1;

		//wg
		static inline bool wg_allowed = true;
		static inline bool wg_player = true;
		static inline bool wg_followers = true;
		static inline bool wg_unique = true;
		static inline bool wg_other = false;
		static inline bool wg_female = true;
		static inline bool wg_male = false;
		static inline bool wg_creature = false;
		static inline double wg_fattemp = 5.0;
		static inline double wg_fatlong = 0.5;
		static inline double wg_locusgrowth = 1.0;
		static inline double wg_sizegrowth = 0.8;

		// 6 humans/hour
		// 120
		// 30/hour
		// 1/2 minute
		// 1/120 second ~ 0.008
		static inline double wg_loss_temp = 0.001;
		static inline double wg_loss_long = 0.00003;
		static inline double wg_loss_locus = 0.00006;
		static inline double wg_loss_size = 0.00008;

		// fake food
		static inline bool fake_food_player = true;
		static inline bool fake_food_team = true;

		//sliders

		//changes the graph of the function from cubic volume to linear slider increase
		static inline float slider_pow = 0.5f;
		//weight of a single person

		//max slider step per second
		static inline float slider_maxstep = 150.0f;

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//
		// if you encounter a bug where sliders freeze and stop changing, change bEnableEarlyRegistration to true in racemenu's skee.ini
		//
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		// !!!!!!!!!!!!!!!!!!!!!!!!! one person size for sliders in the belly group should be equal
		static inline std::array<std::vector<std::tuple<std::string, float, float>>, LocusSliders::NUMOFSLIDERS> sliders_bodypart_female = { { // gr1 (belly)
			{ { "Vore prey belly", 0.8f, 10.0f }, { "Giant belly up", 0.4f, 0.4f } },
			{ { "Giant belly (coldsteelj)", 1.25f, 5.0f } },
			{ { "Vore prey belly", 0.8f, 10.0f } },
			{ { "FatBelly", 2.6f, 4.0f } },
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
			{ { "Vore prey belly", 0.8f, 10.0f }, { "Giant belly up", 0.4f, 0.4f } },
			{ { "Giant belly (coldsteelj)", 1.25f, 5.0f } },
			{ { "Vore prey belly", 0.8f, 10.0f } },
			{ { "FatBelly", 2.6f, 4.0f } },
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
			{ { "Vore prey belly", 0.8f, 10.0f } },
			{ { "Vore prey belly", 0.8f, 10.0f } },
			{ { "Vore prey belly", 0.8f, 10.0f } },
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

		//distr
		//main toggles
		//static inline float dist_female = 30;
		//static inline float dist_male = 20;
		//static inline float dist_creature = 35;

		//static inline bool dist_size_influence = true;
		//// specific things
		//// previous toggles are multiplied by this percentage
		//static inline float dist_dragon = 5.0f;
		//static inline float dist_giant = 3.0f;
		//static inline float dist_creature_predator = 2.0f;
		//static inline float dist_creature_prey = 0.5f;
		//static inline float dist_vampire = 3.0f;
		//static inline float dist_daedra = 3.0f;
		//static inline float dist_robot = 0.3f;

		//static inline float dist_skeleton = 0.0f;
		//static inline float dist_ghost = 0.2f;
		//static inline float dist_undead = 0.4f;

		static inline int32_t dist_version = 0;

		static inline bool dist_f_allow = true;
		static inline bool dist_m_allow = true;
		static inline bool dist_c_allow = true;

		//female
		static inline std::array<int, 6> dist_pd_f_arg{ 10, 25, 15, 5, 3, 2 };
		static inline std::array<int, 6> dist_py_f_arg{ 15, 25, 25, 5, 5, 5 };

		static inline std::array<int, 6> dist_pd_f_bre{ 10, 15, 10, 5, 3, 2 };
		static inline std::array<int, 6> dist_py_f_bre{ 20, 20, 15, 5, 3, 2 };

		static inline std::array<int, 6> dist_pd_f_del{ 15, 25, 20, 5, 5, 5 };
		static inline std::array<int, 6> dist_py_f_del{ 20, 20, 15, 5, 5, 5 };

		static inline std::array<int, 6> dist_pd_f_hel{ 10, 25, 20, 5, 3, 2 };
		static inline std::array<int, 6> dist_py_f_hel{ 20, 20, 15, 5, 5, 5 };

		static inline std::array<int, 6> dist_pd_f_imp{ 10, 15, 10, 3, 3, 2 };
		static inline std::array<int, 6> dist_py_f_imp{ 20, 20, 15, 5, 3, 2 };

		static inline std::array<int, 6> dist_pd_f_kha{ 10, 25, 15, 5, 3, 2 };
		static inline std::array<int, 6> dist_py_f_kha{ 15, 25, 25, 5, 5, 5 };

		static inline std::array<int, 6> dist_pd_f_nor{ 10, 15, 15, 10, 3, 2 };
		static inline std::array<int, 6> dist_py_f_nor{ 15, 25, 25, 3, 2, 2 };

		static inline std::array<int, 6> dist_pd_f_orc{ 15, 25, 20, 5, 5, 5 };
		static inline std::array<int, 6> dist_py_f_orc{ 15, 25, 25, 3, 2, 2 };

		static inline std::array<int, 6> dist_pd_f_red{ 10, 15, 10, 5, 3, 2 };
		static inline std::array<int, 6> dist_py_f_red{ 20, 20, 15, 5, 3, 2 };

		static inline std::array<int, 6> dist_pd_f_wel{ 15, 25, 25, 10, 5, 5 };
		static inline std::array<int, 6> dist_py_f_wel{ 20, 20, 15, 5, 3, 2 };

		static inline std::array<int, 6> dist_pd_f_old{ 0, 0, 0, 0, 0, 0 };
		static inline std::array<int, 6> dist_py_f_old{ 20, 20, 15, 5, 3, 2 };

		static inline std::array<int, 6> dist_pd_f_vampire{ 0, 25, 25, 25, 15, 10 };
		static inline std::array<int, 6> dist_py_f_vampire{ 15, 30, 30, 15, 5, 5 };

		//male
		static inline std::array<int, 6> dist_pd_m_arg{ 10, 10, 10, 2, 2, 1 };
		static inline std::array<int, 6> dist_py_m_arg{ 15, 25, 25, 5, 5, 5 };

		static inline std::array<int, 6> dist_pd_m_bre{ 10, 5, 5, 2, 2, 1 };
		static inline std::array<int, 6> dist_py_m_bre{ 20, 20, 15, 5, 3, 2 };

		static inline std::array<int, 6> dist_pd_m_del{ 10, 5, 5, 2, 2, 1 };
		static inline std::array<int, 6> dist_py_m_del{ 20, 20, 15, 5, 5, 5 };

		static inline std::array<int, 6> dist_pd_m_hel{ 10, 5, 5, 2, 2, 1 };
		static inline std::array<int, 6> dist_py_m_hel{ 20, 20, 15, 5, 5, 5 };

		static inline std::array<int, 6> dist_pd_m_imp{ 10, 5, 5, 2, 2, 1 };
		static inline std::array<int, 6> dist_py_m_imp{ 20, 20, 15, 5, 3, 2 };

		static inline std::array<int, 6> dist_pd_m_kha{ 10, 5, 5, 2, 2, 1 };
		static inline std::array<int, 6> dist_py_m_kha{ 15, 25, 25, 5, 5, 5 };

		static inline std::array<int, 6> dist_pd_m_nor{ 10, 10, 10, 2, 2, 1 };
		static inline std::array<int, 6> dist_py_m_nor{ 15, 25, 25, 3, 2, 2 };

		static inline std::array<int, 6> dist_pd_m_orc{ 10, 15, 15, 5, 3, 2 };
		static inline std::array<int, 6> dist_py_m_orc{ 15, 25, 25, 3, 2, 2 };

		static inline std::array<int, 6> dist_pd_m_red{ 10, 5, 5, 2, 2, 1 };
		static inline std::array<int, 6> dist_py_m_red{ 20, 20, 15, 5, 3, 2 };

		static inline std::array<int, 6> dist_pd_m_wel{ 10, 15, 15, 5, 3, 2 };
		static inline std::array<int, 6> dist_py_m_wel{ 20, 20, 15, 5, 3, 2 };

		static inline std::array<int, 6> dist_pd_m_old{ 0, 0, 0, 0, 0, 0 };
		static inline std::array<int, 6> dist_py_m_old{ 20, 20, 15, 5, 3, 2 };

		static inline std::array<int, 6> dist_pd_m_vampire{ 0, 25, 25, 25, 15, 10 };
		static inline std::array<int, 6> dist_py_m_vampire{ 15, 30, 30, 15, 5, 5 };

		// creatures
		static inline std::array<int, 6> dist_pd_c_4{ 0, 10, 15, 25, 25, 25 };
		static inline std::array<int, 6> dist_py_c_4{ 50, 25, 10, 5, 5, 3 };
		static inline std::array<int, 6> dist_pd_c_3{ 5, 25, 20, 15, 10, 5 };
		static inline std::array<int, 6> dist_py_c_3{ 15, 30, 30, 15, 5, 5 };
		static inline std::array<int, 6> dist_pd_c_2{ 10, 15, 10, 5, 3, 2 };
		static inline std::array<int, 6> dist_py_c_2{ 20, 20, 15, 5, 3, 2 };
		static inline std::array<int, 6> dist_pd_c_1{ 10, 10, 5, 3, 2, 1 };
		static inline std::array<int, 6> dist_py_c_1{ 10, 10, 5, 2, 2, 1 };
		static inline std::array<int, 6> dist_pd_c_0{ 10, 5, 2, 1, 1, 1 };
		static inline std::array<int, 6> dist_py_c_0{ 10, 10, 5, 2, 2, 1 };

		static inline std::array<int, 6> dist_pd_vampire{ 0, 25, 25, 25, 15, 10 };
		static inline std::array<int, 6> dist_py_vampire{ 15, 30, 30, 15, 5, 5 };

		static inline std::array<int, 6> dist_pd_daedra{ 0, 25, 25, 25, 15, 10 };
		static inline std::array<int, 6> dist_py_daedra{ 15, 30, 30, 15, 5, 5 };

		static inline std::array<int, 6> dist_pd_dragon{ 0, 0, 0, 25, 30, 45 };
		static inline std::array<int, 6> dist_py_dragon{ 50, 25, 10, 5, 5, 3 };

		static inline std::array<int, 6> dist_pd_skeleton{ 0, 0, 0, 0, 0, 0 };
		static inline std::array<int, 6> dist_py_skeleton{ 15, 30, 30, 15, 5, 5 };

		static inline std::array<int, 6> dist_pd_robot{ 10, 5, 5, 0, 0, 0 };
		static inline std::array<int, 6> dist_py_robot{ 15, 30, 30, 15, 5, 5 };


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

		static inline RE::FormID player_camera_owner{};
	};

	class PlayerPrefs
	{
	public:
		static inline Locus voreLoc = Locus::lStomach;
		static inline Locus regLoc = Locus::lNone;
		static inline VoreDataEntry::VoreState voreType = VoreDataEntry::VoreState::hSafe;

		static void clear()
		{
			voreLoc = Locus::lStomach;
			regLoc = Locus::lNone;
			voreType = VoreDataEntry::VoreState::hSafe;
		}

		static std::string_view GetTypeStr(VoreDataEntry::VoreState vType)
		{
			switch (vType) {
			case VoreDataEntry::VoreState::hSafe:
				return "Safe";
			case VoreDataEntry::VoreState::hLethal:
				return "Lethal";
			case VoreDataEntry::VoreState::hReformation:
				return "Healing";
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
			case Locus::lWomb:
				return "Unbirth";
			case Locus::lBreastl:
				return "Breast L";
			case Locus::lBreastr:
				return "Breast R";
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
				if (hasV())
					return Locus::lWomb;
				else if (hasB())
					return Locus::lBreastl;
				else if (hasC())
					return Locus::lBalls;
				else if (allowAll)
					return Locus::lNone;
				else
					return Locus::lStomach;
			case Locus::lWomb:
				if (hasB())
					return Locus::lBreastl;
				else if (hasC())
					return Locus::lBalls;
				else if (allowAll)
					return Locus::lNone;
				else
					return Locus::lStomach;
			case Locus::lBreastl:
				if (VoreSettings::dual_boobs)
					return Locus::lBreastr;
				else if (hasC())
					return Locus::lBalls;
				else if (allowAll)
					return Locus::lNone;
				else
					return Locus::lStomach;
			case Locus::lBreastr:
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