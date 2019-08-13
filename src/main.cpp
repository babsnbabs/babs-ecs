#include <iostream>
#include "Manager.hpp"

struct Health
{
	int max;
	int current;
};

struct Identity
{
	std::string name;
};

struct AI
{
	std::string difficulty;
};

int main()
{
	ECS ecs;
	ecs.RegisterComponent(Identity());
	ecs.RegisterComponent(Health());
	ecs.RegisterComponent(AI());

	Entity babs1 = ecs.CreateEntity();
	Health babs1Hp = Health{ 100, 44 };
	Identity babs1Ident = Identity{ "babs1" };
	ecs.AddComponent(babs1, babs1Hp);
	ecs.AddComponent(babs1, babs1Ident);

	Entity babs2 = ecs.CreateEntity();
	Health babs2Hp = Health{ 200, 144 };
	Identity babs2Ident = Identity{ "babs2" };
	ecs.AddComponent(babs2, babs2Hp);
	ecs.AddComponent(babs2, babs2Ident);

	Entity enemy = ecs.CreateEntity();
	AI enemyAi = AI{ "hard" };
	ecs.AddComponent(enemy, enemyAi);

	for (auto entity : ecs.EntitiesWith(Health{}))
	{
		std::cout << "entity with health: " << entity.UUID << std::endl;
	}

	return 0;
}