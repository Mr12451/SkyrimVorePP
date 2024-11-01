#pragma once
#include "headers/racemenu.h"
#include "headers/voredata.h"

namespace Vore
{
	class VoreSettings
	{
		//settings will be loaded here
	public:
		static inline uint32_t endo_key = 59;
		static inline uint32_t vore_key = 60;
		static inline uint32_t heal_key = 61;
		static inline uint32_t regurg_key = 62;
		static inline uint32_t check_time_key = 64;

		static inline double slow_update = 5;
		static inline double fast_update = 0.2;
		static inline float belly_fast_update = 0.05f;

		//change this to a lower value, about ~0.1
		static inline double locus_process_speed = 10;

		static inline double acid_damage = 5;
		static inline double digestion_amount_base = 5;
		static inline double struggle_amount = 5;
		static inline double struggle_stamina = 10;
		static inline double indigestion_loss = 1;

		//wg
		static inline double wg_fattemp = 0.2;
		static inline double wg_fatlong = 0.075;
		static inline double wg_locusgrowth = 0.1;
		static inline double wg_sizegrowth = 0.075;

		// 6 humans/hour
		// 120
		// 30/hour
		// 1/2 minute
		// 1/120 second ~ 0.008
		static inline double wg_loss_temp = 0.008;
		static inline double wg_loss_long = 0.001;
		static inline double wg_loss_locus = 0.001;
		static inline double wg_loss_size = 0.001;

		//enabling this disables qte for swallow
		static inline bool swallow_auto = true;
		static inline double swallow_auto_speed = 30;
		static inline double swallow_decrease_speed = 20;

		//sliders

		//changes the graph of the function from cubic volume to linear slider increase
		static inline float slider_pow_divider = 2.0f;
		//weight of a single person
		static inline float slider_100 = 100.0f;
		//max slider step per second
		static inline float slider_maxstep = 100.0f;
		static inline float slider_struggle_maxstep = 20.0f;

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
		static inline std::array<std::vector<std::tuple<const char*, float, float>>, LocusSliders::NUMOFSLIDERS> sliders_bodypart_female = { { //vore
			{ { "Vore prey belly", 0.7f, 10.0f }, { "Giant belly up", 0.4f, 0.4f } },
			{ { "ChestDepth", 3.0f, 6.0f } },
			{ { "Giant belly (coldsteelj)", 1.0f, 5.0f } },
			{ { "BVoreL", 1.0f, 10.0f } },
			{ { "BVoreR", 1.0f, 10.0f } },
			{ { "Vore prey belly", 0.7f, 10.0f } },
			{ { "SOS - SchlongLength", 4.0f, 10.0f }, { "SOS - SchlongGirth", 4.0f, 10.0f }, { "SOS - GlansSize", -1.6f, -4.0f }, { "SOS - GlansMushroom", 0.8f, 2.0f } },
			{ { "CVore", 1.0f, 10.0f } },
			//wg
			{ { "FatBelly", 1.0f, 4.0f } },
			{ { "ChubbyLegs", 0.25f, 2.5f }, { "Thighs", 0.5f, 5.0f }, { "AppleCheeks", 1.0f, 10.0f }, { "Butt", 1.0f, 10.0f }, { "BigButt", 1.0f, 10.0f }, { "ChubbyButt", 1.0f, 10.0f } },
			{ { "BreastsNewSH", 1.0f, 4.0f }, { "Breasts", 1.0f, 4.0f }, { "DoubleMelon", 0.25f, 1.0f }, { "BreastWidth", 1.0f, 4.0f } },
			{ { "SOS - SchlongLength", 1.0f, 4.0f }, { "SOS - SchlongGirth", 1.0f, 4.0f }, { "SOS - GlansSize", -0.4f, -4.0f }, { "SOS - GlansMushroom", 0.2f, 0.8f }, { "SOS - BallsSize", 1.0f, 4.0f },
				{ "SOS - BallsGravity", 1.0f, 4.0f }, { "SOS - BallsSpherify", 0.5f, 2.0f } },
			//fat
			{ { "SSBBW3 body", 1.0f, 4.0f } },
			{ { "SSBBW3 body", 1.0f, 4.0f } },
			{ { "SSBBW2 body", 1.0f, 4.0f } },
			{ { "SSBBW2 body", 1.0f, 4.0f } } } };

		static inline std::array<std::vector<std::tuple<const char*, float, float>>, LocusSliders::NUMOFSLIDERS> sliders_bodypart_male = { { //vore
			{ { "Vore prey belly", 0.7f, 10.0f }, { "Giant belly up", 0.4f, 0.4f } },
			{ { "ChestDepth", 1.0f, 2.0f } },
			{ { "Giant belly (coldsteelj)", 1.0f, 5.0f } },
			{ { "BVoreL", 1.0f, 10.0f } },
			{ { "BVoreR", 1.0f, 10.0f } },
			{ { "Vore prey belly", 0.7f, 10.0f } },
			{ { "SOS - SchlongLength", 4.0f, 10.0f }, { "SOS - SchlongGirth", 4.0f, 10.0f }, { "SOS - GlansSize", -1.6f, -4.0f }, { "SOS - GlansMushroom", 0.8f, 2.0f } },
			{ { "CVore", 1.0f, 10.0f } },
			//wg
			{ { "FatBelly", 1.0f, 4.0f } },
			{ { "ChubbyLegs", 0.25f, 2.5f }, { "Thighs", 0.5f, 5.0f }, { "AppleCheeks", 1.0f, 10.0f }, { "Butt", 1.0f, 10.0f }, { "BigButt", 1.0f, 10.0f }, { "ChubbyButt", 1.0f, 10.0f } },
			{ { "BreastsNewSH", 1.0f, 4.0f }, { "Breasts", 1.0f, 4.0f }, { "DoubleMelon", 0.25f, 1.0f }, { "BreastWidth", 1.0f, 4.0f } },
			{ { "SOS - SchlongLength", 1.0f, 4.0f }, { "SOS - SchlongGirth", 1.0f, 4.0f }, { "SOS - GlansSize", -0.4f, -4.0f }, { "SOS - GlansMushroom", 0.2f, 0.8f }, { "SOS - BallsSize", 1.0f, 4.0f },
				{ "SOS - BallsGravity", 1.0f, 4.0f }, { "SOS - BallsSpherify", 0.5f, 2.0f } },
			//fat
			{ { "SSBBW3 body", 1.0f, 4.0f } },
			{ { "SSBBW3 body", 1.0f, 4.0f } },
			{ { "SSBBW2 body", 1.0f, 4.0f } },
			{ { "SSBBW2 body", 1.0f, 4.0f } } } };

		static inline std::array<std::vector<std::tuple<const char*, float, float>>, LocusSliders::NUMOFSLIDERS> sliders_bodypart_creature = { { //vore
			{ { "Vore prey belly", 0.7f, 10.0f } },
			{},
			{ { "Vore prey belly", 0.7f, 10.0f } },
			{},
			{},
			{ { "Vore prey belly", 0.7f, 10.0f } },
			{},
			{ { "CVore", 1.0f, 10.0f } },
			//wg
			{},
			{},
			{},
			{},
			//fat
			{},
			{},
			{},
			{} } };

		static inline std::array<std::vector<std::tuple<const char*, float, float>>, Locus::NUMOFLOCI> struggle_sliders = { { 
			{ { "StruggleSlider1", 1.0f, 5.0f }, { "StruggleSlider2", 1.0f, 5.0f }, { "StruggleSlider3", 1.0f, 5.0f } },
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
		static inline std::array<std::array<double, LocusSliders::uFatLow - LocusSliders::uFatBelly>, Locus::NUMOFLOCI> voretypes_partgain = { { 
			{ 0.7, 0.05, 0.05, 0.0 },
			{ 0.2, 0.5, 0.5, 0.0 },
			{ 0.0, 0.0, 1.0, 0.0 },
			{ 0.0, 0.0, 1.0, 0.0 },
			{ 0.0, 0.5, 0.5, 0.0 },
			{ 0.0, 0.1, 0.1, 0.5 } } };
	};

	class VoreGlobals
	{
		//probably a shitty way to do this
	public:
		static inline const char* MORPH_KEY = "SkyrimVorePP.esp";
		static inline IBodyMorphInterface* body_morphs = nullptr;
		//static inline std::mt19937 randomMT;
		static inline const std::array<Locus, Locus::NUMOFLOCI> locus_transfer_destination = { Locus::lBowel,
			Locus::lStomach,
			Locus::lStomach,
			Locus::lStomach,
			Locus::lStomach,
			Locus::lStomach };

		static inline std::vector<RE::FormID> delete_queue;
	};
}