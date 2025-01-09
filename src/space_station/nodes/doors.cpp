module;
#include "libraries.h"

module game.Doors;

namespace space_station {

    void Door::open() const {
        if (!isOpen) {
            for(auto& animPlayer : animationPlayers) {
                if (!animPlayer->isPlaying()) { animPlayer->play(); }
            }
        }
    }

    void Door::close() const {
        if (isOpen) {
            for(auto& animPlayer : animationPlayers) {
                if (!animPlayer->isPlaying()) { animPlayer->playBackwards(); }
            }
        }
    }

    void Door::toggleState() {
        isOpen = !isOpen;
    }

    void Door::onReady() {
        animationPlayers = this->findAllChildren<AnimationPlayer>();
        animationPlayers.front()->connect(AnimationPlayer::on_playback_finish, [this]{toggleState(); });

        const auto areaOpen = make_shared<CollisionArea>(
                    make_shared<SphereShape>(2.0f),
                    TRIGGERS, "TunnelDoorOpen");
        areaOpen->setPosition({0.0,-1.4,0.0});
        areaOpen->connect(CollisionObject::on_collision_starts, [this]{open(); });
        addChild(areaOpen);
    }

    void TunnelDoor::onReady() {
        Door::onReady();
        const auto areaClose = make_shared<CollisionArea>(
                make_shared<SphereShape>(1.5f),
                TRIGGERS, "TunnelDoorClose");
        areaClose->setPosition({0.0,-1.5,-6.0});
        areaClose->connect(CollisionObject::on_collision_starts, [this]{close(); });
        addChild(areaClose);

        const auto areaClose1 = make_shared<CollisionArea>(
                make_shared<SphereShape>(1.5f),
                TRIGGERS, "TunnelDoorClose1");
        areaClose1->setPosition({0.0,-2.0,4.0});
        areaClose1->connect(CollisionObject::on_collision_starts, [this]{close(); });
        addChild(areaClose1);
    }
}