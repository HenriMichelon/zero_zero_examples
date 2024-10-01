module;
#include "libraries.h"

export module Example:PlatformPlayer;

import :Player;

export class PlatformPlayer : public Player {
public:
    PlatformPlayer() {
        cameraCollisions = false;
    }
};
