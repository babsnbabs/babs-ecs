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

TEST_SUITE("Event Manager")
{
	TEST_CASE("EventManager can subscribe and broadcast events")
	{
		int expectedPayload = 111;
		EventManager eventManager;

		eventManager.Subscribe<Example>([&](const std::any& e) {
			Example eventData = std::any_cast<Example>(e);
			REQUIRE(eventData.payload == 111);
			std::cout << "yeah babs, it delivered the event data: " << eventData.payload << std::endl;
		});
		eventManager.Broadcast<Example>(Example(111));
	}
}