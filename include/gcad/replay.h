#include <vector>
#include <optional>

#include "solver.h"

namespace gcad {
    struct move_t {
        unsigned move, observations;
        float weight; // 0 for moves that should be ignored during backprop
    };

    struct player_t {
        // stores a replay and the position in the replay
        // moves selected by the solver are added at the end of the replay
        // this serves as a constraint on the sampled games
        std::vector<unsigned> observations;
        std::vector<move_t> moves;
        unsigned current_move = 0;
        unsigned current_observation = 0;
        unsigned replay_end = 0; // TODO: remove
    };

    struct replay_t;

    struct player_ptr {
        // TODO: maybe choose should return unsigned
        std::optional<unsigned> choose(unsigned maximum, uint64_t mask = ~0);
        void see(unsigned value);
        void score(float value);

        // TODO: rename sample to fork
        replay_t sample(solver_t *solver);
        void resize(unsigned size);
        void input(unsigned value);
        statistics get_expected_score(unsigned choice);

        replay_t *players;
        unsigned index;
    };

    // TODO: rename to replay_t
    struct replay_t {
        replay_t(unsigned number_players = 1, solver_t *solver = nullptr);

        player_ptr operator[](unsigned index);
        void restart(); // TODO: remove
        unsigned size();

        void see_all(unsigned value);

        unsigned current_player = 0;
        unsigned current_choice = 0;
        bool constrained = false;
        unsigned constrained_player = 0;
        bool contradiction = false;
        std::vector<player_t> players;
        std::vector<unsigned> assumed_moves;
        std::vector<float> assumed_moves_weights;

        solver_t *solver;
    };
}
