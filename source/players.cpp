#include <gcad/players.h>

#include <cstdlib>
#include <limits>

namespace gcad {

    unsigned player_ptr::choose(unsigned maximum)
    {
        auto &output = players->output[index];
        auto node = players->nodes.find(output);
        
        if (node == players->nodes.end()) {
            unsigned move = rand() % maximum;
            players->moves[index].push_back({output, move});
            return move;
        }

        // Thompson sampling
        unsigned best_move = 0;
        float best_score = std::numeric_limits<float>::lowest();
        for (auto i = 0u; i < maximum; i++) {
            auto &node = players->nodes[{output, i}];
            float sum = node.sum, count = node.count, squares = node.squares;

            // bias result towards root average to encourage exploration
            sum += (float)players->root.sum / players->root.count;
            squares += (float)players->root.squares / players->root.count;
            count++;

            float mean = sum / count;
            float deviation = std::sqrt(squares / count - mean * mean);

            float score = 
                std::normal_distribution<float>()(players->random) * 
                deviation +
                mean;

            if (score > best_score) {
                best_score = score;
                best_move = i;
            }
        }

        players->moves[index].push_back({output, best_move});

        return best_move;
    }

    void player_ptr::see(unsigned value) {
        players->output[index].push_back(value);
    }

    void player_ptr::score(unsigned value) {
        unsigned square = value * value;

        players->root.count++;
        players->root.sum += value;
        players->root.squares += square;

        for (auto edge : players->moves[index]) {
            auto node = players->nodes.find(edge);

            if (node != players->nodes.end()) {
                node->second.count++;
                node->second.sum += value;
                node->second.squares += square;

            } else {
                players->nodes[edge] = {1, value, square};
            }
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

    bool operator<(
        const std::vector<unsigned> &output, const players_t::edge_t &edge
    ) {
        return output < edge.output;
    }

}
