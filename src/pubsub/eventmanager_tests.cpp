#include "doctest.h"
#include "PubSub.hpp"

#include <functional>
#include <iostream>

struct ExampleEvent : public Event
{
	ExampleEvent() {};
	virtual ~ExampleEvent() {};
};

struct AnotherEvent : public Event
{
    AnotherEvent() {};
    virtual ~AnotherEvent() {};
};

struct ExampleObserver {
	int eventCount = 0;
	void handle(const Event& e)
	{
        std::cout << "ExampleObserver received: " << typeid(e).name() << std::endl;
		eventCount++;
	}
};

TEST_SUITE("Event Manager")
{
	TEST_CASE("EventManager can subscribe and broadcast events")
	{
		EventManager eventManager;

        // binding to a classes method
		ExampleObserver exampleObserver;
		eventManager.Subscribe<ExampleEvent>(std::bind(&ExampleObserver::handle, &exampleObserver, std::placeholders::_1));

        // binding an anonymous function
        int eventCount = 0;
        eventManager.Subscribe<ExampleEvent>([&](const Event& e){
            std::cout << "Anonymous function received: " << typeid(e).name() << std::endl;
            eventCount++;
        });

		REQUIRE(exampleObserver.eventCount == 0);

		eventManager.Broadcast<ExampleEvent>(ExampleEvent());

		REQUIRE(exampleObserver.eventCount == 1);

		eventManager.Broadcast<ExampleEvent>(ExampleEvent());
		eventManager.Broadcast<ExampleEvent>(ExampleEvent());
		eventManager.Broadcast<ExampleEvent>(ExampleEvent());
		eventManager.Broadcast<ExampleEvent>(ExampleEvent());
        eventManager.Broadcast<AnotherEvent>(AnotherEvent());

		REQUIRE(exampleObserver.eventCount == 5);
	}

	TEST_CASE("Broadcast does not break with no subscribers")
	{
		EventManager eventManager;
		ExampleObserver exampleObserver;

		// Starts off as 0
		REQUIRE(exampleObserver.eventCount == 0);
		eventManager.Broadcast<ExampleEvent>(ExampleEvent());
		
		// Was never subscribed to, should still be 0
		REQUIRE(exampleObserver.eventCount == 0);
	}
}