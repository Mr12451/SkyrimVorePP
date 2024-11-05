#include "headers/times.h"
#include "headers/ui.h"

namespace Vore
{
	Timer::Timer(uint8_t id, double max, VoidFunc f)
	{
		this->value = 0;
		this->id = id;
		this->max = max;
		this->f = f;
	}

	void Timer::Process(float& delta)
	{
		value += delta;
		if (value >= max) {
			f();
			value = 0;
		}
	}

	float Time::WorldTimeDelta()
	{
		static float* wtdelta = (float*)RELOCATION_ID(523660, 410199).address();
		return (*wtdelta);
		//return 1;
	}
	float Time::RealTimeDelta()
	{
		static float* rtdelta = (float*)RELOCATION_ID(523661, 410200).address();
		return (*rtdelta);
		//return 1;
	}
	double Time::WorldTimeElapsed()
	{
		return worldTimeElapsed;
	}

	double Time::RealTimeElapsed()
	{
		return realTimeElapsed;
	}

	std::uint64_t Time::FramesElapsed()
	{
		return framesElapsed;
	}

	float* Time::GetTimeMultiplier()
	{
		static float* tmulti = (float*)RELOCATION_ID(511883, 388443).address();
		return (tmulti);
	}

	void Time::MultiplyGameTime(float modifier)
	{
		*GetTimeMultiplier() = modifier;
		using func_t = decltype(MultiplyGameTime);
		REL::Relocation<func_t> func{ RELOCATION_ID(66989, 68246) };
		return;
	}

	void Time::Update()
	{
		framesElapsed++;
		float delta = Time::RealTimeDelta();
		//worldTimeElapsed += delta;
		realTimeElapsed += delta;

		if (UI::VoreMenu::NeedUpdate) {
			UI::VoreMenu::Update();
		}

		for (auto& el : timers) {
			el.Process(delta);
		}
	}

	void Time::SetTimer(Timer& t)
	{
		for (auto& el : timers) {
			if (el.id == t.id) {
				el.max = t.max;
				el.value = 0;
				el.f = t.f;
				return;
			}
		}
		timers.push_back(t);
	}

	void Time::DeleteAllTimers()
	{
		timers.clear();
	}
}
