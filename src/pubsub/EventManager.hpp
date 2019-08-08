#include <functional>
#include <map>
#include <vector>
#include <string>

#include "Event.hpp"

class EventManager
{
	public:
		using SlotType = std::function<void(const Event&)>;

        template <typename T>
		void Subscribe(SlotType&& slot)
		{
            std::string type = typeid(T).name();
			this->observers[type].push_back(slot);
		};

        template <typename T>
		void Broadcast(const Event& event) const
		{
			std::string type = typeid(T).name();

			if (observers.find(type) == observers.end())
			{
				// No subscribers for this event yet.
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