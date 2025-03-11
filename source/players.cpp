#include <gcad/players.h>

#include <limits>

namespace gcad {

    optional<unsigned> player_ptr::replay(unsigned maximum) {
        auto &moves = players->current_game.players[index].moves;
        auto &output = players->current_game.players[index].output;
        auto &filter_moves = players->filter.players[index].moves;

        if (moves.size() < filter_moves.size()) {
            auto move = filter_moves[moves.size()].input;
            moves.push_back({output, move});
            return move;
        }

        return nullopt;
    }

    unsigned player_ptr::choose(unsigned maximum) {
        if (auto move = replay(maximum)) {
            return *move;
        }
        
        auto &moves = players->current_game.players[index].moves;
        auto &output = players->current_game.players[index].output;

        auto move = players->solver.choose(output, maximum);
        moves.push_back({output, move});
        return move;
    }

    void player_ptr::see(unsigned value) {
        auto &current_output = players->current_game.players[index].output;
        auto &filter_output = players->filter.players[index].output;

        if (current_output.size() < filter_output.size() && 
            value != filter_output[current_output.size()]) {
            players->contradiction = true;
            return;
        }
        current_output.push_back(value);
    }

    void player_ptr::score(unsigned value) {
        for (auto edge : players->current_game.players[index].moves) {
            players->solver.score(edge.output, edge.input, value);
        }
    }

    players_t::players_t(unsigned number_players) {
        current_game.players.resize(number_players);
        filter.players.resize(number_players);
    }
    
    player_ptr players_t::operator[](unsigned index) {
        return {this, index};
    }

    void players_t::restart() {
        for (auto &player : current_game.players) {
            player.moves.clear();
            player.output.clear();
        }
    }
}
