module;
#include "../libraries.h"

export module Example:TopBar;

class TopBar : public GWindow {
public:
    TopBar(Object *obj, const std::function<void()> &_onQuit);

    void onCreate() override;

    void updateFPS();

private:
    uint32_t          fps{0};
    shared_ptr<GText> textFPS;
    Object *          onQuitHandler;
    std::function<void()>   onQuit;

    void onPauseToggle(GEventClick *event) const;
};
