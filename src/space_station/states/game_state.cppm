module;
#include "libraries.h"

export module game.GameState;

import game.State;

export namespace space_station {

    class GameState {
    public:
        PlayerState         player;
        map<string, bool>   levelsLoadingState;

        void save() const;
        void load();

        GameState(const string &fileName) : fileName{std::move(fileName)} {}
        GameState(const GameState&) = delete;
        GameState& operator=(const GameState&) = delete;

    private:
        const string fileName;


        template<typename T>
        void saveState(ostream& stream, const T& state) const {
            stream.write(reinterpret_cast<const char *>(&state), sizeof(T));
        }

        template<typename T>
        void loadState(istream& stream, T& state) {
            stream.read(reinterpret_cast<char *>(&state), sizeof(T));
        }
    };

}