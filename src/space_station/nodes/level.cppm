module;
#include "libraries.h"

export module game.Level;

export namespace space_station {

    class Level : public z0::Node {
    public:
        static inline Signal::signal on_loaded = "on_level_loaded";

        explicit Level(const string& name) : z0::Node(name) {}
        void onEnterScene() override;
    };

}