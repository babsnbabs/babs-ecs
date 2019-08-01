#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <exception>

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
