module;
#include "libraries.h"

export module Example:Terrain;

import :Layers;

using namespace z0;

// Physic body for the terrain
export class Terrain : public StaticBody {
public:
    Terrain();

    void onReady() override ;
};
