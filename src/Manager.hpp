#pragma once

#include "bitfield.hpp"
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>
#include <set>
#include <typeinfo>
#include <vector>


struct Entity
{
	bitfield::Bitfield bitfield;
	int32_t UUID;

	Entity(uint32_t uuid)
	{
		bitfield = 0;
		UUID = uuid;
	}
};

// This is needed to use Entity as a key in a map.
struct EntityComparer
{
	bool operator() (const Entity& lhs, const Entity& rhs) const
	{
		return lhs.UUID < rhs.UUID;
	}
};

// Exists to satisfying compiler warnings
class BaseContainer
{
public:
	BaseContainer() {  };
	virtual ~BaseContainer() {};
};

// This would be the concrete type created by RegisterComponent and inserted into the map
template <typename T>
class ComponentContainer : public BaseContainer
{
public:
	ComponentContainer() {};
	virtual ~ComponentContainer() {};
	std::map<Entity, T, EntityComparer> data;
};

class ECS {
public:
	ECS()
	{
		this->bitIndex = 1;
		this->entityIndex = 0;
	}

	Entity CreateEntity()
	{
		Entity e = Entity(this->entityIndex);
		this->entityIndex++;
		this->entities.push_back(e);

		return e;
	}

	template <typename T>
	void RegisterComponent(T component);

	template <typename T>
	void AddComponent(Entity entity, T component);

	template <typename T>
	T* GetComponent(Entity entity, T component);

	template<typename... Ts>
	std::vector<Entity*> EntitiesWith(Ts&& ... types);
private:
	int32_t entityIndex;
	bitfield::Bitfield bitIndex;
	std::vector<Entity> entities;

	std::map<std::string, BaseContainer*> components;
	std::map<std::string, bitfield::Bitfield> componentIndex;

	template <typename T>
	std::string GetComponentName(T component);

	std::vector<std::string> GetComponentNames(std::vector<std::string> names);

	template <typename T>
	std::vector<std::string> GetComponentNames(std::vector<std::string> names, T type);

	template <typename T, typename ...Ts>
	std::vector<std::string> GetComponentNames(std::vector<std::string> names, T type, Ts... types);

	bool ComponentIsRegistered(std::string componentName)
	{
		for (auto const& component : this->components)
		{
			if (component.first == componentName)
			{
				return true;
			}
		}

		return false;
	}
};

template<typename T>
inline void ECS::RegisterComponent(T component)
{
	// Example: "class TestComponent", "struct Health", "struct Identity"
	// using typeid(T).name() means we don't need to rely on ToString();
	std::string componentName = this->GetComponentName(component);

	if (components.find(componentName) == components.end())
	{
		componentIndex[componentName] = bitIndex;
		components[componentName] = new ComponentContainer<T>();

		unsigned int lastIndex = bitIndex;
		bitIndex *= 2;
		if (bitIndex < lastIndex)
		{
			throw std::out_of_range("Exceeded available flags for the bitfield! (max 32 b/c uint32)");
		}
	}
}

template<typename T>
inline void ECS::AddComponent(Entity entity, T component)
{
	std::string componentName = this->GetComponentName(component);

	if (!this->ComponentIsRegistered(componentName))
	{
		throw ComponentNotRegisteredException(componentName);
	}

	ComponentContainer<T>* container = dynamic_cast<ComponentContainer<T>*>(this->components[componentName]);
	container->data[entity] = component;
	int componentFlag = componentIndex[componentName];

	components[componentName] = container;

	bool entityFound = false;
	for (Entity& e : this->entities)
	{
		if (e.UUID == entity.UUID)
		{
			entityFound = true;
			e.bitfield = bitfield::Set(e.bitfield, componentFlag);
		}
	}

	if (!entityFound)
	{
		throw std::runtime_error("Failed to find entity to add component to");
	}
}

template<typename T>
inline T* ECS::GetComponent(Entity entity, T component)
{
	std::string componentName = this->GetComponentName(component);
	if (!this->ComponentIsRegistered(componentName))
	{
		throw ComponentNotRegisteredException(componentName);
	}

	int componentFlag = componentIndex[componentName];
	
	for (Entity e : this->entities)
	{
		if (e.UUID == entity.UUID)
		{
			if (bitfield::Has(e.bitfield, componentFlag))
			{
				ComponentContainer<T>* container = dynamic_cast<ComponentContainer<T>*>(this->components[componentName]);
				return &container->data[entity];
			}
		}
	}

	return NULL;
}

std::vector<std::string> ECS::GetComponentNames(std::vector<std::string> names)
{
	return names;
}

template <typename T>
std::vector<std::string> ECS::GetComponentNames(std::vector<std::string> names, T type)
{
	std::string name = this->GetComponentName(std::forward<T>(type));
	names.push_back(name);

	return names;
}

template <typename T, typename ...Ts>
std::vector<std::string> ECS::GetComponentNames(std::vector<std::string> names, T type, Ts... types)
{
	std::string name = this->GetComponentName(std::forward<T>(type));
	names.push_back(name);

	// Continue getting component neames until we are out of template arguments and return the list
	return this->GetComponentNames(names, std::forward<Ts>(types)...);
}

template<typename ...Ts>
inline std::vector<Entity*> ECS::EntitiesWith(Ts&& ...types)
{
	// build bitfield flags for this search
	std::vector<std::string> componentNames;
	componentNames = this->GetComponentNames(componentNames, std::forward<Ts>(types)...);

	for (auto componentName : componentNames)
	{
		if (!this->ComponentIsRegistered(componentName))
		{
			throw ComponentNotRegisteredException(componentName);
		}
	}

	bitfield::Bitfield field = 0;
	for (auto name : componentNames)
	{
		field = bitfield::Set(field, this->componentIndex[name]);
	}

	std::vector<Entity*> requestedEntities;
	requestedEntities.reserve(this->entities.size());
	for (auto& e : this->entities) {
		if (bitfield::Has(e.bitfield, field)) {
			requestedEntities.emplace_back(&e);
		}
	}

	return requestedEntities;
}

template<typename T>
inline std::string ECS::GetComponentName(T component)
{
	return typeid(component).name();
}