#pragma once

namespace Vore
{
	typedef void (*VoidFunc)();

	struct Timer
	{
		double value;
		double max;
		uint8_t id;
		VoidFunc f;

		Timer(uint8_t id, double max, VoidFunc f);
		void Process(float& delta);
	};

	class Time
	{
	public:
		static float WorldTimeDelta();
		static float RealTimeDelta();
		static double WorldTimeElapsed();
		static double RealTimeElapsed();
		static std::uint64_t FramesElapsed();
		static float* GetTimeMultiplier();
		static void MultiplyGameTime(float modifier);
		static void Update();
		static void SetTimer(Timer& t);
		static void DeleteAllTimers();

	private:
		static inline double worldTimeElapsed = 0.0;
		static inline double realTimeElapsed = 0.0;
		static inline std::uint64_t framesElapsed = 0;
		static inline std::vector<Timer> timers = {};
	};
}