#include <gcad/players.h>

#include <cstdlib>
#include <limits>

namespace gcad {

    unsigned player_ptr::choose(unsigned maximum)
    {
        auto &output = players->output[index];
        auto node = players->output_node.find(output);
        
        if (node == players->output_node.end()) {
            unsigned move = rand() % maximum;
            players->moves[index].push_back({output, move});
            return move;
        }

        // Thompson sampling
        unsigned best_move = 0;
        float best_score = 0;
        unsigned offset = rand() % maximum;
        for (auto i = 0u; i < maximum; i++) {
            auto move = (offset + i) % maximum;
            auto move_score = node->second.move_score.find(move);
            
            if (move_score == node->second.move_score.end()) {
                // explore new nodes first
                best_move = move;
                break;
            }

            float score_sum = 0;
            float weight_sum = 0;

            for (
                auto [current_score, count] : players->root.score_count
            ) {
                float weight = gamma_distribution<float>(count)(players->random);
                weight_sum += weight;
                score_sum += current_score * weight;
            }

            score_sum /= weight_sum;
            weight_sum = 1;

            for (
                auto [current_score, count] : move_score->second.score_count
            ) {
                float weight = gamma_distribution<float>(count)(
                    players->random
                );
                weight_sum += weight;
                score_sum += current_score * weight;
            }

            float score = score_sum / weight_sum;

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
        }

        players->moves[index].push_back({output, best_move});

        return best_move;
    }

    void player_ptr::see(unsigned value) {
        players->output[index].push_back(value);
    }

    void player_ptr::score(unsigned value) {
        players->root.score_count.insert({value, 0}).first->second++;

        for (auto edge : players->moves[index]) {
            auto &node = 
                players->output_node.insert({edge.output, {}}).first->second;
            auto &score = 
                node.move_score.insert({edge.input, {}}).first->second;
            score.score_count.insert({value, 0}).first->second++;
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
