#include <gcad/players.h>

#include <limits>

namespace gcad {

    unsigned player_ptr::choose(unsigned maximum)
    {
        auto &output = players->output[index];
        
        auto move = players->solver.choose(output, maximum);

        players->moves[index].push_back({output, move});
        return move;
    }

    void player_ptr::see(unsigned value) {
        players->output[index].push_back(value);
    }

    void player_ptr::score(unsigned value) {
        for (auto edge : players->moves[index]) {
            players->solver.score(edge.output, edge.input, value);
        }
    }

    players_t::players_t(unsigned number_players) {
        output.resize(number_players);
        moves.resize(number_players);
    }
    
    player_ptr players_t::operator[](unsigned index) {
        return {this, index};
    }

    void players_t::restart() {
        for (auto &player : moves) {
            player.clear();
        }
        for (auto &player : output) {
            player.clear();
        }
    }
}
