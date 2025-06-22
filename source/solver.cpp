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
            auto &score = node.move_score;
            score.resize(maximum);
            auto move_score = score[move];

            if (move_score.count == 0) {
                // TODO: maybe skip with low probability for games with high
                // branching factor
                return move;
            }

            parent_count += move_score.count;
            parent_mean += move_score.sum;
            squares += move_score.squares;
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
        auto &move_score = information_node.at(information).move_score;
        move_score.resize(max<unsigned>(move + 1, move_score.size()));
        auto &score = move_score[move];
        score.sum += value;
        score.squares += value * value;
        score.count++;
    }

    statistics solver_t::get_statistics(
        const vector<unsigned> &information, unsigned move
    ) {
        auto &node = information_node[information];

        if (node.move_score.empty()) {
            return {};
        }

        auto move_score = node.move_score[move];

        if (move_score.count == 0) {
            return {};
        }

        float mean = move_score.sum / move_score.count;
        float deviation = sqrt(
            (
                move_score.squares / (move_score.count + 1) - 
                mean * mean
            ) / move_score.count
        );

        return {mean, deviation};
    }
}
