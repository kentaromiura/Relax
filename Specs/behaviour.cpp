#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../relax/relax.h"
#include <string>
#include <map>

using std::map;
using std::string;

SCENARIO( "Using relax for a simple count state", "[relax]" ) {

    // State may be anything in this scenario is a map string->int
    typedef map<string, int> State;

    // Actions can be anything as well, this case is simple enough
    // so an enum will work, if you need more detail you can use a custom object
    enum Actions {
        INIT,
        ADD,
        SUBTRACT
    };

    // thanks to c++14 we could use `auto` here,
    // leaving parameters to allow c++11 to build
    auto reducer = [](State state, Actions action){

        switch (action) {
            case INIT:
                state["count"] = 0;
                break;
            case ADD:
                state["count"]++;
                break;
            case SUBTRACT:
                state["count"]--;
                break;
        }
        return state;

    };

    // defining a store and passing the reducer function
    relax::Store<State, Actions> store(reducer);

    GIVEN("An empty state map") {
        auto& state = store.state;

        WHEN("Inspected before any action fired") {
            THEN("count doesn't exists") {
                REQUIRE(state.find("count") == state.end());
            }
        }

        WHEN("INIT get dispatched") {
            store.dispatch(Actions::INIT);
            THEN("count exists") {
                REQUIRE(state["count"] == 0);

                WHEN("ADD get dispatched") {
                    store.dispatch(Actions::ADD);
                    THEN("count is 1") {
                        REQUIRE(state["count"] == 1);

                        WHEN("ADD get dispatched again") {
                            store.dispatch(Actions::ADD);
                            THEN("count is 2"){
                                REQUIRE(state["count"] == 2);
                                for (auto i=0; i<10; i++){
                                    store.dispatch(Actions::ADD);
                                }
                                REQUIRE(state["count"] == 12);
                            }
                        }

                        WHEN("SUBTRACT get dispatched") {
                            store.dispatch(Actions::SUBTRACT);
                            THEN("count is 0") {
                                REQUIRE(state["count"] == 0);
                            }
                        }
                    }
                }
            }
        }

        WHEN("A callback gets attached") {
            uint64_t called = 0;
            auto unsubscribe = store.subscribe([&](){
                called ++;
            });

            THEN("After any action get fired, called increments") {
                REQUIRE(called == 0);
                store.dispatch(Actions::ADD);
                REQUIRE(called == 1);
                WHEN("Unsubscribed") {
                    unsubscribe();
                    THEN("called is not incremented anymore"){
                        store.dispatch(Actions::ADD);
                        store.dispatch(Actions::SUBTRACT);
                        REQUIRE(called == 1);
                    }
                }

                WHEN("Multiple subscription") {
                    uint64_t call_2 = 0;
                    uint64_t call_3 = 0;
                    auto unsubscribe_2 = store.subscribe([&](){
                        call_2 ++;
                    });
                    auto unsubscribe_3 = store.subscribe([&](){
                        call_3 ++;
                    });
                    THEN("A single dispatch trigger multiple callbacks") {
                        REQUIRE(called == 1);
                        REQUIRE(call_2 == 0);
                        REQUIRE(call_3 == 0);
                        store.dispatch(Actions::ADD);
                        REQUIRE(called == 2);
                        REQUIRE(call_2 == 1);
                        REQUIRE(call_3 == 1);
                        WHEN("The second unsubscribes") {
                            unsubscribe_2();
                            THEN("The other two still works") {
                                store.dispatch(Actions::ADD);
                                REQUIRE(called == 3);
                                REQUIRE(call_2 == 1);
                                REQUIRE(call_3 == 2);
                                WHEN("Unsubscribing the last one") {
                                    unsubscribe_3();
                                    THEN("called still increments") {
                                        store.dispatch(Actions::ADD);
                                        REQUIRE(called == 4);
                                        REQUIRE(call_2 == 1);
                                        REQUIRE(call_3 == 2);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
