#pragma once

namespace Vore
{
	constexpr int struggle_sliders_per_locus = 5;

	enum Locus : uint8_t
	{
		lStomach,
		lBowel,
		lBreastl,
		lBreastr,
		lWomb,
		lBalls,

		NUMOFLOCI,
		lNone
	};

	// stomach 0% - throat
	// stomach 100% - stomach
	// bowel 0% - stomach
	// bowel 100% - bowel
	// breastl 0% - breastl
	// breastl 100% - breastl
	// breastr 0% - breastr
	// breastr 100% - breastr
	// womb 0% - womb
	// womb 100% - womb
	// balls 0% - cock
	// balls 100% - balls
	//
	// fat min - fatLow
	// fat max - fatHigh
	// growth min - uGrowthLow
	// growth max - uGrowthHigh
	// post av fat gain - ass
	enum LocusSliders : uint8_t
	{
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
	};

	enum VoreState : uint8_t
	{
		//overall state
		hNone = 0,
		hSafe = 1,
		hLethal = 2,
		hReformation = 3,

		//stamina? mod
		sStill = 0,
		sStruggling = 1,
		sExhausted = 2,

		//full tour
		//for every locus other than lBowel should be mStill
		mStill = 0,
		mIncrease = 1,
		mDecrease = 2
	};

	struct VoreDataEntry
	{
		//saved 
		// pred
		RE::FormID pred = 0;
		//set of prey
		std::unordered_set<RE::FormID> prey = {};
		/*
		hNone = 0,
		hSafe = 1,
		hLethal = 2,
		hReformation = 3,
		*/
		std::array<VoreState, Locus::NUMOFLOCI> pdLoci = { VoreState::hNone };

		//struggle process per locus
		std::array<double, Locus::NUMOFLOCI> pdIndigestion = { 0 };

		//semi-permanent growth
		std::array<double, LocusSliders::uFatLow - LocusSliders::uFatBelly> pdGrowthLocus = { 0 };

		//universal stats
		RE::FormType aCharType = RE::FormType::None;
		bool aIsPlayer = false;
		bool aAlive = true;
		// how big this character is. changes stomach size
		double aSize = 0;
		// how much does this character weight (with equipment), changes digestion speed
		double aWeight = 0;

		//pred stats
		//temp wg
		double pdFat = 0;
		//long-term wg; lower than normal wg, but lasts for much longer
		double pdFatgrowth = 0;
		double pdSizegrowth = 0;

		//prey stats
		Locus pyLocus = Locus::lNone;
		Locus pyElimLocus = Locus::lNone;
		VoreState pyDigestion = VoreState::hNone;
		VoreState pyStruggle = VoreState::sStill;
		//full tour, only for lBowel
		//mIncrease is when prey is moving towards stomach
		VoreState pyLocusMovement = VoreState::mStill;

		double pyDigestProgress = 0;
		double pySwallowProcess = 0;
		// full tour, only for lBowel
		// if character is dead, this will be synced with digest process
		double pyLocusProcess = 0;

		//not saved

		//current slider values
		std::array<float, LocusSliders::NUMOFSLIDERS> pdSliders = { 0 };
		//slider goals
		std::array<float, LocusSliders::NUMOFSLIDERS> pdGoal = { 0 };

		//struggle sliders per locus
		std::array<float, Locus::NUMOFLOCI * struggle_sliders_per_locus> pdStruggleSliders = { 0 };
		std::array<float, Locus::NUMOFLOCI * struggle_sliders_per_locus> pdStruggleGoal = { 0 };

		//size of all loci/sliders before factoring in that at higher values a slider will add more volume
		//so, at 2 preys this may be something like 200, while pdGoal is like 150
		//std::array<double, LocusSliders::NUMOFSLIDERS> pdGoalVolume = { 0 };
		//moved this calculation to UpdateBelly()

		bool pdUpdateGoal = false;
		bool pdUpdateSlider = false;
		//this is float to count the amount of preys, including stamina or health% (idk)
		bool pdUpdateStruggleGoal = 0;

		bool pdUpdateStruggleSlider = false;
		double pdFullBurden = 0.0;


		// REF HANDLE !!!
		RE::ObjectRefHandle me;

		RE::TESObjectREFR* get() const;

		void GetSizeWeight(double& size, double& weight);
		double GetStomachSpace(uint64_t locus);
	};

	class VoreData
	{
		//presistent data will be saved here
	public:
		static inline std::unordered_map<RE::FormID, Vore::VoreDataEntry> Data;

		static bool IsValid(RE::FormID character);
		static bool IsPred(RE::FormID character);
		static bool IsPrey(RE::FormID character);

		static RE::FormID MakeData(RE::TESObjectREFR* character);
		static void SoftDelete(RE::FormID character);
		static void HardDelete(RE::FormID character);

		static void DataSetup();

		static void OnSave(SKSE::SerializationInterface* a_intfc);
		static void OnLoad(SKSE::SerializationInterface* a_intfc);
		static void OnRevert(SKSE::SerializationInterface* a_intfc);

	};

}