module;
#include "libraries.h"

export module game.MainScene;

import game.Player;
import game.Level;
import game.GameState;

export namespace space_station {

    class MainScene : public Node {
    public:
        MainScene();

        void onReady() override;
        void onExitScene() override;
        void onProcess(float alpha) override;
        bool onInput(InputEvent &inputEvent) override;

    private:
        GameState               gameState;
        shared_ptr<MyPlayer>      player{};
        shared_ptr<Environment> environment{};
        shared_ptr<Level>       level1_0{};
        shared_ptr<Level>       level1_1{};
        bool                    isLoading{false};
        shared_ptr<ui::Window>  screenWindow;
//        shared_ptr<ui::Window>  menu;
        shared_ptr<ui::Text>    textLoading;
        shared_ptr<ui::Image>   iconProcessing;
//        shared_ptr<ui::Text>    textFPS{};
        uint32_t                fps{0};

        string level_1_0_name;
        string level_1_1_name;

        static constexpr string_view level_1_0_filename {"app://res/scenes/level_1/level_1.json"};
        static constexpr string_view level_1_1_filename {"app://res/scenes/level_1/level_1_1.json"};

        void buildUI();
        void loadScene();
        void updateFPS();
        void onMenuQuit() const;
        void onLevelLoad(const shared_ptr<Node> &level);
        void onLevelUnload(const shared_ptr<Node> &level);
    };
}