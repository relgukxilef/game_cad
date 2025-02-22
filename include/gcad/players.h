#include <vector>
#include <unordered_map>
#include <random>
#include <functional>

#include "solver.h"

namespace gcad {
    using namespace std;

    struct player_ptr {
        struct players_t *players;
        unsigned index;

        unsigned choose(unsigned maximum);
        void see(unsigned value);
        void score(unsigned value);
    };

    struct players_t {
        players_t(unsigned number_players = 1);

        player_ptr operator[](unsigned index);
        void restart(); 
        // TODO: maybe store state during a game in separate class

        struct edge {
            vector<unsigned> output;
            unsigned input;
        };
        
        // players moves and outputs for the current game
        vector<vector<edge>> moves;
        vector<vector<unsigned>> output;

        solver_t solver;
    };

}
