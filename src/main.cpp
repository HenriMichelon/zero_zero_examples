#include <z0/macros.h>
#include "libraries.h"

import Example;

const ApplicationConfig applicationConfig{
        .appName = "Example App",
        .appDir = "..",
        .windowMode = WINDOW_MODE_WINDOWED_FULLSCREEN,
        .windowWidth = 1920,
        .windowHeight = 1080,
        .defaultFontName = "res/Signwood.ttf",
        .defaultFontSize = 25,
        .loggingMode = LOGGING_FILE | LOGGING_WINDOW | LOGGING_STDOUT
};

// Register the examples node classes for the JSON loading
Z0_REGISTER_TYPE(Crate)
Z0_REGISTER_TYPE(Player)
Z0_REGISTER_TYPE(PlatformPlayer)
Z0_REGISTER_TYPE(Terrain)

Z0_APP(applicationConfig, make_shared<ExampleMainScene>())
