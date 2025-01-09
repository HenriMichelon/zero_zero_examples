module;
#include "libraries.h"

export module game.Doors;

import layers;

export namespace space_station {

    class Door : public z0::Node {
    public:
        void onReady() override;

    protected:
        void open() const;
        void close() const;
        void toggleState();

    private:
        list<shared_ptr<AnimationPlayer>> animationPlayers;
        bool isOpen{false};
    };

    class TunnelDoor : public Door {
    public:
        void onReady() override;
    };

    class SlideDoor : public Door {
    };

}