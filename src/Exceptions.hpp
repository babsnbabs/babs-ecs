#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <exception>

namespace babs_ecs
{
    struct EntityNotFoundException : public std::exception
    {
    public:
        EntityNotFoundException(uint32_t id) : entityId(id)
        {
            std::cerr << this->entityId<< " was not found." << std::endl;
        }

    private:
        uint32_t entityId;
    };


    struct ComponentNotRegisteredException : public std::exception
    {
    public:
        ComponentNotRegisteredException(std::string componentName) : componentNotRegistered(componentName) 
        {
            std::cerr << this->componentNotRegistered << " must be registered before being used." << std::endl;
        }

    private:
        std::string componentNotRegistered;
    };
}