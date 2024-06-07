#include "includes.h"
#include "player.h"
#include "crate.h"
#include "layers.h"
#include "raycast.h"
#include "topbar.h"

void RaycastMainScene::onReady() {
    auto player = make_shared<Player>();
    addChild(player);
    player->setPosition({0.0, 0.5, 0.0});

    auto crateModel = Loader::loadModelFromFile("res/models/crate.glb", true);
    for (int x = 0; x < 5; x++) {
        for (int z = 0; z < 5; z++) {
            auto model = make_shared<Crate>(crateModel->duplicate());
            model->setPosition({x * 5 - 1.5 * 5, 1.0 + rand() % 5, -z * 5 - 5});
            addChild(model);
        }
    }

    auto floor = make_shared<StaticBody>(
            make_shared<BoxShape>(vec3{200.0f, 0.2f, 200.0f}),
            Layers::WORLD,
            0,
            "Floor");
    floor->addChild(Loader::loadModelFromFile("res/models/floor.glb", true));
    floor->setPosition({0.0, -2.0, 0.0});
    addChild(floor);

    auto topbar = make_shared<TopBar>(this, GEventFunction(&RaycastMainScene::onMenuQuit));
    app().addWindow(topbar);
}

void RaycastMainScene::onMenuQuit(GWidget &, GEvent *) {
    app().quit();
}
