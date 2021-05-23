#include <iostream>
#include <functional>
#include <vector>

using namespace std;

template<typename... T>
using EventHandler = std::function<bool (T...)>;

template<typename... T>
class EventDispatcher {
public:
    EventDispatcher() {}

    bool operator()(T... arg) const {
        for (EventHandler<T...> handler : handlers) {
            if (!handler(arg...)) {
                return false;
            }
        }
        
        return true;
    }
    void operator+=(EventHandler<T...> handler) {
        handlers.push_back(handler);
    }

private:
    std::vector<EventHandler<T...>> handlers;
};

using ScrollEventDispatcher = EventDispatcher<const double&, double>;

int main() {
    ScrollEventDispatcher event;

    event += [](const double& xoffset, double yoffset) -> bool {
        cout << "cancel!" << endl;
        return true;
    };
    event += [](const double& xoffset, double yoffset) -> bool {
        cout << "scroll event : " << xoffset << ", " << yoffset << endl;
        return true;
    };

    cout << event(0, 1.2345) << endl;

    return 0;
}