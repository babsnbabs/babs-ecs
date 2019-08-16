#pragma once

#include <string>
#include <map>
#include <iostream>
#include <stdexcept>
#include <set>
#include <typeinfo>
#include <vector>
#include <tuple>
#include <algorithm>
#include <queue>

#include "bitfield/bitfield.hpp"
#include "Exceptions.hpp"
#include "Entity.hpp"
#include "events/EventManager.hpp"
#include "Events.hpp"

namespace babs_ecs
{

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

    // ECSManageris the manager of the whole dealio.
    class ECSManager {
    public:

        events::EventManager events;

        ECSManager()
        {
            this->bitIndex = 1;
            this->entityIndex = 0;
        }

        // CreateEntity will initialize and return a new entity with no components.
        Entity CreateEntity()
        {
            uint32_t entityId = -1;

            if (!this->unusedEntityIndices.empty())
            {
                entityId = this->unusedEntityIndices.front();
                this->unusedEntityIndices.pop();
            }
            else
            {
                entityId = this->entityIndex;
                entityIndex++;
            }

            Entity e = Entity(entityId);
            this->entities.push_back(e);

            EntityCreated entityCreated(e);
            this->events.Broadcast(entityCreated);
            return e;
        }

        template <typename T>
        void RegisterComponent();

        template <typename T>
        void AddComponent(Entity entity, T component);

        template <typename T>
        void RemoveComponent(Entity entity);

        template <typename T>
        T* GetComponent(Entity entity);

        template<typename... Ts>
        std::vector<Entity> EntitiesWith();

        template <typename T>
        bool HasComponent(Entity entity);

        void RemoveEntity(Entity entity)
        {
            uint32_t entityId = entity.UUID;
            size_t originalSize = this->entities.size();
            auto erase = this->entities.erase(std::remove(this->entities.begin(), this->entities.end(), entity), this->entities.end());

            if (originalSize == this->entities.size())
            {
                throw EntityNotFoundException(entityId);
            }

            this->unusedEntityIndices.push(entityId);

            std::map<std::string, std::vector<Entity>>::iterator it;
            for (it = this->individualComponentVecs.begin(); it != this->individualComponentVecs.end(); ++it)
            {
                std::string name = it->first;

                auto it = this->individualComponentVecs[name].begin();
                while (it != this->individualComponentVecs[name].end())
                {
                    if (entity.UUID == it->UUID)
                    {
                        this->individualComponentVecs[name].erase(it);
                        break;
                    }
                    ++it;
                }
            }
        }

    private:
        std::queue<uint32_t> unusedEntityIndices;
        uint32_t entityIndex;
        bitfield::Bitfield bitIndex;
        std::vector<Entity> entities;

        std::map<std::string, BaseContainer*> components;
        std::map<std::string, bitfield::Bitfield> componentIndex;

        std::map<std::string, std::vector<Entity>> individualComponentVecs;

        template <typename T>
        std::string GetComponentName();

        template <typename T, typename... Ts>
        std::vector<std::string> GetComponentNames();

        bool ComponentIsRegistered(std::string componentName)
        {
            return (this->components.count(componentName) > 0);
        }
    };

    // RegisterComponent will let ECS know of a new component type it needs to keep track of.
    //
    // Until this is called, components cannot be added/retrieved.
    template<typename T>
    inline void ECSManager::RegisterComponent()
    {
        // Example: "class TestComponent", "struct Health", "struct Identity"
        // using typeid(T).name() means we don't need to rely on ToString();
        std::string componentName = this->GetComponentName<T>();

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
    inline void ECSManager::AddComponent(Entity entity, T component)
    {
        std::string componentName = this->GetComponentName<T>();

        if (!this->ComponentIsRegistered(componentName))
        {
            throw babs_ecs::ComponentNotRegisteredException(componentName);
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
                babs_ecs::ComponentAdded componentAdded(entity, component);
                this->events.Broadcast(componentAdded);
            }
        }

        if (!entityFound)
        {
            throw std::runtime_error("Failed to find entity to add component to");
        }
    }

    // GetComponent will return a pointer to the entities component data. Modifications to the component will persist.
    template<typename T>
    inline void ECSManager::RemoveComponent(Entity entity)
    {
        std::string componentName = this->GetComponentName<T>();

        if (!this->ComponentIsRegistered(componentName))
        {
            throw babs_ecs::ComponentNotRegisteredException(componentName);
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

                if (componentVector == this->individualComponentVecs.end())
                {
                    // Nothing to remove
                    return;
                }

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
                        babs_ecs::ComponentRemoved componentRemoved(entity, componentData);
                        this->events.Broadcast(componentRemoved);
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
    inline T* ECSManager::GetComponent(Entity entity)
    {
        std::string componentName = this->GetComponentName<T>();
        if (!this->ComponentIsRegistered(componentName))
        {
            throw babs_ecs::ComponentNotRegisteredException(componentName);
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

    template<typename T, typename... Ts>
    inline std::vector<std::string> ECSManager::GetComponentNames()
    {
        std::vector<std::string> names;

        if constexpr(sizeof...(Ts) == 0)
        {
            // this is effectively the "base case" if you were to think of this like recursion
            names.push_back(typeid(T).name());
        }
        else
        {
            // we must still have more component names to retrieve, continue templating
            names = GetComponentNames<Ts...>();
            names.push_back(GetComponentNames<T>()[0]);
        }

        return names;
    }

    // Returns a list of Entity pointers of entities matching the provided list of component types.
    //
    // If no component types are provided, all entities will be returned.
    // 
    // Typical usage: auto entities = ecs.EntitiesWith<Identity, Health>();
    template<typename ...Ts>
    inline std::vector<Entity> ECSManager::EntitiesWith()
    {
        // build bitfield flags for this search
        std::vector<std::string> componentNames;
        if constexpr(sizeof...(Ts) > 0)
        {
            componentNames = this->GetComponentNames<Ts...>();
        }

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
                throw babs_ecs::ComponentNotRegisteredException(componentName);
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
    inline bool ECSManager::HasComponent(Entity entity)
    {
        return this->GetComponent<T>(entity) != nullptr ? true : false;
    }

    template<typename T>
    inline std::string ECSManager::GetComponentName()
    {
        return typeid(T).name();
    }

}
