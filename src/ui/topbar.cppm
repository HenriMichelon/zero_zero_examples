module;
#include "../libraries.h"

export module example.TopBar;

export class TopBar : public ui::Window {
public:
    TopBar(Object *obj, const std::function<void()> &_onQuit);

    void onCreate() override;

    void updateFPS();

private:
    uint32_t          fps{0};
    shared_ptr<ui::Text> textFPS;
    Object *          onQuitHandler;
    std::function<void()>   onQuit;

    void onPauseToggle(ui::EventClick *event) const;
};
