#ifndef H_LUVOASI_EVENT
#define H_LUVOASI_EVENT

#include <string>
#include <functional>
#include <vector>

#include "luvoasi/core/base.h"

namespace Luvoasi {

template<typename... T>
using EventHandler = std::function<bool (T...)>;

/**
 * @brief Create event channel.
 * @details
 * Supports C# like event dispatch style.
 */
template<typename... T>
class EventDispatcher {
public:
    bool Dispatch(T... arg) const;
    void AddHandler(EventHandler<T...> handler);
    void RemoveHandler(EventHandler<T...> handler);

    bool operator()(T... arg) const;
    void operator+=(EventHandler<T...> handler);
    void operator-=(EventHandler<T...> handler);

private:
    std::vector<EventHandler<T...>> handlers;
};

class Event {
public:
    virtual ~Event() = default;

    virtual std::wstring GetName() const = 0;
};

}
#include "luvoasi/event.tcc"

#endif