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
        const auto usage = (app().getVideoMemoryUsage() / static_cast<float>(app().getDedicatedVideoMemory())) * 100.0f;
        log("VRAM usage", to_string(usage) + "%");
        printTree();
    }

    ~TerrainScene() override = default;
};
