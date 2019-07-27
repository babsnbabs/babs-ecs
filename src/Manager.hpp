#pragma once

#include "bitfield.hpp"
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>
#include <set>
#include <typeinfo>

typedef uint32_t Entity;

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
	std::map<Entity, T> data;
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
		Entity e = this->entityIndex;
		this->entityIndex++;

		bitfield::Bitfield bit = 0;
		this->entities[e] = bit;

		return e;
	}

	template <typename T>
	void RegisterComponent(T component);

	template <typename T>
	void AddComponent(Entity entity, T component);

	template <typename T>
	T* GetComponent(Entity entity, T component);
private:
	Entity entityIndex;
	bitfield::Bitfield bitIndex;
	std::map<Entity, bitfield::Bitfield> entities;

	std::map<std::string, BaseContainer*> components;
	std::map<std::string, bitfield::Bitfield> componentIndex;

	template <typename T>
	std::string GetComponentName(T component);
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

		std::cout << "added " << componentName << " to ECS" << std::endl;
	}
	else // key already registered!
	{
		std::cout << componentName << " is already a registered component!" << std::endl;
	}
}

template<typename T>
inline void ECS::AddComponent(Entity entity, T component)
{
	std::string componentName = this->GetComponentName(component);
	ComponentContainer<T>* container = dynamic_cast<ComponentContainer<T>*>(this->components[componentName]);
	container->data[entity] = component;
	int componentFlag = componentIndex[componentName];

	entities.insert(std::pair<Entity, bitfield::Bitfield>(entities[entity], componentFlag));
	components[componentName] = container;

	entities[entity] = bitfield::Set(entities[entity], componentFlag);

	std::cout << "added data for " << componentName << " to ECS for entity " << entity << std::endl;
	std::cout << " container has " << container->data.size() << " components in it" << std::endl;
}

template<typename T>
inline T* ECS::GetComponent(Entity entity, T component)
{
	std::string componentName = this->GetComponentName(component);
	int componentFlag = componentIndex[componentName];

	if (bitfield::Has(entities[entity], componentFlag))
	{
		ComponentContainer<T>* container = dynamic_cast<ComponentContainer<T>*>(this->components[componentName]);
		return &container->data[entity];
	}

	return NULL;
}

template<typename T>
inline std::string ECS::GetComponentName(T component)
{
	return typeid(component).name();
}
