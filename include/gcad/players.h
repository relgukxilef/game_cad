#include <vector>
#include <optional>

#include "solver.h"

namespace gcad {
    using namespace std;

    struct replay_t {
        struct edge {
            vector<unsigned> output;
            unsigned input;
        };

        struct player {
            vector<edge> moves;
            vector<unsigned> output;
        };
        
        vector<player> players;
    };

    struct player_ptr {
        optional<unsigned> replay(unsigned maximum);
        unsigned choose(unsigned maximum);
        void see(unsigned value);
        void score(unsigned value);
        
        struct players_t *players;
        unsigned index;
    };

    struct players_t {
        players_t(unsigned number_players = 1);

        player_ptr operator[](unsigned index);
        void restart();
        
        bool contradiction = false;
        replay_t current_game;
        replay_t filter;
        solver_t solver;
    };

}
