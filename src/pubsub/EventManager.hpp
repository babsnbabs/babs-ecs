#include <functional>
#include <map>
#include <vector>
#include <string>
#include <any>

#include "Event.hpp"

class EventManager
{
	public:
		using SlotType = std::function<void(const std::any&)>;

		template <typename T>
		void Subscribe(SlotType&& slot)
		{
			std::string type = typeid(T).name();
			this->observers[type].push_back(slot);
		}

		template <typename T>
		void Broadcast(const std::any& event) const
		{
			std::string type = typeid(T).name();

			if (observers.find(type) == observers.end())
			{
				return;
			}

			auto&& event_observers = this->observers.at(type);

			for (auto&& observer : event_observers)
			{
				observer(event);
			}
		}

private:
	std::map<std::string, std::vector<SlotType>> observers;
};