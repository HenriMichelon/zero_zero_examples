module;
#include "libraries.h"

module game.Level;

namespace space_station {
    void Level::onEnterScene() {
        emit(on_loaded, this);
    }
}