module;
#include <z0/z0.h>
using namespace z0;

export module Example:TopBar;

class TopBar : public GWindow {
public:
    TopBar(Object* obj, Signal::Handler _onQuit):
        GWindow(Rect{0, 900, 1000, 100}),
        onQuitHandler{obj},
        onQuit{_onQuit} {
    }

    void onCreate() override {
        //getWidget().setDrawBackground(false);
        //getWidget().setFont(make_shared<Font>(getWidget().getFont()->getFontName(), 40));

        const auto rightPadding = make_shared<GPanel>();
        rightPadding->setDrawBackground(false);
        getWidget().add(rightPadding, GWidget::RIGHT, "5,5");

        textFPS = make_shared<GText>("9999");
        getWidget().add(textFPS, GWidget::RIGHTCENTER);
        textFPS->setTextColor(Color{1.0, 1.0, 0.2});

        const auto buttonQuit = make_shared<GButton>();
        buttonQuit->connect(GEvent::OnClick, onQuitHandler, onQuit);
        getWidget().add(buttonQuit, GWidget::LEFTCENTER, "10,10");
        const auto textQuit = make_shared<GText>("Menu");
        buttonQuit->add(textQuit, GWidget::CENTER, "");
        buttonQuit->setSize(textQuit->getWidth() + 20, textQuit->getHeight() + 20);
        setHeight(buttonQuit->getHeight());

        const auto buttonPause = make_shared<GButton>();
        buttonPause->connect(GEvent::OnClick, this, Signal::Handler(&TopBar::onPauseToggle));
        getWidget().add(buttonPause, GWidget::LEFTCENTER, "10,10");
        const auto textPause = make_shared<GText>("Pause");
        buttonPause->add(textPause, GWidget::CENTER, "");
        buttonPause->setSize(textPause->getWidth() + 20, textPause->getHeight() + 20);

        //setHeight(buttonQuit->getHeight());
        setY(1000 - getHeight());
        hide();
    }

    void updateFPS() {
        if (isVisible()) {
            if (app().isPaused()) {
                textFPS->setText("Pause");
            }
            else {
                auto newFPS = app().getFPS();
                if (newFPS != fps) {
                    fps = newFPS;
                    textFPS->setText(to_string(fps));
                }
            }
        }
    }

private:
    uint32_t fps{0};
    shared_ptr<GText> textFPS;
    Object* onQuitHandler;
    Signal::Handler onQuit;

    void onPauseToggle(GEventClick* event) {
        Application::get().setPaused(!app().isPaused());
        log("Pause ", to_string(Application::get().isPaused()));
        event->consumed = true;
    }
};
