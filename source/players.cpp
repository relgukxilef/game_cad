#include <gcad/players.h>

#include <limits>

using namespace std;

namespace gcad {
    vector<unsigned> encode_bias(
        vector<unsigned> &observations, 
        vector<unsigned> &assumptions,
        size_t prefix_length
    ) {
        vector<unsigned> result;
        result.push_back(observations.size());
        result.insert(result.end(), observations.begin(), observations.end());
        result.insert(
            result.end(), assumptions.begin(), 
            assumptions.begin() + prefix_length
        );
        return result;
    }

    vector<unsigned> encode_bias(
        vector<unsigned> &observations, 
        vector<unsigned> &assumptions
    ) {
        return encode_bias(observations, assumptions, assumptions.size());
    }

    optional<unsigned> player_ptr::choose(unsigned maximum, uint64_t mask) {
        auto &player = players->players[index];

        optional<unsigned> move;
        float bias_weight = 0;

        if (player.current_move < player.moves.size()) {
            move = player.moves[player.current_move].move;
            // Replay can't contain packed moves as the mask is not known at the
            // call to input

        } else if (players->solver) {
            // TODO: avoid copy
            vector<unsigned> observations{
                player.observations.begin(), 
                player.observations.begin() + 
                player.current_observation
            };

            float weight;
            solution_t solution;

            if (players->constrained) {
                // this has to be done jointly with Thompson sampling 
                auto constrained_player = players->constrained_player;
                solution = players->solver->choose(
                    observations, 
                    encode_bias(
                        players->players[constrained_player].observations,
                        players->assumed_moves
                    ), 
                    maximum, mask
                );

            } else {
                solution = players->solver->choose(observations, maximum, mask);
            }

            move = solution.move;
            weight = 1.f / solution.plausibility;
            bias_weight = 1.f / solution.bias;

            // TODO: let player see choice?
            player.moves.push_back({*move, player.current_observation, weight});
        }

        if (move) {
            players->assumed_moves.push_back(*move);
            players->assumed_moves_weights.push_back(bias_weight);
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
            }
            if (player.current_observation == player.observations.size() - 1) {
                // no more constraints
                players->constrained = false;
            }
        } else {
            player.observations.push_back(value);
        }
        player.current_observation++;
    
        if (players->contradiction)
            return; // TODO: need to split up expected and actual observations
        for (auto i = 0; i < players->assumed_moves.size(); i++) {
            players->solver->bias(
                encode_bias(
                    player.observations, 
                    players->assumed_moves,
                    i
                ),
                players->assumed_moves[i],
                players->assumed_moves_weights[i]
            );
        }
    }

    void player_ptr::score(float value) {
        if (!players->solver)
            return;
        auto &player = players->players[index];
        // score leaf
        vector<unsigned> observations{
            player.observations.begin(), 
            player.observations.begin() + player.moves.back().observations
        };
        players->solver->score(
            observations, player.moves.back().move, 
            value + 1, player.moves.back().weight, true
        );
        value = players->solver->get_statistics(
            observations, player.moves.back().move
        ).mean;
        for (auto i = 0; i < player.moves.size() - 1; i++) {
            auto move = player.moves[i];
            // TODO: avoid copy
            observations = {
                player.observations.begin(), 
                player.observations.begin() + move.observations
            };
            players->solver->score(
                observations, move.move, value, move.weight
            );
        }
    }

    players_t player_ptr::sample(solver_t *solver) {
        // removes all moves from other players from the replay
        // TODO: avoid copying, have caller use assignment operator, 
        // and use vector::assign in operator
        players_t copy = *players;
        copy.solver = solver;
        copy.assumed_moves.clear();
        copy.assumed_moves_weights.clear();
        for (auto i = 0u; i < copy.players.size(); i++) {
            auto &player = copy.players[i];
            if (i != index)
                copy[i].resize(0);
            player.current_move = 0;
            player.current_observation = 0;
            player.replay_end = (unsigned)copy.players[i].moves.size();
        }
        copy.constrained = true;
        copy.constrained_player = index;
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
        player.moves.push_back({move, player.current_observation, 0});
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
        assumed_moves.clear();
        assumed_moves_weights.clear();
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
