#include <vector>
#include <optional>

#include "solver.h"

namespace gcad {
    using namespace std;

    struct move_t {
        unsigned move, observations;
        unsigned weight; // 0 for moves that should be ignored during backprop
    };

    enum struct policy_t {
        best, explore, random,
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
        // TODO: maybe choose should return unsigned
        optional<unsigned> choose(unsigned maximum);
        void see(unsigned value);
        void score(unsigned value);

        // TODO: rename sample to fork
        players_t sample(solver_t *solver);
        void resize(unsigned size);
        void input(unsigned value);
        statistics get_expected_score(unsigned choice);

        players_t *players;
        unsigned index;
    };

    // TODO: rename to replay_t
    struct players_t {
        players_t(unsigned number_players = 1, solver_t *solver = nullptr);

        player_ptr operator[](unsigned index);
        void restart(); // TODO: remove
        unsigned size();

        void see_all(unsigned value);

        unsigned current_player = 0;
        unsigned current_choice = 0;
        unsigned constrained_players = 0;
        bool contradiction = false;
        vector<player_t> players;
        solver_t *solver;
    };
}
