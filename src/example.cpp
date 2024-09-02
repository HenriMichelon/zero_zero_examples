#include <z0/z0.h>
using namespace z0;

import Example;

const ApplicationConfig applicationConfig {
    .appName = "Example App",
    .appDir = "..",
    .windowMode = WINDOW_MODE_WINDOWED,
    .windowWidth = 1280,
    .windowHeight = 1024,
    .defaultFontName = "res/Signwood.ttf",
    .defaultFontSize = 25,
    .loggingMode = LOGGING_FILE | LOGGING_WINDOW | LOGGING_STDOUT
};

Z0_APP(applicationConfig, make_shared<ExampleMainScene>())
