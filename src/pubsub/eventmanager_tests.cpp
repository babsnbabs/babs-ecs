#include "doctest.h"
#include "PubSub.hpp"

#include <functional>

struct ExampleEvent : public Event
{
	ExampleEvent() {};
	virtual ~ExampleEvent() {};

	static constexpr DescriptorType descriptor = "ExampleEvent";

	virtual DescriptorType Type() const
	{
		return descriptor;
	}
};

struct ExampleObserver {
	int eventCount = 0;
	void handle(const Event& e)
	{
		if (e.Type() == ExampleEvent::descriptor)
		{
			eventCount++;
		}
	}
};

TEST_SUITE("Event Manager")
{
	TEST_CASE("EventManager can subscribe and broadcast events")
	{
		EventManager eventManager;
		ExampleObserver exampleObserver;
		eventManager.Subscribe(ExampleEvent::descriptor, std::bind(&ExampleObserver::handle, &exampleObserver, std::placeholders::_1));

		REQUIRE(exampleObserver.eventCount == 0);

		eventManager.Broadcast(ExampleEvent());

		REQUIRE(exampleObserver.eventCount == 1);

		eventManager.Broadcast(ExampleEvent());
		eventManager.Broadcast(ExampleEvent());
		eventManager.Broadcast(ExampleEvent());
		eventManager.Broadcast(ExampleEvent());

		REQUIRE(exampleObserver.eventCount == 5);
	}

	TEST_CASE("Broadcast does not break with no subscribers")
	{
		EventManager eventManager;
		ExampleObserver exampleObserver;

		// Starts off as 0
		REQUIRE(exampleObserver.eventCount == 0);
		eventManager.Broadcast(ExampleEvent());
		
		// Was never subscribed to, should still be 0
		REQUIRE(exampleObserver.eventCount == 0);
	}
}