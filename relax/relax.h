#ifndef RELAX_H
#define RELAX_H
#include <vector>
#include <algorithm>
#include <functional>
#include <exception>

namespace relax {

using std::vector;
using std::pair;
using std::function;
using std::logic_error;

template<typename State, typename Action>
class Store {
    vector<pair<uint64_t, function<void(void)>>> listeners;
    function<State(State, Action)> reducer;
    uint64_t counter = 0;
    bool isDispatching = false;
public:
    State state;
    Store(function<State(State, Action)> r): reducer(r){};

    function<void(void)> subscribe(function<void(void)> listener){
        uint64_t id = counter++;
        listeners.push_back({
            id,
            listener
        });

        return [=](){
            listeners.erase(
                remove_if(
                    listeners.begin(),
                    listeners.end(),
                    [&](pair<uint64_t, function<void(void)>> pair) {
                        return pair.first == id;
                    }
                )
            );
        };
    }

    Action dispatch(Action action) {
        if (isDispatching){
            logic_error e("Reducers cannot dispatch actions");
            throw e;
        }
        isDispatching = true;

        state = reducer(state, action);
        isDispatching = false;
        for (auto l : listeners) l.second();

        return action;
    }
};

}
#endif
