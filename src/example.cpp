#include <z0/macros.h>
#include "libraries.h"
import Z0;
using namespace z0;

import Example;

const ApplicationConfig applicationConfig {
    .appName = "Example App",
    .appDir = "..",
    .windowMode = WINDOW_MODE_WINDOWED,
    .windowWidth = 800,
    .windowHeight = 600,
    .defaultFontName = "res/Signwood.ttf",
    .defaultFontSize = 25,
    .loggingMode = LOGGING_FILE | LOGGING_WINDOW | LOGGING_STDOUT
};

Z0_REGISTER_TYPE(Crate)
Z0_REGISTER_TYPE(Player)
Z0_REGISTER_TYPE(Terrain)

Z0_APP(applicationConfig, make_shared<ExampleMainScene>())
