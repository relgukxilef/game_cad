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

        float parent_count = 0;
        float parent_mean = 0;
        float squares = 0;
        unsigned offset = random() % maximum;
        for (auto i = 0u; i < maximum; i++) {
            auto move = (offset + i) % maximum;
            auto move_score = node.move_score[move];

            if (move_score.count == 0) {
                continue;
            }

            parent_count += move_score.count;
            parent_mean += move_score.sum;
            squares += move_score.squares;
        }

        // I don't know why this works well.
        if (bernoulli_distribution(1.f / (sqrt(parent_count) + 1))(random)) {
            return offset;
        }

        squares /= parent_count;
        parent_mean /= parent_count;

        // Thompson sampling
        unsigned best_move = offset;
        float best_score = 0;
        for (auto i = 0u; i < maximum; i++) {
            auto move = (offset + i) % maximum;
            auto move_score = node.move_score[move];

            if (move_score.count == 0) {
                continue;
            }

            float mean = 
                (move_score.sum + parent_mean) / (move_score.count + 1);
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
