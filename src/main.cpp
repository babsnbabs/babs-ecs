#include <iostream>
#include "Manager.hpp"

struct Health
{
	int max;
	int current;


	std::string ToString() {
		return "health";
	}
};

struct Identity
{
	std::string name;


	std::string ToString() {
		return "identity";
	}
};

int main() 
{
	ECS ecs;
	ecs.RegisterComponent(Identity());
	ecs.RegisterComponent(Health());
	Identity babs1Ident{ "Babs1 Biddy Babs" };
	Entity babs1 = ecs.CreateEntity();
	ecs.AddComponent(babs1, babs1Ident);

	// Should have identity now
	Identity* b1IdentityRetrieved = ecs.GetComponent(babs1, Identity());
	if (b1IdentityRetrieved == NULL)
	{
		throw std::runtime_error("Expected component on entity");
	}

	std::cout << b1IdentityRetrieved->name << std::endl;
	return 0;
}