module;
#include "libraries.h"

export module game.State;

export namespace space_station {

    struct State  {
        static constexpr auto MAX_STATE_NAME{32};
        char name[MAX_STATE_NAME];

        State(const string&name);
    };

    struct PlayerState : State {
        vec3 position{VEC3ZERO};
        vec3 rotation{VEC3ZERO};

        PlayerState(): State{"player"} {}
    };

}