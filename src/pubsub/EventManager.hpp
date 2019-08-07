#include <functional>
#include <map>
#include <vector>

#include "Event.hpp"

class EventManager
{
	public:
		using SlotType = std::function<void(const Event&)>;

		void Subscribe(const Event::DescriptorType& descriptor, SlotType&& slot)
		{
			this->observers[descriptor].push_back(slot);
		};

		void Broadcast(const Event& event) const
		{
			auto type = event.Type();

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
	std::map<Event::DescriptorType, std::vector<SlotType>> observers;
};