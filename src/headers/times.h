#pragma once

namespace Vore
{
	typedef void (*TimerFunc)(const double& delta);

	struct Timer
	{
		double value;
		double max;
		uint8_t id;
		TimerFunc f;

		Timer(uint8_t id, double max, TimerFunc f);
		void Process(float& delta);
	};

	class Time
	{
	public:
		static float* WorldTimeDelta();
		static float* RealTimeDelta();
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