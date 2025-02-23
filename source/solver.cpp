#include <gcad/solver.h>

namespace gcad {
    unsigned solver_t::choose(
        const vector<unsigned> &information, unsigned maximum
    ) {
        auto &node = information_node[information];

        if (node.move_score.empty()) {
            node.move_score.resize(maximum);
            return random() % maximum;
        }

        float sum = 0;
        float squares = 0;
        unsigned offset = random() % maximum;
        for (auto i = 0u; i < maximum; i++) {
            auto move = (offset + i) % maximum;
            auto move_score = node.move_score[move];

            if (move_score.count == 0) {
                // explore new nodes first
                // TODO: is this smart for games with high branching factors?
                return move;
            }

            sum += move_score.sum / move_score.count;
            squares += move_score.squares / move_score.count;
        }

        squares /= maximum;
        sum /= maximum;

        // Thompson sampling
        unsigned best_move = offset;
        float best_score = 0;
        for (auto i = 0u; i < maximum; i++) {
            auto move = (offset + i) % maximum;
            auto move_score = node.move_score[move];
            float mean = (move_score.sum + sum) / (move_score.count + 1);
            float deviation = sqrt(
                (
                    (move_score.squares + squares) / (move_score.count + 1) - 
                    mean * mean
                ) / move_score.count
            );

            float score = 
                normal_distribution<float>()(random) * 
                deviation +
                mean;

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
        }

        return best_move;
    }

    void solver_t::score(
        const vector<unsigned> &information, unsigned move, unsigned value
    ) {
        auto &move_score = information_node.at(information).move_score[move];
        move_score.sum += value;
        move_score.squares += value * value;
        move_score.count++;
    }
}
