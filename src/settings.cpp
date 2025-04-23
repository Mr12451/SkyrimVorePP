#include "headers/settings.h"

namespace cini = clib_util::ini;

namespace Vore
{

	static std::string_view GetLocusName(LocusSliders slider)
	{
		switch (slider) {
		case Vore::uStomach:
			return "Stomach";
		case Vore::uBowel:
			return "Bowel";
		case Vore::uWomb:
			return "Womb";
		case Vore::uFatBelly:
			return "Fat Belly";
		case Vore::uThroat:
			return "Throat";
		case Vore::uBreastl:
			return "Breast L";
		case Vore::uBreastr:
			return "Breast R";
		case Vore::uCock:
			return "Cock";
		case Vore::uBalls:
			return "Balls";
		case Vore::uFatAss:
			return "Fat Ass";
		case Vore::uFatBreasts:
			return "Fat Breast";
		case Vore::uFatCock:
			return "Big Cock";
		case Vore::uFatLow:
			return "Negative Fat";
		case Vore::uFatHigh:
			return "Positive Fat";
		case Vore::uGrowthLow:
			return "N Long Fat";
		case Vore::uGrowthHigh:
			return "P Long Fat";
		}
		return "Error";
	}

	static bool IsFloat(std::string& s)
	{
		std::istringstream iss(s);
		float dummy;
		iss >> std::noskipws >> dummy;
		return iss && iss.eof();
	}

	template <std::size_t SIZE>
	static void ReadLocusSliders(std::array<std::vector<std::tuple<std::string, float, float>>, SIZE>& bType, const char* bName, CSimpleIniA& ini, bool allowLog = false)
	{
		for (uint8_t i = 0; i < SIZE; i++) {
			std::vector<std::string> lSliders = {};
			for (auto& [name, one, max] : bType[i]) {
				lSliders.push_back(name);
				lSliders.push_back(std::format("{:6f}", one));
				lSliders.push_back(std::format("{:6f}", max));
			}
			std::string lName = bName;
			if constexpr (SIZE == LocusSliders::NUMOFSLIDERS) {
				lName += GetLocusName((LocusSliders)i);

			} else if constexpr (SIZE == Locus::NUMOFLOCI) {
				lName += PlayerPrefs::GetLocStr((Locus)i);
			} else {
				lName += std::to_string(i);
			}
			cini::get_value(ini, lSliders, "Sliders", lName.c_str(), "; ");

			std::vector<std::tuple<std::string, float, float>> newLSliders = {};

			bool foundErrors = false;

			for (size_t j = 0; j < lSliders.size(); j += 3) {
				if (j + 2 < lSliders.size() && IsFloat(lSliders[j + 1]) && IsFloat(lSliders[j + 2])) {
					float one = std::stof(lSliders[j + 1]);
					float max = std::stof(lSliders[j + 2]);
					newLSliders.push_back({ lSliders[j], one, max });
				} else {
					foundErrors = true;
					break;
				}
			}
			if (!foundErrors) {
				bType[i] = newLSliders;
			}

		}
		if (allowLog) {

			flog::info("sliders {}", bName);
			int i = 0;
			for (auto& loc : bType) {
				flog::info("Loc {}", i);
				for (auto& [name, one, max] : loc) {
					flog::info("name {}, one {}, max {}", name, one, max);
				}
				i++;
			}
		}
	}

	static void ReadDistChance(std::array<int, 6>& dist, CSimpleIniA& ini, const char* aName, const char* desc = ";")
	{
		std::vector<std::string> vect{};
		for (int i = 0; i < 6; i++) {
			vect.push_back(std::format("{}", dist[i]));
		}
		cini::get_value(ini, vect, "Distribution", aName, desc);

		for (int i = 0; i < 6; i++) {
			dist[i] = std::stoi(vect[i]);
		}
	}

	void VoreSettings::LoadIniSettings()
	{
		constexpr auto path = L"Data/SKSE/Plugins/SkyrimVorePP.ini";
		CSimpleIniA ini;
		ini.SetUnicode();
		SI_Error er = ini.LoadFile(path);
		if (er < 0) {
			flog::warn("Reading settings file error!");
		}

		static const char* section0 = "Updates";

		cini::get_value(ini, slow_update, section0, "Slow Update", ";How often are dead or inactive prey updated (in seconds)");
		cini::get_value(ini, fast_update, section0, "Fast Update", ";How often are living prey updated (in seconds)");
		cini::get_value(ini, belly_fast_update, section0, "Slider Update", ";How often are belly sliders updated (in seconds)\n;Lowering this value will increase the smoothness but reduce performance");

		static const char* section1 = "Keybinds";

		cini::get_value(ini, k_vore_key, section1, "Vore Key", ";Find keycodes here https://ck.uesp.net/wiki/Input_Script\n;Or here https://github.com/Mr12451/SkyrimVorePP/blob/main/help/KeyCodesSkyrim.txt\n;Keys used by Skyrim https://github.com/Mr12451/SkyrimVorePP/blob/main/help/Kbd-elder-scrolls-v-the-skyrim.png\n;Swallow key for any vore type");
		cini::get_value(ini, k_regurg_key, section1, "Regurgitation", ";Release prey key, also used for disposal.");
		cini::get_value(ini, k_i_menu, section1, "Info menu", ";Shows targeted character's stats. If no character is selected, shows player's stats");
		cini::get_value(ini, k_sw_menu, section1, "Vore preferences menu", ";Menu for choosing swallow/release type and lethality");
		cini::get_value(ini, k_menu_1, section1, "Menu key 1", ";Key used for navigating the menus");
		cini::get_value(ini, k_menu_2, section1, "Menu key 2", ";Key used for navigating the menus");
		cini::get_value(ini, k_menu_3, section1, "Menu key 3", ";Key used for navigating the menus");
		cini::get_value(ini, k_menu_4, section1, "Menu key 4", ";Key used for navigating the menus (rarely used)");
		cini::get_value(ini, k_test, section1, "Debug info key", ";Prints Vore Data to the log file");
		cini::get_value(ini, ui_show_struggle_sliders, section1, "Show struggle slider info", ";(Debug) Shows struggle slider values in info menu");
		cini::get_value(ini, ui_show_wg, section1, "Show weight gain info", ";(Debug) Shows weight gain stats in info menu");
		cini::get_value(ini, ui_show_more, section1, "Show more info", ";(Debug) Shows more info in menus. DOES NOTHING RIGHT NOW");

		static const char* section2 = "Main";

		cini::get_value(ini, dual_boobs, section2, "Dual breasts mode", ";Each breast can be used separately. If you set this to false, manually add struggle sliders from both breasts to both breasts");
		cini::get_value(ini, digest_protected, section2, "Digest protected", ";Protected NPCs can be digested");
		cini::get_value(ini, digest_essential, section2, "Digest essential", ";Essential NPCs can be digested. Fun, but can softlock the game");
		cini::get_value(ini, swallow_auto, section2, "Auto swallow", ";Disables swallow minigame. DON'T DISABLE, SWALLOW MINIGAME IS NOT IMPLEMENTED YET!");
		cini::get_value(ini, companion_disposal, section2, "Auto disposal companions", ";Conpanions will automatically use disposal when they finish digesting a prey");
		cini::get_value(ini, size_softcap, section2, "Size softcap", ";Default human size is 100\n;NPCs whose size is bigger than the softcap will have their size reduced exponentially");
		cini::get_value(ini, size_softcap_power, section2, "Size softcap power", ";Power for size softcap. The lower this is, the harder the softcap is. Set to 1 to disable\n;If this is disabled, the size of a dragon will be 300 times that of a human, which isn't good for the gameplay");
		cini::get_value(ini, gain_stats_base, section2, "Stat gain", ";Gain stats when finishing digestion. You will get this amount of the highest pred's stat (compared to yours) that's higher than your stat.\n;Set to 0 to disable.");
		cini::get_value(ini, gain_skill_base, section2, "Skill gain", ";Same as Stat gain but for skills");
		cini::get_value(ini, gain_pd_digestion_live, section2, "Pred XP 1", ";pred xp per 1 damage dealt to prey during digestion");
		cini::get_value(ini, gain_pd_struggling, section2, "Pred XP 2", ";pred xp per 1 indigestion dealt by prey (multiplied by prey level / 10)");
		cini::get_value(ini, gain_pd_digestion_dead, section2, "Pred XP 3", ";pred xp per 100 weight digested or reformed");
		cini::get_value(ini, gain_pd_endo, section2, "Pred XP 4", ";pred xp per 1 second of having 1 size of prey in stomach (human prey is 100 in size), increased during physical activities");
		cini::get_value(ini, gain_py_digestion, section2, "Prey XP 1", ";prey xp per 1 hp lost during digestion");
		cini::get_value(ini, gain_py_struggling, section2, "Prey XP 2", ";prey xp per 1 stamina lost when struggling");
		cini::get_value(ini, gain_py_release, section2, "Prey XP 3", ";prey xp when released. When prey is willing or pred is player and they released the prey the XP will be divided by 10.\n;If vore is non-lethal the XP will be divided by 2.5.\n;These XP reductions don't stack");

		static const char* sectionS = "Sound";
		cini::get_value(ini, enable_sound, sectionS, "Enable Sound", ";Enables sounds. Works only if you have devourment dummy plugin installed, otherwise does nothing");
		cini::get_value(ini, play_scream, sectionS, "Play Scream", ";Plays prey scream when they die. Loud and off-putting, not recommended. The screamless version is much better");
		cini::get_value(ini, burp_rate, sectionS, "Burp Rate", ";Chance to burp each slow update with human-sized prey inside stomach.");
		cini::get_value(ini, gurgle_rate, sectionS, "Gurgle Rate", ";Same as above");

		static const char* section3 = "Process";

		cini::get_value(ini, digestion_amount_base, section3, "Digestion speed", ";All values in this category are PER SECOND\n;Base digestion speed for dead prey. Actual speed scales with prey size");
		cini::get_value(ini, player_digest_multi, section3, "Player digestion multi", ";Dead player will be digested this times faster");
		cini::get_value(ini, struggle_amount, section3, "Struggle amount", ";Indigestion gain rate");
		cini::get_value(ini, struggle_stamina, section3, "Struggle stamina", ";Prey stamina loss during struggling");
		cini::get_value(ini, indigestion_loss, section3, "Indigestion loss", ";Pred indigestion loss");
		cini::get_value(ini, acid_damage, section3, "Digestion damage", ";Base digestion damage at max acid level");
		cini::get_value(ini, acid_gain, section3, "Acid gain", ";How fast acid levels increase (per organ) when digestion damage is applied to a prey\n;The more prey you have, the faster the acid rises");
		cini::get_value(ini, acid_loss, section3, "Acid loss", ";How fast acid levels decrease when no digestion of living prey happens");
		cini::get_value(ini, swallow_auto_speed, section3, "Auto swallowing speed", ";Swallow process speed when auto swallowing is enabled (for human-sized prey)");
		cini::get_value(ini, swallow_decrease_speed, section3, "Swallow decrease", ";Decrease of swallow process during the swallow minigame SWALLOW MINIGAME IS NOT IMPLEMENTED YET!");
		cini::get_value(ini, locus_process_speed, section3, "Full tour speed", ";Full tour speed (percentage)");

		static const char* section4 = "Weight Gain";

		cini::get_value(ini, wg_allowed, section4, "Weight Gain", ";Allows all forms of weight or size gain");
		cini::get_value(ini, wg_player, section4, "Player WG", ";Allows wg for player");
		cini::get_value(ini, wg_followers, section4, "Followers WG", ";Allows wg for followers and recruitable NPCs");
		cini::get_value(ini, wg_unique, section4, "Unique NPCs WG", ";Allows wg for unique NPCs");
		cini::get_value(ini, wg_other, section4, "WG for everyone else", ";Allows wg for every other actor. NOT RECOMMENDED, may cause lag (untested)");
		cini::get_value(ini, wg_female, section4, "WG for females", ";Allows wg for female humanoids. Works on top of other toggles\n;Example: if player is female and Player WG is on, but this is off, no wg will happen");
		cini::get_value(ini, wg_male, section4, "WG for males", ";Allows wg for male humanoids. Works on top of other toggles");
		cini::get_value(ini, wg_creature, section4, "WG for creatures", ";Allows wg for non humanoids. Works on top of other toggles");
		
		cini::get_value(ini, wg_fattemp, section4, "Fast weight gain", ";All values in this category are PER SECOND\n;Short-term weight gain. You gain it much faster, but doing physical activities will burn it faster");
		cini::get_value(ini, wg_fatlong, section4, "Slow weight gain", ";You gain this weight type slower, and doing physical activities has less effect on it.");
		cini::get_value(ini, wg_locusgrowth, section4, "Body part WG", ";Weight gain for a specific body part (depends on vore type). Only happens during vore - item consumption does not affect this");
		cini::get_value(ini, wg_sizegrowth, section4, "Size gain", ";Pred height increase. Only happens during vore - item consumption does not affect this");
		cini::get_value(ini, wg_loss_temp, section4, "Weight loss (temp)", ";");
		cini::get_value(ini, wg_loss_long, section4, "Weight loss (long)", ";");
		cini::get_value(ini, wg_loss_locus, section4, "Weight loss (body part)", ";");
		cini::get_value(ini, wg_loss_size, section4, "Size loss", ";");


		static const char* sectionFF = "Fake Food";

		cini::get_value(ini, fake_food_player, sectionFF, "Add fake food (player)", ";Consuming an item will add a fake food item to the stomach. Basically realistic food digestion");
		cini::get_value(ini, fake_food_team, sectionFF, "Add fake food (followers)", ";");

		static const char* section5 = "Sliders";

		cini::get_value(ini, slider_pow, section5, "Size to volume power", ";Power for turning linear sum of prey sizes into volume. The default value looks good enough");
		cini::get_value(ini, slider_maxstep, section5, "Slider max step", ";Max slider change per second when slider goal - current value = Default prey size. Speed scales with diff");

		int pad0 = 0;
		cini::get_value(ini, pad0, section5, "pad0", ";pad values don't do anything, they're for inserting comments\n;Below is a list of all sliders for each body and slider type\n;Each slider has 3 values: Name (same as in the Outfit studio, not the one from Bodyslide), One (value for one default-sized prey), Max (max value)\n;Values here are divided by 100 compared to Bodyslide and outfit studio\n;You can add as many sliders as you want, but it will reduce performance");

		ReadLocusSliders(sliders_bodypart_female, "Female ", ini);
		ReadLocusSliders(sliders_bodypart_male, "Male ", ini);
		ReadLocusSliders(sliders_bodypart_creature, "Creature ", ini);
		ReadLocusSliders(struggle_sliders, "Struggle ", ini);


		int pad1 = 0;
		cini::get_value(ini, pad1, section5, "pad1", ";WG coefficient for different body parts from different vore types.\n;In order: Belly, Ass, Breasts, Cock\n;If prey died in one organ, but then was moved to a different one, the original one is used.");

		for (uint8_t i = 0; i < Locus::NUMOFLOCI; i++) {
			std::vector<std::string> lSliders = {};
			for (uint8_t j = 0; j < 4; j++) {
				lSliders.push_back(std::format("{:6f}", voretypes_partgain[i][j]));
			}
			std::string lName = "WG Parts ";

			lName += PlayerPrefs::GetLocStr((Locus)i);

			cini::get_value(ini, lSliders, section5, lName.c_str(), "; ");

			std::array<double, 4> newLSliders = {};


			if (lSliders.size() == 4) {
				for (uint8_t j = 0; j < 4; j++) {
					newLSliders[j] = std::stod(lSliders[j]);
				}
				voretypes_partgain[i] = newLSliders;
			}
		}
		bool allowLog = false;
		if (allowLog) {
			int i = 0;
			for (auto& loc : voretypes_partgain) {
				flog::info("Loc {}", i);
				for (auto& el : loc) {
					flog::info("{}", el);
				}
				i++;
			}
		}

		static const char* section6 = "Distribution";

		cini::get_value(ini, dist_version, section6, "D_Version", ";The version of distributed vore stats. Change this to a number between 0 and 127 to reset all vore stats for all npcs and player. Changing this to values outside of the range will break the game\n;Use this if you want to redistribute vore stats.\n;If you had version 0 and then changed to 1, and then back to 0, it will redistribute stats once again, it doesn't keep previous version info.");
		cini::get_value(ini, dist_f_allow, section6, "D_FemalePreds", ";Allows females to be chosen as preds");
		cini::get_value(ini, dist_m_allow, section6, "D_MalePreds", ";Allows males to be chosen as preds");
		cini::get_value(ini, dist_c_allow, section6, "D_CreaturePreds", ";Allows non-humanoid npcs to be chosen as preds");

		ReadDistChance(dist_pd_f_arg, ini, "Argonian_Female_Pred", ";Below is a section for configuarting the probability of characters of a certain race and sex becoming a pred or a prey of a certain rank.\n;There are 7 ranks:\n;For preds:\n;rank 0 - can't do vore, can only swallow small items\n;rank 1 - can swallow small creatures\n;rank 2 - can swallow a single human-sized prey\n;rank 3 - can swallow 3 humans\n;rank 4 - 10 humans\n;rank 5 - 20 humans\n;rank 6 - no limit\n;These ranks correspond to leveling pred/prey skill from 0 to 100. When an npc recieves the rank, they will also get perks according to their rank/vore level.\n;The options below configurate how likely an npc is to get a certain rank, starting from rank 1 to rank 6. In other cases the npc will get rank 0.\n;So the chance to become a non-pred is 100 minus the sum of all the chances to become a pred\n;So, a string like 25|10|5|5|3|2 means that an npc has 25% chance to become a rank 1 pred, 10% chance to become a rank 2 pred, etc. And 100-(25+10+5+5+3+2) = 50% to not become a pred");

		ReadDistChance(dist_py_f_arg, ini, "Argonian_Female_Prey");
		ReadDistChance(dist_pd_f_bre, ini, "Breton_Female_Pred");
		ReadDistChance(dist_py_f_bre, ini, "Breton_Female_Prey");
		ReadDistChance(dist_pd_f_del, ini, "DarkElf_Female_Pred");
		ReadDistChance(dist_py_f_del, ini, "DarkElf_Female_Prey");
		ReadDistChance(dist_pd_f_hel, ini, "HighElf_Female_Pred");
		ReadDistChance(dist_py_f_hel, ini, "HighElf_Female_Prey");
		ReadDistChance(dist_pd_f_imp, ini, "Imperial_Female_Pred");
		ReadDistChance(dist_py_f_imp, ini, "Imperial_Female_Prey");
		ReadDistChance(dist_pd_f_kha, ini, "Khajiit_Female_Pred");
		ReadDistChance(dist_py_f_kha, ini, "Khajiit_Female_Prey");
		ReadDistChance(dist_pd_f_nor, ini, "Nord_Female_Pred");
		ReadDistChance(dist_py_f_nor, ini, "Nord_Female_Prey");
		ReadDistChance(dist_pd_f_orc, ini, "Orc_Female_Pred");
		ReadDistChance(dist_py_f_orc, ini, "Orc_Female_Prey");
		ReadDistChance(dist_pd_f_red, ini, "Redguard_Female_Pred");
		ReadDistChance(dist_py_f_red, ini, "Redguard_Female_Prey");
		ReadDistChance(dist_pd_f_wel, ini, "WoodElf_Female_Pred");
		ReadDistChance(dist_py_f_wel, ini, "WoodElf_Female_Prey");
		ReadDistChance(dist_pd_f_old, ini, "OldPeopleRace_Female_Pred");
		ReadDistChance(dist_py_f_old, ini, "OldPeopleRace_Female_Prey");
		ReadDistChance(dist_pd_f_vampire, ini, "Vampire_Female_Pred", ";Also for werewolves (companions)");
		ReadDistChance(dist_py_f_vampire, ini, "Vampire_Female_Prey");

		ReadDistChance(dist_pd_m_arg, ini, "Argonian_Male_Pred", ";Male Npcs\n;");
		ReadDistChance(dist_py_m_arg, ini, "Argonian_Male_Prey");
		ReadDistChance(dist_pd_m_bre, ini, "Breton_Male_Pred");
		ReadDistChance(dist_py_m_bre, ini, "Breton_Male_Prey");
		ReadDistChance(dist_pd_m_del, ini, "DarkElf_Male_Pred");
		ReadDistChance(dist_py_m_del, ini, "DarkElf_Male_Prey");
		ReadDistChance(dist_pd_m_hel, ini, "HighElf_Male_Pred");
		ReadDistChance(dist_py_m_hel, ini, "HighElf_Male_Prey");
		ReadDistChance(dist_pd_m_imp, ini, "Imperial_Male_Pred");
		ReadDistChance(dist_py_m_imp, ini, "Imperial_Male_Prey");
		ReadDistChance(dist_pd_m_kha, ini, "Khajiit_Male_Pred");
		ReadDistChance(dist_py_m_kha, ini, "Khajiit_Male_Prey");
		ReadDistChance(dist_pd_m_nor, ini, "Nord_Male_Pred");
		ReadDistChance(dist_py_m_nor, ini, "Nord_Male_Prey");
		ReadDistChance(dist_pd_m_orc, ini, "Orc_Male_Pred");
		ReadDistChance(dist_py_m_orc, ini, "Orc_Male_Prey");
		ReadDistChance(dist_pd_m_red, ini, "Redguard_Male_Pred");
		ReadDistChance(dist_py_m_red, ini, "Redguard_Male_Prey");
		ReadDistChance(dist_pd_m_wel, ini, "WoodElf_Male_Pred");
		ReadDistChance(dist_py_m_wel, ini, "WoodElf_Male_Prey");
		ReadDistChance(dist_pd_m_old, ini, "OldPeopleRace_Male_Pred");
		ReadDistChance(dist_py_m_old, ini, "OldPeopleRace_Male_Prey");
		ReadDistChance(dist_pd_m_vampire, ini, "Vampire_Male_Pred", ";Also for werewolves (companions)");
		ReadDistChance(dist_py_m_vampire, ini, "Vampire_Male_Prey");

		ReadDistChance(dist_pd_vampire, ini, "Vampire_Pred", ";The next section is for non-humanoid creatures\n;");
		ReadDistChance(dist_py_vampire, ini, "Vampire_Prey");
		ReadDistChance(dist_pd_daedra, ini, "Daedra_Pred");
		ReadDistChance(dist_py_daedra, ini, "Daedra_Prey");
		ReadDistChance(dist_pd_dragon, ini, "Dragon_Pred");
		ReadDistChance(dist_py_dragon, ini, "Dragon_Prey");
		ReadDistChance(dist_pd_skeleton, ini, "Skeleton_Pred", ";Also includes ghosts and whisps");
		ReadDistChance(dist_py_skeleton, ini, "Skeleton_Prey");
		ReadDistChance(dist_pd_robot, ini, "Robot_Pred", ";Dwarven constructs");
		ReadDistChance(dist_py_robot, ini, "Robot_Prey");

		ReadDistChance(dist_pd_c_0, ini, "Tiny_Pred", ";The next section is for every creature not included above.\n;It's not actually based on the size, but on the base race health, which is pretty close to creature's actual size/pred abilities according to logic\n;Tiny creatures are those with less than 12 starting health, like hares and chicken");
		ReadDistChance(dist_py_c_0, ini, "Tiny_Prey");
		ReadDistChance(dist_pd_c_1, ini, "Small_Pred", ";Small creatures are those with less than 40 starting health, like wolves");
		ReadDistChance(dist_py_c_1, ini, "Small_Prey");
		ReadDistChance(dist_pd_c_2, ini, "Medium_Pred", ";Medium creatures are those with less than 101 starting health, like humanoids");
		ReadDistChance(dist_py_c_2, ini, "Medium_Prey");
		ReadDistChance(dist_pd_c_3, ini, "Large_Pred", ";Large creatures are those with less than 300 starting health, like trolls and bears");
		ReadDistChance(dist_py_c_3, ini, "Large_Prey");
		ReadDistChance(dist_pd_c_4, ini, "Huge_Pred", ";Large creatures are those with more than 300 starting health, like dragons and mammoths");
		ReadDistChance(dist_py_c_4, ini, "Huge_Prey");

		(void)ini.SaveFile(path);
	}

}
