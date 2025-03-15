#include <gcad/players.h>

#include <limits>

namespace gcad {

    optional<unsigned> player_ptr::choose(unsigned maximum) {
        auto &moves = players->current.players[index].moves;
        auto &output = players->current.players[index].output;
        auto &filter_moves = players->filter.players[index].moves;

        optional<unsigned> move;
        if (moves.size() < filter_moves.size()) {
            move = filter_moves[moves.size()].input;
        } else if (players->solver) {
            move = players->solver->choose(output, maximum);
        }

        if (move) {
            moves.push_back({output, *move});
        }

        players->current_player = index;
        players->current_choice = maximum;

        return move;
    }

    void player_ptr::see(unsigned value) {
        auto &current_output = players->current.players[index].output;
        auto &filter_output = players->filter.players[index].output;

        if (current_output.size() < filter_output.size() && 
            value != filter_output[current_output.size()]) {
            players->contradiction = true;
            return;
        }
        current_output.push_back(value);
    }

    void player_ptr::score(unsigned value) {
        if (!players->solver)
            return;
        for (auto edge : players->current.players[index].moves) {
            players->solver->score(edge.output, edge.input, value);
        }
    }

    players_t player_ptr::sample(solver_t *solver) {
        players_t p(players->filter.players.size(), solver);
        p.filter.players[index] = players->current.players[index];
        return p;
    }

    players_t::players_t(unsigned number_players, solver_t *solver) {
        this->solver = solver;
        current.players.resize(number_players);
        filter.players.resize(number_players);
    }
    
    player_ptr players_t::operator[](unsigned index) {
        return {this, index};
    }

    void players_t::restart() {
        for (auto &player : current.players) {
            player.moves.clear();
            player.output.clear();
        }
    }
}
