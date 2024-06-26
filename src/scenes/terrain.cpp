#include "includes.h"
#include "layers.h"
#include "nodes/player.h"
#include "nodes/crate.h"
#include "nodes/terrain.h"
#include "scenes/terrain.h"

class Sun: public DirectionalLight {
public:
    Sun(): DirectionalLight{
        vec3{-0.2f, -0.8f, 1.0f},
        vec4{1.0f, 1.0f, 1.0f, 1.0f}
    } {
        setCastShadow(true);
    };
};
Z0_REGISTER_TYPE(Sun)

void TerrainScene::onReady() {
    addChild(make_shared<Environment>(vec4{1.0,1.0,1.0,1.0f}));

    setProcessMode(PROCESS_MODE_ALWAYS);
    auto game = make_shared<Node>("Game");
    game->setProcessMode(PROCESS_MODE_PAUSABLE);
    addChild(game);
    
    Loader::addSceneFromFile(game, "res/scenes/scene1.json");
}
