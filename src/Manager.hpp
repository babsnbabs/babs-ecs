#pragma once

#include "bitfield.hpp"
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>
#include <set>
#include <typeinfo>
#include <vector>
#include <tuple>
#include <algorithm>
#include "exceptions/ComponentNotRegisteredException.hpp"
#include "Entity.hpp"
#include "pubsub/PubSub.hpp"

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
	BaseContainer() {};
	virtual ~BaseContainer() {};
};

// This would be the concrete type created by RegisterComponent and inserted into the map.
// This container will hold all of the component data for a specific component type.
template <typename T>
class ComponentContainer : public BaseContainer
{
public:
	ComponentContainer() {};
	virtual ~ComponentContainer() {};
	std::map<Entity, T, EntityComparer> data;
};

// ECS is the manager of the whole dealio.
class ECS {
public:
	ECS()
	{
		this->bitIndex = 1;
		this->entityIndex = 0;
	}

	// CreateEntity will initialize and return a new entity with no components.
	Entity CreateEntity()
	{
		Entity e = Entity(this->entityIndex);
		this->entityIndex++;
		this->entities.push_back(e);


		EntityCreated entityCreated(e);
		this->eventManager.Broadcast(entityCreated);
		return e;
	}

	EventManager eventManager;

	template <typename T>
	void RegisterComponent(T component);

	template <typename T>
	void AddComponent(Entity entity, T component);

	template <typename T>
	void RemoveComponent(Entity entity, T component);

	template <typename T>
	T* GetComponent(Entity entity, T component);

	template<typename... Ts>
	std::vector<Entity> EntitiesWith(Ts&& ... types);

	template <typename T>
	bool HasComponent(Entity entity, T component);

private:
	uint32_t entityIndex;
	bitfield::Bitfield bitIndex;
	std::vector<Entity> entities;

	std::map<std::string, BaseContainer*> components;
	std::map<std::string, bitfield::Bitfield> componentIndex;

	std::map<std::string, std::vector<Entity>> individualComponentVecs;

	template <typename T>
	std::string GetComponentName(T component);

	std::vector<std::string>* GetComponentNames(std::vector<std::string>* names);

	template <typename T>
	std::vector<std::string>* GetComponentNames(std::vector<std::string>* names, T type);

	template <typename T, typename ...Ts>
	std::vector<std::string>* GetComponentNames(std::vector<std::string>* names, T type, Ts... types);

	bool ComponentIsRegistered(std::string componentName)
	{
		return (this->components.count(componentName) > 0);
	}
};

// RegisterComponent will let ECS know of a new component type it needs to keep track of.
//
// Until this is called, components cannot be added/retrieved.
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

		// set the next bit index
		unsigned int lastIndex = bitIndex;
		bitIndex *= 2;
		if (bitIndex < lastIndex)
		{
			throw std::out_of_range("Exceeded available flags for the bitfield! (max 32 b/c uint32)");
		}
	}
}

// AddComponent will add the component to the entity. It can be retrieved later with ecs.GetComponent(...)
template<typename T>
inline void ECS::AddComponent(Entity entity, T component)
{
	std::string componentName = this->GetComponentName(component);

	if (!this->ComponentIsRegistered(componentName))
	{
		throw ComponentNotRegisteredException(componentName);
	}

	// get the container for this component and add the component data to this entity
	ComponentContainer<T>* container = dynamic_cast<ComponentContainer<T>*>(this->components[componentName]);
	container->data[entity] = component;
	int componentFlag = componentIndex[componentName];

	// now we find and update this entity
	// we do this because we can't trust the provided entity's bitfield is up to date
	bool entityFound = false;
	for (Entity& e : this->entities)
	{
		if (e.UUID == entity.UUID)
		{
			entityFound = true;
			e.bitfield = bitfield::Set(e.bitfield, componentFlag);

			// make sure this entity is in the component specific list of entities
			auto iter = this->individualComponentVecs.find(componentName);
			if (iter != this->individualComponentVecs.end())
			{
				iter->second.push_back(e);
			}
			else
			{
				std::vector<Entity> entityList;
				entityList.push_back(e);
				this->individualComponentVecs.insert(std::pair<std::string, std::vector<Entity>>(componentName, entityList));
			}

			std::map<std::string, std::vector<Entity>>::iterator it;
			for (it = this->individualComponentVecs.begin(); it != this->individualComponentVecs.end(); ++it)
			{
				auto copiedEntity = std::find(it->second.begin(), it->second.end(), e);
				if (copiedEntity != it->second.end())
				{
					copiedEntity->bitfield = e.bitfield;
				}
			}

			// fire the component added event
			ComponentAdded componentAdded(entity, component);
			this->eventManager.Broadcast(componentAdded);
		}
	}

	if (!entityFound)
	{
		throw std::runtime_error("Failed to find entity to add component to");
	}
}

// GetComponent will return a pointer to the entities component data. Modifications to the component will persist.
template<typename T>
inline void ECS::RemoveComponent(Entity entity, T component)
{
	std::string componentName = this->GetComponentName(component);

	if (!this->ComponentIsRegistered(componentName))
	{
		throw ComponentNotRegisteredException(componentName);
	}

	int componentFlag = componentIndex[componentName];

	// look for and update the entity
	bool entityFound = false;
	for (Entity& e : this->entities)
	{
		if (e.UUID == entity.UUID)
		{
			entityFound = true;

			// first we clear its bitfield
			e.bitfield = bitfield::Clear(e.bitfield, componentFlag);

			// now we remove it from the component specific list
			auto componentVector = this->individualComponentVecs.find(componentName);
			for (std::vector<Entity>::iterator it = componentVector->second.begin(); it != componentVector->second.end(); ++it)
			{
				if (it->UUID == e.UUID)
				{
					ComponentContainer<T>* container = dynamic_cast<ComponentContainer<T>*>(this->components[componentName]);
					T componentData = container->data[entity];

					componentVector->second.erase(it);

					std::map<std::string, std::vector<Entity>>::iterator it;
					for (it = this->individualComponentVecs.begin(); it != this->individualComponentVecs.end(); ++it)
					{
						auto copiedEntity = std::find(it->second.begin(), it->second.end(), e);
						if (copiedEntity != it->second.end())
						{
							copiedEntity->bitfield = e.bitfield;
						}
					}

					// fire the component removed event
					ComponentRemoved componentRemoved(entity, componentData);
					this->eventManager.Broadcast(componentRemoved);
					break;
				}
			}

			break;
		}
	}

	if (!entityFound)
	{
		throw std::runtime_error("Failed to find entity to remove component from");
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

	// first find the entity, then check for its component data
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

	return nullptr;
}

// This is the base case of the GetComponentNames recursion, it exists only to stop the recursion.
inline std::vector<std::string>* ECS::GetComponentNames(std::vector<std::string>* names)
{
	return names;
}

// This ends up being either the entry point and/or 2nd to last case of the GetComponentNames recursion.
// It will call the function above.
template <typename T>
inline std::vector<std::string>* ECS::GetComponentNames(std::vector<std::string>* names, T type)
{
	std::string name = this->GetComponentName(std::forward<T>(type));
	names->push_back(name);

	return names;
}

// This is an entry point for the GetComponentNames recursion. It will call of version of itself
// or the function above.
template <typename T, typename ...Ts>
inline  std::vector<std::string>* ECS::GetComponentNames(std::vector<std::string>* names, T type, Ts... types)
{
	std::string name = this->GetComponentName(std::forward<T>(type));
	names->push_back(name);

	// Continue getting component neames until we are out of template arguments and return the list
	return this->GetComponentNames(names, std::forward<Ts>(types)...);
}

// Returns a list of Entity pointers of entities matching the provided list of component types.
//
// If no component types are provided, all entities will be returned.
// 
// Typical usage: auto entities = ecs.EntitiesWith(Identity(), Health());
template<typename ...Ts>
inline std::vector<Entity> ECS::EntitiesWith(Ts&& ...types)
{
	// build bitfield flags for this search
	std::vector<std::string> componentNames;
	this->GetComponentNames(&componentNames, std::forward<Ts>(types)...);

	// if no components were provided, we'll return all entities
	if (componentNames.size() == 0)
	{
		// Asking for all entities
		std::vector<Entity> requestedEntities;
		for (auto e : this->entities)
		{
			requestedEntities.push_back(e);
		}
		return requestedEntities;
	}

	// looks like they asked for 1+ components. first we'll get the current size of
	// each component specific list
	std::vector<std::tuple<std::string, int>> componentListSizes;
	for (auto componentName : componentNames)
	{
		if (!this->ComponentIsRegistered(componentName))
		{
			throw ComponentNotRegisteredException(componentName);
		}

		auto vec = &this->individualComponentVecs[componentName];
		componentListSizes.push_back(std::make_tuple(componentName, vec->size()));
	}

	// now we'll build our search bitfield
	bitfield::Bitfield field = 0;
	for (auto name : componentNames)
	{
		field = bitfield::Set(field, this->componentIndex[name]);
	}

	// grab our smallest component list 
	std::string smallestComponentList = std::get<0>(*std::min_element(begin(componentListSizes), end(componentListSizes), [](auto lhs, auto rhs) {return std::get<1>(lhs) < std::get<1>(rhs); }));
	auto entitySearchVector = this->individualComponentVecs[smallestComponentList];

	// using the smallest list as our base, we'll check each entity against the
	// search bitfield
	std::vector<Entity> requestedEntities;
	requestedEntities.reserve(entitySearchVector.size());
	for (auto e : entitySearchVector) {
		if (bitfield::Has(e.bitfield, field)) {
			requestedEntities.emplace_back(e);
		}
	}

	return requestedEntities;
}

// Returns the compiler created string for this component. We don't actually care what the
// string is, but generally it seems to match the type name.
template<typename T>
inline bool ECS::HasComponent(Entity entity, T component)
{
	return this->GetComponent(entity, component) != nullptr ? true : false;
}

template<typename T>
inline std::string ECS::GetComponentName(T component)
{
	return typeid(component).name();
}