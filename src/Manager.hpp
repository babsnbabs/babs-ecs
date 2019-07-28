#pragma once

#include "bitfield.hpp"
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>
#include <set>
#include <typeinfo>
#include <vector>

typedef uint32_t Entity;
	
template<typename T>
class ComponentVector
{
	std::vector<T> Components;
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

	template<typename... Ts>
	std::vector<Entity> EntitiesWith(Ts&&... types);
private:
	Entity entityIndex;
	bitfield::Bitfield bitIndex;
	std::map<Entity, bitfield::Bitfield> entities;

	std::map<std::string, BaseContainer*> components;
	std::map<std::string, bitfield::Bitfield> componentIndex;

	template <typename T>
	std::string GetComponentName(T component);

    std::vector<std::string> GetComponentNames(std::vector<std::string> names);

    template <typename T>
    std::vector<std::string> GetComponentNames(std::vector<std::string> names, T type);

    template <typename T, typename ...Ts>
    std::vector<std::string> GetComponentNames(std::vector<std::string> names, T type, Ts... types);
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
    
    // recursion basically - we keep showing up in this function, and eventually
    // we hit the base case of one type and no list of types, so we get sent to the
    // function above and then everything returns back to the caller.
    this->GetComponentNames(names, std::forward<Ts>(types)...);

    return names;
}


template <typename... Types>
inline std::vector<Entity> ECS::EntitiesWith(Types&&... types)
{
    // build bitfield flags for this search
    std::vector<std::string> componentNames;
    componentNames = this->GetComponentNames(componentNames, std::forward<Types>(types)...);

    bitfield::Bitfield field = 0;
    for (auto name : componentNames)
    {
        field = bitfield::Set(field, this->componentIndex[name]);
    }

    // search time
    std::vector<Entity> requestedEntities;
    for (auto mapEntry : this->entities) {
        std::cout << "  checking entity " << mapEntry.first << std::endl;
        if (bitfield::Has(mapEntry.second, field)) {
            std::cout << "    adding" << std::endl;
            requestedEntities.push_back(mapEntry.first);
        }
    }
		
	return requestedEntities;
}

template<typename T>
inline std::string ECS::GetComponentName(T component)
{
	return typeid(component).name();
}