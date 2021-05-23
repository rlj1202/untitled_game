#include "luvoasi/event.h"

namespace Luvoasi {

template<typename... T>
bool EventDispatcher<T...>::Dispatch(T... arg) const {
    for (EventHandler<T...> handler : handlers) {
        if (!handler(arg...)) {
            return false;
        }
    }

    return true;
}

template<typename... T>
void EventDispatcher<T...>::AddHandler(EventHandler<T...> handler) {
    handlers.push_back(handler);
}

template<typename... T>
void EventDispatcher<T...>::RemoveHandler(EventHandler<T...> handler) {
    handlers.erase(
        std::remove(handlers.begin(), handlers.end(), handler),
        handlers.end()
    );
}

template<typename... T>
bool EventDispatcher<T...>::operator()(T... arg) const {
    return Dispatch(arg...);
}

template<typename... T>
void EventDispatcher<T...>::operator+=(EventHandler<T...> handler) {
    AddHandler(handler);
}

template<typename... T>
void EventDispatcher<T...>::operator-=(EventHandler<T...> handler) {
    RemoveHandler(handler);
}

}
