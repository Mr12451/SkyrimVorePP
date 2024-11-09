#include "headers/times.h"
#include "headers/ui.h"
#include "headers/settings.h"

namespace Vore
{
	Timer::Timer(uint8_t id, double max, TimerFunc f)
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
			f(value);
			value = 0;
		}
	}

	float* Time::WorldTimeDelta()
	{
		static float* wtdelta = (float*)RELOCATION_ID(523660, 410199).address();
		return wtdelta;
		//return 1;
	}
	float* Time::RealTimeDelta()
	{
		static float* rtdelta = (float*)RELOCATION_ID(523661, 410200).address();
		return rtdelta;
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
		/*if (*GetTimeMultiplier() > 1.0) {
			flog::critical("TIME {}", *GetTimeMultiplier());
		}*/
		worldTimeElapsed += *WorldTimeDelta();
		realTimeElapsed += *RealTimeDelta();

		if (UI::VoreMenu::NeedUpdate) {
			UI::VoreMenu::Update();
		}
		else if (UI::VoreMenu::_menuMode == UI::kSwallow && VoreSettings::ui_show_time) {
			auto thisMenu = UI::VoreMenu::GetVoreMenu();
			if (!thisMenu) {
				flog::critical("Cannot switch menu states - no menu found!");
				return;
			}
			std::string text{ "" };
			text += std::format("\n\n{:.4f}, {:.4f}", *Time::RealTimeDelta(), realTimeElapsed);
			text += std::format("\n{:.4f}, {:.4f}", *Time::WorldTimeDelta(), worldTimeElapsed);
			text += std::format("\n{:.4f}", *Time::GetTimeMultiplier());
			thisMenu->SetText(text);
		}
		for (auto& el : timers) {
			el.Process(*WorldTimeDelta());
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
