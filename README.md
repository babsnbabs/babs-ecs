# Babs-ECS
[![Build Status](https://travis-ci.org/babsnbabs/babs-ecs.svg?branch=master)](https://travis-ci.org/babsnbabs/babs-ecs)

A fast, simple, and cross platform C++17 header-only Entity Component System


## Overview

There are quite a few Entity Component Systems available in C++:
* [EntityPlus](https://github.com/Yelnats321/EntityPlus)
* [EntityX](https://github.com/alecthomas/entityx)
* [anax](https://github.com/miguelmartin75/anax)
* [Artemis](https://github.com/vinova/Artemis-Cpp)

This library will seem very familiar at its core and usage. It exists because we wanted a specific set of capabilities as well as the speed, but wanted a manageable codebase to work with in the event that we want to improve it. Some design goals included the following:
* simple, small codebase
* cross platform (linux, windows, and osx)
* well documented and tested
* fastest in the west
* friendly error output on misconfiguration


## Downloading and Including

This library is header only, so just grab the [latest release](https://github.com/babsnbabs/babs-ecs/releases) and add `#include "babs-ecs/Manager.hpp"`.

If you're interested in running the tests and/or the benchmark, follow the following steps will get those built. CMake is configured to download the necessary dependencies automatically:
1. `cd babs-ecs`
2. `mkdir build`
3. `cd build`
4. `cmake ..`
5. `make`

You should then see the executables `./tests` and `./babs-benchmark` available to run.


## Tutorial

As mentioned above, this usage will be unsurprising to anyone who has used other ECS libraries. Our library is comprised of entities, components, and events.

### Setting up the ECS Manager

Everything revolves around an ECS Manager instance. This instance will keep track of all the entities and components, as well as provide the interface for entity searching and component retrieval.

All you need to do is create an instance of the manager and you're ready to go:

```c++
#include "babs-ecs/Manager.hpp"

ECS ecs;
```

### Entities

Entities are easy to create and effectively just a unique identifier within the manager. It's safe to store the entities in your own systems, but make sure to go through the manager when checking the latest component availability.

```c++
#include "babs-ecs/Manager.hpp"

ECS ecs;

Entity entity = ecs.CreateEntity();
entity.UUID // returns 0 since it's the first
```

### Components

Components can be defined freely and registered with ECS for subsequent use. They can be as complicated as you need, so feel free to add methods.

```c++
#include <string>
#include <iostream>
#include "babs-ecs/Manager.hpp"

struct Identity {
    std::string name;
};

ECS ecs;

ecs.RegisterComponent(Identity());
```

Once the component is registered, instances of it can be assigned to specific entities, retrieved later, and removed. An exception will be thrown if you try to access or remove component data that doesn't exist.

```c++
Entity player = ecs.CreateEntity();
Identity player_identity{"babs"};

ecs.AddComponent(player, player_identity);

Identity ident = ecs.GetComponent(Identity());
std::cout << "players name is " << ident.name << std::endl;

ecs.RemoveComponent(player, Identity());
```

### Events

Event systems work well with ECS for de-coupled communication between systems. Events are as easy as components to work with. These don't require registration, and you can subscribe and broadcast at any time through the event manager provided by the ECS instance.

```c++
#include "babs-ecs/Manager.hpp"

ECS ecs;

struct CollisionEvent
{
    Entity e1;
    Entity e2;

    CollisionEvent(Entity e1, Entity e2) : e1(e1), e2(e2) {}
};

ecs.eventManager.Subscribe<CollisionEvent>([&](const CollisionEvent& e) {
    std::cout << "entity " << e1.UUID << " and entity " << e2.UUID << " collided!" << std::endl; 
});

Entity e1 = ecs.CreateEntity();
Entity e2 = ecs.CreateEntity();

ecs.eventManager.Broadcast<CollisionEvent>(CollisionEvent(e1, e2));
```

The following events can be subscribed to, and are broadcasted by ECS automatically:

* `EntityCreated` - when a new entity is created, provides a copy of the entity
* `ComponentAdded<MyComponent>` - when a component is added to an entity, provides the entity and component data
* `ComponentRemoved<MyComponent>` - when a component is removed from an entity, provides the entity and component data


## Special Thanks

Special thanks to [Bastian Rieck](https://github.com/Pseudomanifold) for writing a great article on making a [Simple Event System](https://bastian.rieck.ru/blog/posts/2015/event_system_cxx11/) for which our Publisher-Subscriber system is derived.
