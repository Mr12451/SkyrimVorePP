#include "headers/papyrusUtil.h"
#include "headers/versionlibdb.h"

namespace
{
	using VM = RE::BSScript::Internal::VirtualMachine;
	using ObjectPtr = RE::BSTSmartPointer<RE::BSScript::Object>;
}

namespace Vore
{
	void PapyrusAPI::TalkToPred()
	{
	}
	void PapyrusAPI::TalkToPrey()
	{
	}

	static bool InitializeOffsets()
	{
		void* MyAddress = NULL;
		unsigned long long MyId = 0;
		unsigned long long MyOffset = 0;
		// Allocate on stack so it will be unloaded when we exit this function.
		// No need to have the whole database loaded and using up memory for no reason.
		VersionDb db;

		// Load database with current executable version.
		if (!db.Load()) {
			flog::critical("Failed to load version database for current executable!");
			return false;
		} else {
			// "SkyrimSE.exe", "1.5.97.0"
			flog::info("Loaded database for %s version %s.", db.GetModuleName().c_str(), db.GetLoadedVersionString().c_str());
		}

		// This address already includes the base address of module so we can use the address directly.
		/*MyAddress = db.FindAddressById(123);
		if (MyAddress == NULL) {
			flog::critical("Failed to find address!");
			return false;
		}*/

		void* ptr = reinterpret_cast<void*>(0x003CDEE0);


		// This offset does not include base address. Actual address would be ModuleBase + MyOffset.
		/*if (!db.FindIdByAddress(ptr, MyOffset)) {
			flog::critical("Failed to find offset for my thing!");
			return false;
		}
		flog::info("My id", MyOffset);
		
		*/

		MyOffset = 25052;
		if (!db.FindIdByOffset(MyOffset, MyId)) {
			flog::critical("Failed to find id for my offset!");
			return false;
		}
		flog::critical("my id {:x}", MyId);
		MyAddress = db.FindAddressById(MyId);

		flog::critical("my address {:x}", (unsigned long long)MyAddress);

		// Everything was successful.
		return true;
	} 

	void PapyrusAPI::Register()
	{
		/*REL::IDDatabase::Offset2ID t;
		flog::critical("{:x}", t(0x61dc));
		InitializeOffsets();*/
	}
}

