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
            if (
                !player.moves.empty() && 
                player.moves.back().observations == player.current_observation
            ) {
                // no new observations since last move, discourage
                players->solver->score(
                    observations, player.moves.back().move, 0
                );
            }
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
        unsigned previous_node_observations = ~0;
        for (auto i = player.moves.size(); i > 0; i--) {
            auto move = player.moves[i - 1];
            // TODO: avoid copy
            vector<unsigned> observations{
                player.observations.begin(), 
                player.observations.begin() + move.observations
            };
            if (move.observations == previous_node_observations) {
                // skip moves that didn't advance the game
                continue;
            }
            players->solver->score(observations, move.move, value + 1);
            previous_node_observations = move.observations;
        }
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
        player.current_move = player.moves.size();
        player.current_observation = player.observations.size();
    }

    void player_ptr::input(unsigned move) {
        auto &player = players->players[index];
        player.moves.push_back({move, player.current_observation});
    }

    statistics player_ptr::get_expected_score(unsigned choice) {
        auto &player = players->players[index];

        if (!players->solver) {
            return {};
        }

        // TODO: avoid copy
        vector<unsigned> observations{
            player.observations.begin(), 
            player.observations.begin() + 
            player.current_observation
        };
        auto statistics = players->solver->get_statistics(observations, choice);
        statistics.mean -= 1;
        return statistics;
    }

    player_ptr players_t::operator[](unsigned index) {
        return {this, index};
    }

    void players_t::restart() {
        for (auto i = 0; i < players.size(); i++) {
            auto &player = players[i];
            operator[](i).resize(player.replay_end);
            player.current_move = 0;
            player.current_observation = 0;
        }
    }

    unsigned players_t::size() {
        return players.size();
    }

    void players_t::see_all(unsigned move) {
        for (auto i = 0u; i < size(); i++) {
            (*this)[i].see(move);
        }
    }
}
