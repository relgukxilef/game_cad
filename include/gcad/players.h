#include <vector>
#include <optional>

#include "solver.h"

namespace gcad {
    using namespace std;

    struct move_t {
        unsigned move, observations;
    };

    struct player_t {
        // stores a replay and the position in the replay
        // moves selected by the solver are added at the end of the replay
        // this serves as a constraint on the sampled games
        vector<unsigned> observations;
        vector<move_t> moves;
        unsigned current_move = 0;
        unsigned current_observation = 0;
        unsigned replay_end = 0; // TODO: remove
    };

    struct players_t;

    struct player_ptr {
        optional<unsigned> choose(unsigned maximum);
        void see(unsigned value);
        void score(unsigned value);

        players_t sample(solver_t *solver);
        void resize(unsigned size);
        void input(unsigned value);
        
        players_t *players;
        unsigned index;
    };

    struct players_t {
        players_t(unsigned number_players = 1, solver_t *solver = nullptr);

        player_ptr operator[](unsigned index);
        void restart(); // TODO: remove
        unsigned size();
        
        unsigned current_player = 0;
        unsigned current_choice = 0;
        bool contradiction = false;
        vector<player_t> players;
        solver_t *solver;
    };
}
