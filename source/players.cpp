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
        unsigned best_score = 0;
        for (auto move = 0u; move < maximum; move++) {
            auto move_score = node->second.move_score.find(move);

            unsigned sum = 0;
            unsigned score = 0;
            if (move_score != node->second.move_score.end()) {
                for (
                    auto [current_score, count] : move_score->second.score_count
                ) {
                    sum += count;
                    if (
                        bernoulli_distribution((float)count / sum)(
                            players->random
                        )
                    )
                        score = current_score;
                }
            }

            // exploration bias
            if (bernoulli_distribution(1.0f / (sum + 1))(
                players->random
            )) {
                sum = 0;
                for (auto [current_score, count] : players->root.score_count) {
                    sum += count;
                    if (
                        bernoulli_distribution((float)count / sum)(
                            players->random
                        )
                    )
                        score = current_score;
                }
            }

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
