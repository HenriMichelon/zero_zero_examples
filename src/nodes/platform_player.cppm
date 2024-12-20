module;

export module example.PlatformPlayer;

import example.Player;

export class PlatformPlayer : public Player {
public:
    PlatformPlayer() {
        cameraCollisions = false;
    }

};
