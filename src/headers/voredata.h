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
		//saved
		// pred
		RE::FormID pred = 0;
		//set of prey
		std::set<RE::FormID> prey = {};
		/*
		hNone = 0,
		hSafe = 1,
		hLethal = 2,
		hReformation = 3,
		*/
		std::array<VoreState, Locus::NUMOFLOCI> pdLoci = { VoreState::hNone };
		std::array<double, Locus::NUMOFLOCI> pdAcid = { 0 };

		//struggle process per locus
		std::array<double, Locus::NUMOFLOCI> pdIndigestion = { 0 };

		//semi-permanent growth
		std::array<double, 4> pdGrowthLocus = { 0 };

		//universal stats
		float aScaleDefault = 0.0f;
		bool aIsChar = false;
		bool aIsPlayer = false;
		bool aIsContainer = false;
		bool aAlive = false;
		bool aEssential = false;
		bool aProtected = false;
		RE::SEX aSex = RE::SEX::kNone;
		// how big this character is. changes stomach size
		double aSize = 0;
		double aSizeDefault = 0;
		// used for scaling sliders for big sizes
		//float aSizeScale = 1.0f;



		bool aDialogue = false;
		//pred stats
		//calculated on voredata creation and game reloading
		//if char has wg but this is turned off, wg will be reset
		bool pdWGAllowed = false;
		//temp wg
		double pdFat = 0;
		//long-term wg; lower than normal wg, but lasts for much longer
		double pdFatgrowth = 0;
		double pdSizegrowth = 0;

		//prey stats
		Locus pyLocus = Locus::lNone;
		Locus pyElimLocus = Locus::lNone;
		VoreState pyDigestion = VoreState::hNone;
		// prey will start reforming upon death
		// bool pyPendingReformation = false;

		uint8_t pyStruggleResource = 0;
		bool pyConsentEndo = false;
		bool pyConsentLethal = false;
		bool pyStruggling = false;
		
		//full tour, only for lBowel
		//mIncrease is when prey is moving towards stomach
		FullTour pyLocusMovement = FullTour::mStill;

		double pyDigestProgress = 0;
		double pySwallowProcess = 0;
		// full tour, only for lBowel
		// if character is dead, this will be synced with digest in reverse with an offset
		double pyLocusProcess = 0;

		//not saved

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
		//used for struggle sounds
		RE::SEX pdStrugglePreySex = RE::SEX::kNone;
		//used for stomach ambient sounds
		bool pdHasDigestion = false;


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
		/// <summary>
		/// not implemented
		/// </summary>
		/// <param name="locus"></param>
		/// <returns></returns>
		void DigestLive();
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

	class VoreData
	{
		//presistent data will be saved here
	public:
		static inline std::unordered_map<RE::FormID, Vore::VoreDataEntry> Data{};
		static inline std::unordered_map<RE::FormID, RE::FormID> Reforms{};

		static bool IsValid(RE::FormID character);
		static VoreDataEntry* IsValidGet(RE::FormID character);
		static bool IsPred(RE::FormID character, bool onlyActive);
		static bool IsPrey(RE::FormID character);

		static RE::FormID MakeData(RE::TESObjectREFR* character);
		static void SoftDelete(RE::FormID character, bool leaveOnlyActive = false);
		static void HardDelete(RE::FormID character);

		static void DataSetup();

		static void OnSave(SKSE::SerializationInterface* a_intfc);
		static void OnLoad(SKSE::SerializationInterface* a_intfc);
		static void OnRevert(SKSE::SerializationInterface* a_intfc);
	};

}