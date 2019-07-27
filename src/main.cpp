#include <iostream>
#include "bitfield.hpp"

struct Health
{
	int max;
	int current;


	std::string ToString() {
		return "health";
	}
};

struct Identity
{
	std::string name;


	std::string ToString() {
		return "identity";
	}
};

int main() 
{
	std::cout << "Hello World" << std::endl;

	return 0;
}