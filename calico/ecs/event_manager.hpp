#ifndef _CALICO_EVENT_MANAGER_HPP_
#define _CALICO_EVENT_MANAGER_HPP_

#include <unordered_map>
#include <list>

#include "event.hpp"

namespace Calico {

#define EVENT_LISTENER(event, listener) event, std::bind(&listener, this, std::placeholders::_1)

using EventHandler = std::function<void(const Event&)>;

class EventManager {
    std::unordered_map<EventType, std::list<EventHandler>> event_handlers {};
public:
    inline void add_listener(const EventType type, EventHandler &&handler) {
        event_handlers[type].push_back(handler);
    }

    void broadcast(const Event &e) const {
        if (!event_handlers.contains(e.type)) {
            return;
        }

        for (const auto &handler : event_handlers.at(e.type)) {
            handler(e);
        }
    }
};

}

#endif // _CALICO_EVENT_MANAGER_HPP_
