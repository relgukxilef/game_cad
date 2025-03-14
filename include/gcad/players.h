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

    struct players_t;

    struct player_ptr {
        optional<unsigned> choose(unsigned maximum);
        void see(unsigned value);
        void score(unsigned value);

        players_t sample(solver_t *solver);
        
        players_t *players;
        unsigned index;
    };

    struct players_t {
        players_t(unsigned number_players = 1, solver_t *solver = nullptr);

        player_ptr operator[](unsigned index);
        void restart();
        
        bool contradiction = false;
        replay_t current;
        replay_t filter;
        solver_t *solver;
    };
}
