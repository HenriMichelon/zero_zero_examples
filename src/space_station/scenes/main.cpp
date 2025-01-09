module;
#include "libraries.h"

module game.MainScene;

import game.Player;
import game.GameState;

namespace space_station {

    MainScene::MainScene():
        Node{"MainScene"},
        gameState{"app://savegame.save"},
        level_1_0_name(sanitizeName((level_1_0_filename.data()))),
        level_1_1_name(sanitizeName((level_1_1_filename.data()))) {
    }

    void MainScene::onReady() {
        gameState.levelsLoadingState[level_1_0_name] = true;
        gameState.levelsLoadingState[level_1_1_name] = false;
        gameState.load();

        buildUI();
        setProcessMode(ProcessMode::PAUSABLE);
        environment = make_shared<Environment>(vec4{1.0f, 1.0f, 1.0f, .01f});
        addChild(environment);
        player = make_shared<MyPlayer>();
        player->setPosition(gameState.player.position == VEC3ZERO ? vec3{0.0f, -1.0f, 6.0f} : gameState.player.position);
        player->setRotation(gameState.player.rotation);
        // player->setPosition({31.972490,-3.870000,-5.145043});
        addChild(player);
        loadScene();
    }

    bool MainScene::onInput(InputEvent &inputEvent) {
        if (inputEvent.getType() == InputEventType::KEY) {
            const auto& keyInputEvent = reinterpret_cast<const InputEventKey&>(inputEvent);
            if (keyInputEvent.isPressed()) {
                switch(keyInputEvent.getKey()) {
                case KEY_F1 :
                    loadScene();
                    return true;
                case KEY_F2 :
                    app().setDisplayDebug(!app().getDisplayDebug());
                    return true;
                case KEY_F3 : {
                    const auto stairs = level1_0->findFirstChild("stair_1");
                    if (stairs) {
                        stairs->setVisible(!stairs->isVisible());
                    }
                    return true;
                }
                case KEY_F4 : {
                    auto color = environment->getAmbientColorAndIntensity();
                    color.w = 1.0 - color.w;
                    environment->setAmbientColorAndIntensity(color);
                    return true;
                }
                case KEY_F5 : {
                    log(to_string(player->getPositionGlobal()));
                    return true;
                }
                default:
                    break;
                }
            }
        }
        return false;
    }

    void MainScene::onProcess(const float alpha) {
        // updateFPS();
    }

    void MainScene::buildUI() {
        constexpr float padding = 5.0f;

        screenWindow = make_shared<ui::Window>(ui::FULLSCREEN);
        app().add(screenWindow);
        screenWindow->getWidget().setPadding(padding);

//        menu = make_shared<ui::Window>(ui::Rect{0, 0, 1000, 10});
//        app().add(menu);
//        menu->setVisible(false);
//        menu->getWidget().setPadding(padding);

        textLoading = make_shared<ui::Text>("Loading level...");
        textLoading->setFont(make_shared<Font>(textLoading->getFont(), 60));
        textLoading->setTextColor(vec4{0.3, 0.3, 1.0, 1.0f});

//        const auto textQuit   = make_shared<ui::Text>("Quit");
//        textQuit->setTextColor(vec4{1.0, 1.0, 0.2, 1.0f});
//        const auto buttonQuit = make_shared<ui::Button>();
//        menu->add(buttonQuit, ui::Widget::LEFTCENTER, "50,40");
//        buttonQuit->add(textQuit, ui::Widget::CENTER);
//        buttonQuit->connect(ui::Event::OnClick, [this]{this->onMenuQuit();});

//        menu->setHeight(textQuit->getHeight() + padding * 4);
//        menu->setY(VECTOR_SCALE.y - menu->getHeight());

        // textFPS = make_shared<ui::Text>("------ FPS");
        // menu->add(textFPS, ui::Widget::RIGHTCENTER);
        // textFPS->setTextColor(vec4{1.0, 1.0, 0.2, 1.0f});

        iconProcessing = make_shared<ui::Image>(z0::Image::load("app://res/waiting.png"));
    }

    void MainScene::onLevelLoad(const shared_ptr<Node>& level) {
        if (!level->isInsideTree()) {
            screenWindow->add(iconProcessing, ui::Widget::BOTTOMRIGHT);
            addChild(level, true);
        }
    }

    void MainScene::onLevelUnload(const shared_ptr<Node>& level) {
        if (level->isInsideTree()) {
            removeChild(level, true);
            gameState.levelsLoadingState[level->getName()] = false;
        }
    }

    void MainScene::loadScene() {
        if (isLoading) { return; }
        isLoading= true;
        app().setPaused(true);
//        menu->setVisible(false);
        screenWindow->add(iconProcessing, ui::Widget::BOTTOMRIGHT);
        screenWindow->add(textLoading, ui::Widget::CENTER);

        auto onLevelLoaded = [this](void* param) {
            const auto level = static_cast<Level*>(param);
            gameState.levelsLoadingState[level->getName()] = true;
            log("loaded", level->getName());
            for (const auto & animPlayer :  level->findAllChildren<AnimationPlayer>()) {
                if (animPlayer->getCurrentAnimation() == "ventilation") {
                    animPlayer->getAnimation()->setLoopMode(AnimationLoopMode::LINEAR);
                    animPlayer->play();
                }
            }
            level->setVisible(true);
            app().callDeferred([this] {
                screenWindow->remove(iconProcessing);
            });
        };

        if (level1_0) {
            removeChild(level1_0);
            removeChild(level1_1);
        }
        app().callAsync([this, onLevelLoaded] {

            level1_0 = Loader::load<Level>(level_1_0_filename.data());
            level1_1 = Loader::load<Level>(level_1_1_filename.data());

            auto levelTrigger = level1_0->findFirstChild("level_1_1_load");
            if (levelTrigger) {
                levelTrigger->connect(CollisionArea::on_collision_starts, [this] {
                    this->onLevelLoad(level1_1);
                });
            }
            levelTrigger = level1_0->findFirstChild("level_1_1_unload");
            if (levelTrigger) {
                levelTrigger->connect(CollisionArea::on_collision_starts, [this] {
                    this->onLevelUnload(level1_1);
                });
            }
            levelTrigger = level1_1->findFirstChild("level_1_0_load");
            if (levelTrigger) {
                levelTrigger->connect(CollisionArea::on_collision_starts, [this] {
                    this->onLevelLoad(level1_0);
                });
            }
            levelTrigger = level1_1->findFirstChild("level_1_0_unload");
            if (levelTrigger) {
                levelTrigger->connect(CollisionArea::on_collision_starts, [this] {
                    this->onLevelUnload(level1_0);
                });
            }

            level1_0->setVisible(false);
            level1_0->connect(Level::on_loaded, onLevelLoaded);
            level1_1->setVisible(false);
            level1_1->connect(Level::on_loaded, onLevelLoaded);

            // printTree();

            app().callDeferred([this] {
                if (gameState.levelsLoadingState[level_1_0_name]) { addChild(level1_0); }
                if (gameState.levelsLoadingState[level_1_1_name]) { addChild(level1_1); }
                app().setPaused(false);
                const auto usage = (app().getVideoMemoryUsage() / static_cast<float>(app().getDedicatedVideoMemory())) * 100.0f;
                log("VRAM usage after loadScene():", to_string(usage)+"%");
                screenWindow->remove(textLoading);
//                menu->setVisible(true);
                isLoading= false;
            });
        });
    }

    void MainScene::updateFPS() {
//        if (app().isPaused()) {
//            textFPS->setText("Pause");
//        } else {
//            if (const auto newFPS = app().getFPS(); newFPS != fps) {
//                fps = newFPS;
//                textFPS->setText(to_string(fps) + " FPS");
//            }
//        }
    }

    void MainScene::onMenuQuit() const {
        app().quit();
    }

    void MainScene::onExitScene() {
        gameState.player.position = player->getPositionGlobal();
        gameState.player.rotation = player->getRotation();
        gameState.save();
    }

}