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
		cini::get_value(ini, size_softcap, section2, "Size softcap", ";Default human size is 100\n;NPCs whose size is bigger than the softcap will have their size reduced exponentially");
		cini::get_value(ini, size_softcap_power, section2, "Size softcap power", ";Power for size softcap. The lower this is, the harder the softcap is. Set to 1 to disable\n;If this is disabled, the size of a dragon will be 300 times that of a human, which isn't good for the gameplay");

		static const char* section3 = "Process";

		cini::get_value(ini, digestion_amount_base, section3, "Digestion speed", ";All values in this category are PER SECOND\n;Base digestion speed for dead prey. Actual speed scales with prey size");
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

		cini::get_value(ini, wg_fattemp, section4, "Temp weight gain", ";All values in this category are PER SECOND\n;Short-term weight gain");
		cini::get_value(ini, wg_fatlong, section4, "Long-term weight gain", ";");
		cini::get_value(ini, wg_locusgrowth, section4, "Body part WG", ";Weight gain for a specific body part (depends on vore type)");
		cini::get_value(ini, wg_sizegrowth, section4, "Size gain", ";Pred height increase.");
		cini::get_value(ini, wg_loss_temp, section4, "Weight loss (temp)", ";");
		cini::get_value(ini, wg_loss_long, section4, "Weight loss (long)", ";");
		cini::get_value(ini, wg_loss_locus, section4, "Weight loss (body part)", ";");
		cini::get_value(ini, wg_loss_size, section4, "Size loss", ";");

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

		(void)ini.SaveFile(path);
	}

}
