#pragma once

using InputEvents = RE::InputEvent*;
namespace Vore
{
	class EventProcessor :
		public RE::BSTEventSink<RE::TESActivateEvent>,
		public RE::BSTEventSink<InputEvents>,
		public RE::BSTEventSink<RE::MenuOpenCloseEvent>
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

	private:
		EventProcessor();
		~EventProcessor();
		EventProcessor(const EventProcessor&);
		EventProcessor(EventProcessor&&);
		EventProcessor& operator=(const EventProcessor&) = delete;
		EventProcessor& operator=(EventProcessor&&) = delete;
	};
}