#include <iostream>
#include "temp/math.hpp"
#include "bitfield.hpp"
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

int main() {

	ECS ecs;

	Entity babs1 = ecs.CreateEntity();
	Identity ident = Identity{ "babs1" };
	ecs.RegisterComponent<Identity>(ident);
	ecs.AddComponent<Identity>(babs1, ident);

	Entity babs2 = ecs.CreateEntity();
	ident = Identity{ "babs2" };
	ecs.AddComponent<Identity>(babs2, ident);
	Health health = Health { 100, 82 };
	
	ecs.RegisterComponent<Health>(health);
	ecs.AddComponent<Health>(babs2, health);
	std::cout << "Added health component to Babs2, Max: " << health.max << " current: " << health.current << std::endl;

	// This does not get the Health Component attached to babs2
	Health* babs2Hp = ecs.GetComponent(babs2, Health());
	std::cout << "Retrived health component for Babs2: Max: " << babs2Hp->max << " Current: " << babs2Hp->current << std::endl;

	return 0;
}