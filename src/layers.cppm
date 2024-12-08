export module Example:Layers;

// values used in the layers & masks for the collisions detection
export enum Layers {
    NONE        = 0,
    WORLD       = 1,
    BODIES      = 2,
    PLAYER      = 3,
    LAYERS_COUNT= PLAYER+1,
};

