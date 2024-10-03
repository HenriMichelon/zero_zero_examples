module;
#include "../libraries.h"

export module Example:TopBar;

class TopBar : public GWindow {
public:
    TopBar(Object *obj, const Signal::Handler _onQuit);

    void onCreate() override;

    void updateFPS();

private:
    uint32_t          fps{0};
    shared_ptr<GText> textFPS;
    Object *          onQuitHandler;
    Signal::Handler   onQuit;

    void onPauseToggle(GEventClick *event) const;
};
