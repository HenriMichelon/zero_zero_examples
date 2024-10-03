module;
#include "libraries.h"

export module Example:TerrainScene;

using namespace z0;

// Example of JSON scene file
export class TerrainScene : public Node {
public:
    void onReady() override;

    ~TerrainScene() override = default;
};
