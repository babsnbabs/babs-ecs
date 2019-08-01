#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <exception>

struct ComponentNotRegisteredException : public std::exception
{
public:
	ComponentNotRegisteredException(std::string componentName) : componentNotRegistered(componentName) {}

	const char* what() const throw()
	{
		std::stringstream ss;
		ss << this->componentNotRegistered << " must be registered before being used." << std::endl;

		return ss.str().c_str();
	}

private:
	std::string componentNotRegistered;
};
