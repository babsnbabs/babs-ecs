//#include <entityplus/entity.h>
//#include <entityx/entityx.h>
#include <chrono>
#include <iostream>
#include <string>

#include "Manager.hpp"

class Timer {
	std::chrono::high_resolution_clock::time_point start;
	std::string name;
public:
	Timer(std::string name)
	{
		this->start = std::chrono::high_resolution_clock::now();
		this->name = name;
	}
	~Timer()
	{
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << this->name << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "\n";
	}
};

struct Identity
{
	int uuid;
};

struct Tag {};

void babsEcsTest(int entityCount, int iterationCount, int tagProb)
{
	ECS ecs;
	ecs.RegisterComponent(Identity());
	ecs.RegisterComponent(Tag());

	//Entity entity1 = ecs.CreateEntity();
	   //Identity ident{ "babs1" };
	   //ecs.AddComponent(entity1, ident);

	std::cout << "Babs ECS\n";
	{
		Timer timer("Add entities: ");
		for (int i = 0; i < entityCount; ++i) {
			auto entity = ecs.CreateEntity();
			ecs.AddComponent(entity, Identity{ i });

			if (i % tagProb == 0)
			{
				ecs.AddComponent(entity, Tag{});
			}
		}
	}
	{
		Timer timer("For_each entities w/ Identity: ");
		std::uint64_t sum = 0;
		for (int i = 0; i < iterationCount; ++i) {
			auto entities = ecs.EntitiesWith(Identity{});
			for (auto entity : entities)
			{
				sum++;
			}
		}
		std::cout << sum << "\n";
	}
	{
		Timer timer("For_each entities w/ Tag: ");
		std::uint64_t sum = 0;
		for (int i = 0; i < iterationCount; ++i) {
			auto entities = ecs.EntitiesWith(Tag{});
			for (auto entity : entities)
			{
				sum++;
			}
		}
		std::cout << sum << "\n";
	}
}

// void entPlusTest(int entityCount, int iterationCount, int tagProb) {
//  using namespace entityplus;
//  entity_manager<component_list<int>, tag_list<struct Tag>> em;
//  em.create_grouping<int, Tag>();
//  std::cout << "EntityPlus\n";
//  {
//   Timer timer("Add entities: ");
//   for (int i = 0; i < entityCount; ++i) {
//    auto ent = em.create_entity();
//    ent.add_component<int>(i);
//    if (i % tagProb == 0)
//     ent.set_tag<Tag>(true);
//   }
//  }
//  {
//   Timer timer("For_each entities: ");
//   std::uint64_t sum = 0;
//   for (int i = 0; i < iterationCount; ++i) {
//    em.for_each<Tag, int>([&](auto ent, auto i) {
//     sum += i;
//    });
//   }
//   std::cout << sum << "\n";
//  }
// }

// void entXTest(int entityCount, int iterationCount, int tagProb) {
//  using namespace entityx;
//  struct Tag {};
//  entityx::EntityX ex;
//  std::cout << "EntityX\n";
//  {
//   Timer timer("Add entities: ");
//   for (int i = 0; i < entityCount; ++i) {
//    auto ent = ex.entities.create();
//    ent.assign<int>(i);
//    if (i % tagProb == 0)
//     ent.assign<Tag>();
//   }
//  }
//  {
//   Timer timer("For_each entities: ");
//   std::uint64_t sum = 0;
//   for (int i = 0; i < iterationCount; ++i) {
//    ex.entities.each<Tag, int>([&](auto ent, auto &, auto i) {
//     sum += i;
//    });
//   }
//   std::cout << sum << "\n";
//  }
// }

void runTest(int entityCount, int iterationCount, int tagProb) {
	std::cout << "Count: " << entityCount
		<< " ItrCount: " << iterationCount
		<< " TagProb: " << tagProb << "\n";
	babsEcsTest(entityCount, iterationCount, tagProb);
	//entPlusTest(entityCount, iterationCount, tagProb);
	//std::cout << "\n";
	//entXTest(entityCount, iterationCount, tagProb);
	std::cout << "\n\n";
}

int main() {
	runTest(1'000, 1'000'000, 3);
	//runTest(10'000, 1'000'000, 3);
	//runTest(30'000, 100'000, 3);
	//runTest(100'000, 100'000, 5);
	//runTest(10'000, 1'000'000, 1'000);
	//runTest(100'000, 1'000'000, 1'000);
}