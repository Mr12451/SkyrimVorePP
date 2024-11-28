#pragma once

using InputEvents = RE::InputEvent*;
namespace Vore
{
	class EventProcessor :
		public RE::BSTEventSink<RE::TESActivateEvent>,
		public RE::BSTEventSink<InputEvents>,
		public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
		public RE::BSTEventSink<RE::TESDeathEvent>,
		public RE::BSTEventSink<RE::TESWaitStartEvent>,
		public RE::BSTEventSink<RE::TESWaitStopEvent>,
		public RE::BSTEventSink<RE::TESSleepStartEvent>,
		public RE::BSTEventSink<RE::TESSleepStopEvent>,
		public RE::BSTEventSink<RE::TESFastTravelEndEvent>
	{
		// Pretty typical singleton setup
		// *Private* constructor/destructor
		// And we *delete* the copy constructors and move constructors.

	public:
		// Returns a reference to the one and only instance of EventProcessor :)
		//
		// Note: this is returned as a & reference. When you need this as a pointer, you'll want to use & (see below)
		static EventProcessor& GetSingleton();

		// Log information about Activate events that happen in the game
		RE::BSEventNotifyControl ProcessEvent(const RE::TESActivateEvent* event,
			RE::BSTEventSource<RE::TESActivateEvent>*) override;

		// Log information about Activate events that happen in the game
		RE::BSEventNotifyControl ProcessEvent(const InputEvents* event,
			RE::BSTEventSource<InputEvents>*) override;

		// Log information about Menu open/close events that happen in the game
		RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* event,
			RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override;

		// handle actor death
		RE::BSEventNotifyControl ProcessEvent(const RE::TESDeathEvent* event,
			RE::BSTEventSource<RE::TESDeathEvent>*) override;


		// Time pass events
		RE::BSEventNotifyControl ProcessEvent(const RE::TESWaitStartEvent* event,
			RE::BSTEventSource<RE::TESWaitStartEvent>*) override;
		RE::BSEventNotifyControl ProcessEvent(const RE::TESWaitStopEvent* event,
			RE::BSTEventSource<RE::TESWaitStopEvent>*) override;

		RE::BSEventNotifyControl ProcessEvent(const RE::TESSleepStartEvent* event,
			RE::BSTEventSource<RE::TESSleepStartEvent>*) override;
		RE::BSEventNotifyControl ProcessEvent(const RE::TESSleepStopEvent* event,
			RE::BSTEventSource<RE::TESSleepStopEvent>*) override;

		RE::BSEventNotifyControl ProcessEvent(const RE::TESFastTravelEndEvent* event,
			RE::BSTEventSource<RE::TESFastTravelEndEvent>*) override;

	private:
		float _waitHours{ 0 };
		float _sleepHours{ 0 };
		EventProcessor();
		~EventProcessor();
		EventProcessor(const EventProcessor&);
		EventProcessor(EventProcessor&&);
		EventProcessor& operator=(const EventProcessor&) = delete;
		EventProcessor& operator=(EventProcessor&&) = delete;
	};

	class VEventProcessor
	{
	public:
		static void Hook();

	private:
		static RE::BSEventNotifyControl ProcessEvent(RE::BGSImpactManager* a_this, const RE::BGSFootstepEvent* a_event, RE::BSTEventSource<RE::BGSFootstepEvent>* a_eventSource);
		static inline REL::Relocation<decltype(ProcessEvent)> _ProcessEvent;
	};
}