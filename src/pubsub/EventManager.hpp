#pragma once

#include <functional>
#include <map>
#include <vector>
#include <string>

#ifdef __APPLE__
#include <experimental/any>
#else
#include <any>
#endif

#include "ECSEvents.hpp"

// EventManager is a simple observer pattern for subscribing to and broadcasting custom events
class EventManager
{
public:
	// EventHandler represents the anonymous/bound function provided by an observer
	template <typename EventType>
	using EventHandler = std::function<void(const EventType&)>;

	// Subscribe to the provided EventType with a function
	template <typename EventType>
	void Subscribe(EventHandler<EventType>&& slot)
	{
		std::string type = typeid(EventType).name();
		this->observers[type].push_back(slot);
	}

	// Broadcast will emit the EventType to all registered observers
	template <typename EventType>
	void Broadcast(const EventType& event) const
	{
		std::string type = typeid(EventType).name();

		// bail if we don't have any observers
		if (observers.find(type) == observers.end())
		{
			return;
		}

		// for each observer, call their event handler
		auto event_observers = this->observers.at(type);

		for (auto event_observer : event_observers)
		{
			// cast back to the concrete function handler and call it
			// we do this here so that the observer doesn't have to
#ifdef __APPLE__
			auto handler = std::experimental::any_cast<EventHandler<EventType>>(event_observer);
#else
			auto handler = std::any_cast<EventHandler<EventType>>(event_observer);
#endif

			handler(event);
		}
	}

private:
	// this is a map of event type names -> list of function handlers
#ifdef __APPLE__
	std::map<std::string, std::vector<std::experimental::any>> observers;
#else
	std::map<std::string, std::vector<std::any>> observers;
#endif
};