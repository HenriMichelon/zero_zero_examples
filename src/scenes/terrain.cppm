module;
#include "libraries.h"

export module Example:TerrainScene;

using namespace z0;

// Example of JSON scene file
export class TerrainScene : public Node {
public:
    void onReady() override {
        setProcessMode(PROCESS_MODE_ALWAYS);
        log("Loading scene");
        Loader::addSceneFromFile(this, "res/scenes/scene1.json");
    }

    ~TerrainScene() override = default;
};
