#pragma once

namespace Vore
{
	constexpr int struggle_sliders_per_locus = 5;

	// same order as in devourment for compatibility
	enum Locus : uint8_t
	{
		lStomach,
		lBowel,
		lWomb,
		lBreastl,
		lBreastr,
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
		uBowel,
		uWomb,
		uFatBelly,

		uThroat,
		uBreastl,
		uBreastr,
		uCock,
		uBalls,

		uFatAss,
		uFatBreasts,
		uFatCock,

		uFatLow,
		uFatHigh,
		uGrowthLow,
		uGrowthHigh,

		NUMOFSLIDERS
	};

	struct SoundHandles
	{
		RE::BSSoundHandle swallowHandle{};
		RE::BSSoundHandle digestHandle{};
		RE::BSSoundHandle struggleHandle{};
		RE::SEX preySex = RE::SEX::kNone;
		//RE::BSSoundHandle swallowHandle {};
	};

	struct VStats
	{
		int pdLvl = 0;
		int pyLvl = 0;
	};

	class VoreDataEntry
	{
	public:
		enum VoreState : uint8_t
		{
			//overall state
			hNone = 0,
			hSafe = 1,
			hLethal = 2,
			hReformation = 3,

			//full tour
			//for every locus other than lBowel should be mStill

		};

		enum FullTour : uint8_t
		{
			mStill = 0,
			mIncrease = 1,
			mDecrease = 2
		};

		using VoreStateFunc = void (Vore::VoreDataEntry::*)(const double&);
		// -------------------------------
		//SAVED
		// -------------------------------

		// pred
		RE::FormID pred = 0;
		//set of prey
		std::set<RE::FormID> prey = {};
		//locus digestion mode
		std::array<VoreState, Locus::NUMOFLOCI> pdLoci = { VoreState::hNone };
		//acid levels
		std::array<double, Locus::NUMOFLOCI> pdAcid = { 0 };

		//struggle process per locus
		std::array<double, Locus::NUMOFLOCI> pdIndigestion = { 0 };

		//semi-permanent growth
		std::array<double, 4> pdGrowthLocus = { 0 };

		//universal stats

		//volume before size gain
		//calculated via colliders
		double aSizeDefault = 0;
		//delete object after vore end
		//for placeholder items and item vore stomach container
		bool aDeleteWhenDone = false;

		//pred stats

		//Accum xp; this updates Skills on digestion / vore end / prey death
		float pdXP = 0;
		//temp wg
		double pdFat = 0;
		//long-term wg; lower than normal wg, but lasts for much longer
		double pdFatgrowth = 0;
		//size gain. Size gain also reduces fat sliders because total model volume increases
		double pdSizegrowth = 0;

		//prey stats

		float pyXP = 0;
		Locus pyLocus = Locus::lNone;
		// where prey died (if they died), used for locus-based wg
		Locus pyElimLocus = Locus::lNone;
		// current digestion mode. Do not change manually Do not change manually Do not change manually Do not change manually Do not change manually
		// Do not change manually Do not change manually Do not change manually
		// Do not change manually Do not change manually Do not change manually
		VoreState pyDigestion = VoreState::hNone;
		// how many times they prey ran out of stamina
		uint8_t pyStruggleResource = 0;
		// calculated on swallow; remake the system later (for new dialogue plugin)
		bool pyConsentEndo = false;
		// calculated on swallow; remake the system later (for new dialogue plugin)
		bool pyConsentLethal = false;
		//full tour, only for lBowel; mIncrease is when prey is moving towards stomach
		FullTour pyLocusMovement = FullTour::mStill;
		double pyDigestProgress = 0;
		double pySwallowProcess = 0;
		//full tour
		double pyLocusProcess = 0;

		// -------------------------------
		//NOT SAVED
		// -------------------------------
		bool aIsChar = false;
		bool aIsPlayer = false;
		bool aAlive = false;
		bool aEssential = false;
		bool aProtected = false;
		RE::SEX aSex = RE::SEX::kNone;
		double aSize = 0;
		// for size gain
		float aScaleDefault = 0.0f;
		bool aDialogue = false;

		// pred stats

		//if char has wg but this is turned off, wg will be reset
		bool pdWGAllowed = false;
		//current slider values
		std::array<float, LocusSliders::NUMOFSLIDERS> pdSliders = { 0 };
		//slider goals
		std::array<float, LocusSliders::NUMOFSLIDERS> pdGoal = { 0 };
		std::array<float, LocusSliders::NUMOFSLIDERS> pdGoalStep = { 0 };
		//struggle sliders per locus
		std::array<float, Locus::NUMOFLOCI * struggle_sliders_per_locus> pdStruggleSliders = { 0 };
		std::array<float, Locus::NUMOFLOCI * struggle_sliders_per_locus> pdStruggleGoal = { 0 };
		std::array<float, Locus::NUMOFLOCI * struggle_sliders_per_locus> pdStruggleGoalStep = { 0 };
		std::array<float, LocusSliders::NUMOFSLIDERS> pdAccumStruggle = { 0 };
		bool pdUpdateGoal = false;
		bool pdUpdateSlider = false;
		bool pdUpdateStruggleGoal = 0;
		bool pdUpdateStruggleSlider = false;
		//this is to count the size of preys for sounds and slow effect
		float pdFullBurden = 0.0f;
		//used for struggle sounds; Maybe add an array later, when new sounds are done
		RE::SEX pdStrugglePreySex = RE::SEX::kNone;
		//used for stomach ambient sounds
		bool pdHasDigestion = false;

		//prey stats

		bool pyStruggling = false;
		//passives and stats
		VStats myStats{};
		// REF HANDLE !!!
		RE::ObjectRefHandle me;
		// PAPYRUS OBJECT FOR PERSISTENCY !!!
		RE::BSTSmartPointer<RE::BSScript::Object> meVm = nullptr;

		//sound handles
	private:
		VoreStateFunc FastU = nullptr;
		VoreStateFunc SlowU = nullptr;
		VoreStateFunc BellyU = nullptr;
		VoreStateFunc PredU = nullptr;
		SoundHandles soundHandles{};

	public:
		RE::TESObjectREFR* get() const;
		void UpdatePredScale();
		double GetStomachSpace(uint64_t locus);

		void CalcFast(bool forceStop = false);
		void CalcSlow(bool forceStop = false);
		void SetConsent(bool willing, bool lethal);
		void SetBellyUpdate(bool doUpdate);
		void SetPredUpdate(bool doUpdate);
		void ClearAllUpdates();

		void UpdateStruggleGoals();
		void UpdateSliderGoals();

		const VoreStateFunc& Fast() const { return FastU; }
		const VoreStateFunc& Slow() const { return SlowU; }
		const VoreStateFunc& Belly() const { return BellyU; }
		const VoreStateFunc& Predd() const { return PredU; }

		//sounds
		void PlaySound(RE::BGSSoundDescriptorForm* sound, float volume = 1.0f) const;
		void PlayRegurgitation(bool ass) const;
		void PlayScream(const VoreDataEntry* prey) const;
		void PlaySwallow();
		void PlayBurpRandom() const;
		void PlayGurgleRandom() const;
		void PlayStomachSounds();

		void StopAllSounds();

		//emotes
		void EmoteSmile(int duration_ms) const;

		void GetSize(double& size);
		void DigestLive();
		void AnimatedSwallow() const;
		void DoSwallow();
		// changes pred's digestion mode
		void SetDigestionAsPred(const Locus locus, VoreDataEntry::VoreState dType, const bool forceStopDigestion, bool doDialogueUpd = true);
		// use this to set digestion if dType = hNone, pred's current digestion will be used
		void SetMyDigestion(VoreDataEntry::VoreState dType, bool updateSounds);
		void UpdateStats(bool isPred);
		void GetVStats();

	private:
		void HandlePreyDeathImmidiate();
		void HandleDamage(const double& delta, RE::Actor* asActor, VoreDataEntry* predData);

		//states

		void BellyUpdate(const double& delta);
		void SlowF(const double& delta);
		void SlowD(const double& delta);
		void SlowR(const double& delta);
		void SlowP(const double& delta);
		void Struggle(const double& delta, RE::Actor* asActor, VoreDataEntry* predData);
		void FastLethalW(const double& delta);
		void FastLethalU(const double& delta);
		void FastHealW(const double& delta);
		void FastHealU(const double& delta);
		void FastEndoU(const double& delta);
		void Swallow(const double& delta);
		void PredSlow(const double& delta);
	};
}