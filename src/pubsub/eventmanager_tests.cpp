#include "doctest.h"
#include "PubSub.hpp"

#include <functional>
#include <iostream>

struct Example
{
public:
	int payload;

	Example() : payload(0) {}
	Example(int payload) : payload(payload) {}
};

struct ExampleObserver {
	int eventCount = 0;
	int lastPayloadReceived = 0;
	void HandlExample(const std::any& e)
	{
		Example eventData = std::any_cast<Example>(e);
		this->lastPayloadReceived = eventData.payload;
		eventCount++;
	}
};


TEST_SUITE("Event Manager")
{
	EventManager eventManager;
	int expectedPayload = 111;

	TEST_CASE("EventManager can subscribe and broadcast and event to an anonymous function")
	{
		eventManager.Subscribe<Example>([&](const std::any& e) {
			Example eventData = std::any_cast<Example>(e);
			REQUIRE(eventData.payload == 111);
		});
		eventManager.Broadcast<Example>(Example(111));
	}

	//TEST_CASE("EventManager can subscribe and broadcast to a function in a class")
	//{
	//	ExampleObserver observer;
	//	REQUIRE(observer.eventCount == 0);

	//	eventManager.Subscribe<Example>(std::bind(&ExampleObserver::HandlExample, &observer, std::placeholders::_1));
	//	eventManager.Broadcast<Example>(Example(111));

	//	REQUIRE(observer.eventCount == 1);
	//	REQUIRE(observer.lastPayloadReceived == expectedPayload);
	//}

	//TEST_CASE("EventManager can broadcast an event no one is listening to")
	//{
	//	eventManager.Broadcast<Example>(Example(111));
	//}
}