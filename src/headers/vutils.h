#pragma once
#include "headers/voredata.h"

namespace Vore::Log
{
	void PrintVoreData(RE::FormID voreNpc = 0);

}

namespace Vore
{
	float GetObjectSize(RE::TESObjectREFR* target);
	std::vector<RE::FormID> FilterPrey(RE::FormID pred, Vore::Locus locus, bool noneIsAny);
	//update visuals for all necessary character
	void UpdateBelly(const double& delta);
}