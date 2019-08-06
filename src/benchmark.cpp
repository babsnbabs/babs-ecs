#include <chrono>
#include <iostream>
#include <string>
#include <iomanip>

#include "Manager.hpp"

class Timer {
public:
	std::chrono::milliseconds elapsed;
	std::chrono::high_resolution_clock::time_point start;

	Timer() : elapsed(std::chrono::milliseconds(0))
	{
		this->start = std::chrono::high_resolution_clock::now();
	}
	
	void End()
	{
		this->elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	}
};

void printHeader()
{
	std::cout << "|-------------------------------------------------------------------------------------------------------- |" << std::endl;
	std::cout << "|Name" << "\t\t\t" << "Entities" << "\t" << "Iterations" << "\t\t" << "Probability" << "\t" << "Duration(ms)\t\t  |" << std::endl;
	std::cout << "|-------------------------------------------------------------------------------------------------------- |" << std::endl;

}

void printResults(std::string name, int entities, int iterations, int probability, std::chrono::milliseconds duration)
{
	std::cout << "|" << name << "\t\t" << entities << "\t\t" << iterations << "\t\t\t" << "1/" <<probability << "\t\t\t" << duration.count() << "\t\t  |" << std::endl;
}

void printFooter()
{
	std::cout << "|-------------------------------------------------------------------------------------------------------- |" << std::endl;
}

struct Identity
{
	int uuid;
};

struct Tag {};

void babsEcsTest(int entityCount, int iterationCount, int tagProb)
{
	ECS ecs;
	ecs.RegisterComponent(Identity());
	ecs.RegisterComponent(Tag());

	{
		Timer timer;
		for (int i = 0; i < entityCount; ++i) {
			auto entity = ecs.CreateEntity();
			ecs.AddComponent(entity, Identity{ i });

			if (i % tagProb == 0)
			{
				ecs.AddComponent(entity, Tag{});
			}
		}
	}
	{
		Timer timer;

        std::uint64_t sum = 0;
        for (int i = 0; i < iterationCount; ++i) {
            auto entities = ecs.EntitiesWith(Identity{}, Tag{});
            for (auto entity : entities)
            {
                sum++;
            }
        }
        timer.End();
        printResults("Identity + Tag", entityCount, iterationCount, tagProb, timer.elapsed);
	}
}

void runTest(int entityCount, int iterationCount, int tagProb) {
	babsEcsTest(entityCount, iterationCount, tagProb);
}

int main() {
	std::cout << "Running benchmark..." << std::endl << std::endl;
	printHeader();
	runTest(1'000, 100'000, 3);
	runTest(10'000, 100'000, 3);
	runTest(30'000, 10'000, 3);
	runTest(100'000, 10'000, 5);
	runTest(10'000, 100'000, 1'000);
	runTest(100'000, 100'000, 1'000);
	printFooter();
}