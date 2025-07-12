#include <gcad/solver.h>

namespace gcad {
    unsigned solver_t::choose(
        const vector<unsigned> &information, unsigned maximum
    ) {
        auto &node = information_node[information];

        auto &score = node.move_score;

        score.resize(maximum);
        
        float parent_mean = 0;
        float parent_squares = 0;
        unsigned offset = random() % maximum;
        for (auto i = 0u; i < maximum; i++) {
            auto move = (offset + i) % maximum;
            auto move_score = score[move];

            if (move_score.count == 0) {
                // TODO: maybe skip with low probability for games with high
                // branching factor
                return move;
            }

            parent_mean += move_score.sum / move_score.count;
            parent_squares += move_score.squares / move_score.count;
        }

        parent_squares /= maximum;
        parent_mean /= maximum;

        float max_mean = 0;
        float mean_variance = 0;
        for (auto move = 0u; move < maximum; move++) {
            auto move_score = score[move];
            float mean = 
                (move_score.sum + parent_mean) / (move_score.count + 1);
            float variance = (
                (move_score.squares + parent_squares) / (move_score.count + 1) - 
                mean * mean
            ) / move_score.count;
            
            mean_variance += variance;

            if (mean > max_mean) {
                max_mean = mean;
            }
        }

        mean_variance /= maximum;

        float sum = 0;
        unsigned best_move = 0;

        for (auto move = 0u; move < maximum; move++) {
            auto move_score = node.move_score[move];
            float mean = 
                (move_score.sum + parent_mean) / (move_score.count + 1);
            // Approximate Thompson sampling with normal distributions
            float weight = erf(
                (mean - max_mean) / sqrt(max(2 * mean_variance, 1e-9f)) * 2
            ) / 2 + 0.5;
            weight = max(weight, 1e-9f);
            sum += weight;

            if (bernoulli_distribution(weight / sum)(random)) {
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
