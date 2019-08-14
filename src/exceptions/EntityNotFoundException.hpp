#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <exception>

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
