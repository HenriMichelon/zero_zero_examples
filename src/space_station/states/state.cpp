module;
#include "libraries.h"

module game.State;

namespace space_station {

    State::State(const string& name) {
        memset(this->name, 0, MAX_STATE_NAME);
        strncpy(this->name, name.c_str(), MAX_STATE_NAME-1);
        this->name[MAX_STATE_NAME - 1] = '\0';
    }
}