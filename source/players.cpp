#include <gcad/players.h>

#include <limits>

namespace gcad {

    optional<unsigned> player_ptr::choose(unsigned maximum) {
        auto &player = players->players[index];

        optional<unsigned> move;
        if (player.current_move < player.moves.size()) {
            move = player.moves[player.current_move].move;
        } else if (players->solver) {
            // TODO: avoid copy
            vector<unsigned> observations{
                player.observations.begin(), 
                player.observations.begin() + 
                player.current_observation
            };
            move = players->solver->choose(observations, maximum);
            input(*move);
        }

        if (move) {
            player.current_move++;
        }

        players->current_player = index;
        players->current_choice = maximum;

        return move;
    }

    void player_ptr::see(unsigned value) {
        auto &player = players->players[index];

        if (player.current_observation < player.observations.size()) {
            if (value != player.observations[player.current_observation]) {
                players->contradiction = true;
                return;
            }
        } else {
            player.observations.push_back(value);
        }
        player.current_observation++;
    }

    void player_ptr::score(unsigned value) {
        if (players->contradiction)
            return;
        if (!players->solver)
            return;
        auto &player = players->players[index];
        for (auto move : player.moves) {
            // TODO: avoid copy
            vector<unsigned> observations{
                player.observations.begin(), 
                player.observations.begin() + move.observations
            };
            players->solver->score(observations, move.move, value);
        }
    }

    optional<unsigned> player_ptr::choose_visible(unsigned maximum) {
        optional<unsigned> move;
        
        // since open choices are observed, we can just read the move from
        // the observations
        for (auto i = 0u; i < players->size(); i++) {
            auto &player = players->players[i];
            if (player.current_observation < player.observations.size()) {
                move = player.observations[player.current_observation];
            }
        }

        if (!move) {
            move = choose(maximum);
        }

        if (move) {
            for (auto i = 0u; i < players->size(); i++) {
                (*players)[i].see(*move);
            }
        }
        return move;
    }

    players_t player_ptr::sample(solver_t *solver) {
        // removes all moves from other players from the replay
        // TODO: avoid copying, have caller use assignment operator, 
        // and use vector::assign in operator
        players_t copy = *players;
        copy.solver = solver;
        for (auto i = 0u; i < copy.players.size(); i++) {
            auto &player = copy.players[i];
            if (i != index)
                copy[i].resize(0);
            player.current_move = 0;
            player.current_observation = 0;
            player.replay_end = copy.players[i].moves.size();
        }
        return copy;
    }

    players_t::players_t(unsigned number_players, solver_t *solver) {
        this->solver = solver;
        players.resize(number_players);
    }
    
    void player_ptr::resize(unsigned size) {
        auto &player = players->players[index];
        player.moves.resize(size);
        if (size == 0)
            player.observations.clear();
        else
            player.observations.resize(player.moves.back().observations);
        // TODO: better way to do this?
        player.current_move = 0;
        player.current_observation = 0;
    }

    void player_ptr::input(unsigned move) {
        auto &player = players->players[index];
        player.moves.push_back({move, player.current_observation});
    }

    float player_ptr::get_expected_score(unsigned choice) {
        return 0;
    }

    player_ptr players_t::operator[](unsigned index) {
        return {this, index};
    }

    void players_t::restart() {
        for (auto i = 0; i < players.size(); i++) {
            operator[](i).resize(players[i].replay_end);
        }
    }

    unsigned players_t::size() {
        return players.size();
    }
}
