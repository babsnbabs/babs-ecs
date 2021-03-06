#include "doctest.h"

#include <string>
#include <functional>
#include <iostream>

#include "EventManager.hpp"
#include "../ECSManager.hpp"

struct ExampleEvent
{
	int payload;

	ExampleEvent() : payload(0) {}
	ExampleEvent(int payload) : payload(payload) {}
};

struct ExampleObserver {
	int eventCount = 0;
	int lastPayloadReceived = 0;

	void HandlExample(const ExampleEvent& e)
	{
		this->lastPayloadReceived = e.payload;
		eventCount++;
	}
};

struct Identity
{
	std::string name;
};

TEST_SUITE("Event Manager")
{
	events::EventManager eventManager;
	int expectedPayload = 111;

	TEST_CASE("EventManager can subscribe and broadcast and event to an anonymous function")
	{
		eventManager.Subscribe<ExampleEvent>([&](const ExampleEvent& e) {
			REQUIRE(e.payload == expectedPayload);
			});
		eventManager.Broadcast<ExampleEvent>(ExampleEvent(expectedPayload));
	}

	TEST_CASE("EventManager can subscribe and broadcast to a function in a class")
	{
		ExampleObserver observer;
		REQUIRE(observer.eventCount == 0);

		eventManager.Subscribe<ExampleEvent>(std::bind(&ExampleObserver::HandlExample, &observer, std::placeholders::_1));
		eventManager.Broadcast<ExampleEvent>(ExampleEvent(expectedPayload));

		REQUIRE(observer.eventCount == 1);
		REQUIRE(observer.lastPayloadReceived == expectedPayload);
	}

	TEST_CASE("EventManager can broadcast an event no one is listening to")
	{
		eventManager.Broadcast<ExampleEvent>(ExampleEvent(expectedPayload));
	}
}

TEST_SUITE("Event Manager fires default ECS events")
{
	babs_ecs::ECSManager ecs;
	babs_ecs::Entity e0 = babs_ecs::Entity(0);
	Identity e0Identity;

	TEST_CASE("Entity created")
	{
		bool entityCreatedEventFired = false;
		ecs.events.Subscribe<babs_ecs::EntityCreated>([&](const babs_ecs::EntityCreated& e) {
			REQUIRE(e.entity.UUID == 1);
			entityCreatedEventFired = true;
			});

		e0 = ecs.CreateEntity();

		REQUIRE(entityCreatedEventFired == true);
	}

	TEST_CASE("Component Added")
	{
		bool eventCalled = false;
		ecs.RegisterComponent<Identity>();

		e0Identity.name = "Babs1";
		ecs.events.Subscribe<babs_ecs::ComponentAdded<Identity>>([&](const babs_ecs::ComponentAdded<Identity>& e) {
			REQUIRE(e.component.name == "Babs1");
			eventCalled = true;
			});

		ecs.AddComponent(e0, e0Identity);

		REQUIRE(eventCalled == true);
	}

	TEST_CASE("Component Removed")
	{
		bool eventCalled = false;
		ecs.RegisterComponent<Identity>();

		e0Identity.name = "Babs1";
		ecs.AddComponent(e0, e0Identity);

		ecs.events.Subscribe<babs_ecs::ComponentRemoved<Identity>>([&](const babs_ecs::ComponentRemoved<Identity>& e) {
			REQUIRE(e.component.name == "Babs1");
			eventCalled = true;
			});

		ecs.RemoveComponent<Identity>(e0);

		REQUIRE(eventCalled == true);
	}
}