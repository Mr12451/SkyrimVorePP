#include "headers/log.h"
#include "headers/voredata.h"
#include "headers/events.h"
#include "headers/hook.h"
#include "headers/plugins.h"
#include "headers/racemenu.h"
#include "headers/settings.h"

const SKSE::MessagingInterface* g_messaging;

inline void InitializeEvents()
{
	flog::trace("Begin event initialization");
	auto& eventProcessor = Vore::EventProcessor::GetSingleton();
	flog::trace("Got event processor");
	//RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESActivateEvent>(&eventProcessor);
	//RE::UI::GetSingleton()->AddEventSink<RE::MenuOpenCloseEvent>(&eventProcessor);
	RE::BSInputDeviceManager::GetSingleton()->AddEventSink<InputEvents>(&eventProcessor);
	flog::trace("End event initialization");
}


void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
		flog::trace("All plugins loaded");
		break;
	case SKSE::MessagingInterface::kPostPostLoad:
		{
			flog::trace("Post Post load");
			//get racemenu body morph interface
			InterfaceExchangeMessage* r_inter = new InterfaceExchangeMessage();
			g_messaging->Dispatch(InterfaceExchangeMessage::kMessage_ExchangeInterface, r_inter, sizeof(r_inter), nullptr);
			IInterfaceMap* racemenuInterface = r_inter->interfaceMap;
			delete r_inter;
			flog::info("Racemenu (skee64) interfaces: {}", racemenuInterface != nullptr);
			Vore::VoreGlobals::body_morphs = (IBodyMorphInterface*)(racemenuInterface->QueryInterface("BodyMorph"));
			flog::info("IBodyMorphInterface: {}", Vore::VoreGlobals::body_morphs != nullptr);

			//seed random

			break;
		}
	case SKSE::MessagingInterface::kPreLoadGame:
		flog::trace("Loading save: {}", static_cast<char*>(a_msg->data));
		Vore::Plugin::SetInGame(false);
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		flog::trace("Save file successfuly loaded? {}", static_cast<bool>(a_msg->data));
		Vore::Plugin::SetInGame(true);
		break;
	case SKSE::MessagingInterface::kSaveGame:
		flog::trace("Game saved: {}", static_cast<char*>(a_msg->data));
		break;
	case SKSE::MessagingInterface::kDeleteGame:
		flog::trace("Game deleted: {}", static_cast<char*>(a_msg->data));
		break;
	case SKSE::MessagingInterface::kInputLoaded:
		flog::trace("Input loaded");
		break;
	case SKSE::MessagingInterface::kNewGame:
		flog::trace("New game started");
		Vore::Plugin::SetInGame(true);
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		flog::trace("Data loaded");
		InitializeEvents();
		break;
	}
}

void InitializeMessaging(const SKSE::LoadInterface* skse)
{
	g_messaging = SKSE::GetMessagingInterface();
	auto g_pluginHandle = skse->GetPluginHandle();
	flog::info("Plugin Handle: {}", g_pluginHandle);
	if (!g_messaging->RegisterListener("SKSE", MessageHandler)) {
		flog::critical("Failed to register messaging listener");
	}
}

inline void InitializeSerialization()
{
	flog::trace("Initializing cosave serialization...");
	auto* a_intfc = SKSE::GetSerializationInterface();
	a_intfc->SetUniqueID(_byteswap_ulong('SVPP'));
	a_intfc->SetSaveCallback(Vore::VoreData::OnSave);
	a_intfc->SetRevertCallback(Vore::VoreData::OnRevert);
	a_intfc->SetLoadCallback(Vore::VoreData::OnLoad);
	flog::trace("Cosave serialization initialized.");
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	SetupLog();
	flog::info("Loaded plugin {} {}", Plugin::NAME, Plugin::VERSION.string());
	InitializeSerialization();
	InitializeMessaging(a_skse);
	Vore::Hooks::Install();
	return true;
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() noexcept {
	SKSE::PluginVersionData v;
	v.PluginName(Plugin::NAME.data());
	v.PluginVersion(Plugin::VERSION);
	v.UsesAddressLibrary(true);
	v.HasNoStructUse();
	return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* pluginInfo)
{
	pluginInfo->name = SKSEPlugin_Version.pluginName;
	pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
	pluginInfo->version = SKSEPlugin_Version.pluginVersion;
	return true;
}