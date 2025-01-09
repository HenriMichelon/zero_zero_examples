module;
#include "libraries.h"

module game.GameState;

namespace space_station {

    void GameState::save() const {
        auto stream = VirtualFS::openWriteStream(fileName);
        saveState(stream, player);
        size_t size = levelsLoadingState.size();
        saveState(stream, size);
        for (const auto& state : levelsLoadingState) {
            size_t keySize = state.first.size();
            saveState(stream, keySize);
            stream.write(state.first.data(), keySize);
            saveState(stream, state.second);
        }
    }

    void GameState::load() {
        if (!VirtualFS::fileExists(fileName)) { return; }
        auto stream = VirtualFS::openReadStream(fileName);
        loadState(stream, player);
        size_t size = 0;
        stream.read(reinterpret_cast<char*>(&size), sizeof(size));
        for (size_t i = 0; i < size; ++i) {
            size_t keySize = 0;
            loadState(stream, keySize);
            std::string key(keySize, '\0');
            stream.read(key.data(), keySize);
            bool value = false;
            loadState(stream, value);
            levelsLoadingState[key] = value;
        }
    }

}