#pragma once

#include "Entity.hpp"

namespace babs_ecs
{
	struct EntityCreated
	{
		Entity entity;
		EntityCreated(Entity entity) : entity(entity) {}
	};

	template <typename T>
	struct ComponentAdded
	{
		Entity entity;
		T component;

		ComponentAdded(Entity entity, T component) : entity(entity), component(component) {}
	};

	template <typename T>
	struct ComponentRemoved
	{
		Entity entity;
		T component;

		ComponentRemoved(Entity entity, T component) : entity(entity), component(component) {}
	};
}
