#include "includes.h"
#include "topbar.h"

TopBar::TopBar(GEventHandler* handler, GEventFunction handlerQuit):
    GWindow(Rect{0, 945, 1000, 55}),
    onQuitHandler{handler},
    onQuit{handlerQuit}
{}

void TopBar::onCreate() {
    getWidget().setDrawBackground(false);
    getWidget().setFont(make_shared<Font>(getWidget().getFont()->getFontName(),
                                          getWidget().getFont()->getFontSize() / 1.5));

    auto rightPadding = make_shared<GPanel>();
    rightPadding->setDrawBackground(false);
    getWidget().add(rightPadding, GWidget::RIGHT, "5,5");

    textFPS = make_shared<GText>("9999");
    getWidget().add(textFPS, GWidget::RIGHTCENTER);
    textFPS->setTextColor(Color{1.0, 1.0, 0.2});

    auto buttonQuit = make_shared<GButton>();
    buttonQuit->connect(GEvent::OnClick, onQuitHandler, onQuit);
    getWidget().add(buttonQuit, GWidget::LEFTCENTER, "10,10");
    auto textQuit = make_shared<GText>("Menu");
    buttonQuit->add(textQuit, GWidget::CENTER, "");
    buttonQuit->setSize(textQuit->getWidth() + 20, textQuit->getHeight() + 20);
    setHeight(buttonQuit->getHeight());
    
    auto buttonPause = make_shared<GButton>();
    buttonPause->connect(GEvent::OnClick, this, GEventFunction(&TopBar::onPauseToggle));
    getWidget().add(buttonPause, GWidget::LEFTCENTER, "10,10");
    auto textPause = make_shared<GText>("Pause");
    buttonPause->add(textPause, GWidget::CENTER, "");
    buttonPause->setSize(textPause->getWidth() + 20, textPause->getHeight() + 20);
    
    setHeight(buttonQuit->getHeight());
    setY(1000 - getHeight());
    hide();
}

void TopBar::onPauseToggle(GWidget*, GEvent*) {
    Application::get().setPaused(!app().isPaused());
    log("Pause ", to_string(Application::get().isPaused()));
}

void TopBar::updateFPS() {
    if (isVisible()) {
        if (app().isPaused()) {
            textFPS->setText("Pause");
        } else {
            auto newFPS = app().getFPS();
            if (newFPS != fps) {
                fps = newFPS;
                textFPS->setText(to_string(fps));
            }
        }
    } 
}