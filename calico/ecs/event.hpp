#ifndef _CALICO_EVENT_HPP_
#define _CALICO_EVENT_HPP_

namespace Calico {

enum class EventType {
    None,
    KeyDown,
    KeyUp,
    Quit,
    MouseMoved,
};

struct Event {
private:
    std::any data = {};
public:
    EventType type = {};

    Event() = delete;
    explicit Event(EventType type) : type(type) {}

    template <typename Type, typename... Args>
    Event &set_param(Args&&... args) {
        data = Type(std::forward<Type>(args...));
        return *this;
    }

    template <typename T>
    std::optional<T> get_param() const {
        try {
            return std::any_cast<T>(data);
        } catch(const std::bad_any_cast &e) {
            /* log message */
            // std::printf("Failed to cast param to %s\n", typeid(T).name());
            return {};
        }
    }
};

}

#endif // _CALICO_EVENT_HPP_
