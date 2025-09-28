#include <gcad/solver.h>

using namespace std;

namespace gcad {
    struct distribution {
        void push_back(float x, float weight = 1.0f) {
            sum += x * weight;
            squares += x * x * weight;
            count += weight;
        }
        void insert(distribution x, float weight = 1.0f) {
            sum += x.sum * weight;
            squares += x.squares * weight;
            count += x.count * weight;
        }
        float mean() {
            return sum / count;
        }
        float variance() {
            return (squares - sum * sum / count) / (count - 1);
        }
        float mean_variance() {
            return (squares - sum * sum / count) / (count - 1) / count;
        }
        float sum = 0, squares = 0, count = 0;
    };

    template<class K, class V, class H>
    cache_t<K, V, H>::cache_t(unsigned log_size) : 
        keys(1ull << log_size), 
        values(1ull << log_size),
        log_size(log_size)
    {}

    template<class K, class V, class H>
    template<class T>
    void cache_t<K, V, H>::get(V& v, const T& t) const {
        std::size_t index = H::operator()(t) >> (64 - log_size);
        if (keys[index] != t) {
            v = V();
        } else {
            v = values[index];
        }
    }

    template<class K, class V, class H>
    template<class T>
    void cache_t<K, V, H>::put(const T& t, const V& v) {
        std::size_t index = H::operator()(t) >> (64 - log_size);
        keys[index] = t;
        values[index] = v;
    }

    solver_t::solver_t(unsigned log_size) : 
        information_node(log_size), importance_node(log_size) 
    {}

    solution_t solver_t::choose(
        const vector<unsigned> &information, unsigned maximum,
        uint64_t mask
    ) {
        return choose(information, {}, maximum, mask);
    }

    solution_t solver_t::choose(
        const vector<unsigned> &information, 
        const vector<unsigned> &constraints, unsigned maximum,
        uint64_t mask
    ) {
        solver_t::node node;
        information_node.get(node, information);

        auto &score = node.move_score;

        score.resize(maximum);

        distribution parent;
        unsigned offset = random() % maximum;
        for (auto i = 0u; i < maximum; i++) {
            auto move = (offset + i) % maximum;
            if (~mask & (1ull << move))
                continue;
            auto move_score = score[move];

            if (move_score.count == 0) {
                // TODO: maybe skip with low probability for games with high
                // branching factor
                return {1.f / maximum, 1.f / maximum, move};
            }

            parent.push_back(move_score.sum / move_score.count);
        }

        float max_mean = 0;
        float max_variance = 0;
        for (auto move = 0u; move < maximum; move++) {
            if (~mask & (1ull << move))
                continue;
            auto move_score = score[move];
            distribution scores = 
                {move_score.sum, move_score.squares, move_score.count};
            scores.insert(parent, 1 / parent.count);
            float mean = scores.mean();
            float variance = scores.variance();
            if (move_score.leaf) {
                variance = scores.mean_variance();
            }
            
            max_variance = max(max_variance, variance);
            max_mean = max(max_mean, mean);
        }

        vector<float> bias;
        importance_node.get(bias, constraints);
        bias.resize(maximum);
        float sum = 0, importance_sum = 0;
        solution_t best_move = {1.f, 1.f, 0};

        for (auto move = 0u; move < maximum; move++) {
            if (~mask & (1ull << move))
                continue;
            auto move_score = node.move_score[move];
            distribution scores = 
                {move_score.sum, move_score.squares, move_score.count};
            scores.insert(parent, 1 / parent.count);
            float mean = scores.mean();
            // This matches Thompson sampling with normal distributions
            // for binary choices, and is smooth otherwise.
            // Transformed for robustness.
            // Using the variance of the current move breaks the weight of the
            // best move.
            float flipped_erfc = erfc(
                (max_mean - mean) / sqrt(max(2 * max_variance, 1e-9f))
            );
            float strength = flipped_erfc / (2 - flipped_erfc);
            float plausibility = bias[move] + 1;
            importance_sum += plausibility;
            float weight = strength * plausibility;
            weight = max(weight, 1e-9f);
            sum += weight;

            if (bernoulli_distribution(weight / sum)(random)) {
                best_move = {weight, plausibility, move};
            }
        }

        best_move.plausibility /= importance_sum;
        best_move.bias /= sum;

        return best_move;
    }

    void solver_t::score(
        const vector<unsigned> &information, unsigned move, float value,
        float weight, bool leaf
    ) {
        solver_t::node node;
        information_node.get(node, information);
        auto &move_score = node.move_score;
        move_score.resize(max<unsigned>(move + 1, move_score.size()));
        auto &score = move_score[move];
        score.sum += value * weight;
        score.squares += value * value * weight;
        score.count += weight;
        score.leaf = leaf;
        information_node.put(information, node);
    }

    statistics solver_t::get_statistics(
        const vector<unsigned> &information, unsigned move
    ) {
        solver_t::node node;
        information_node.get(node, information);

        if (node.move_score.empty()) {
            return {};
        }
        node.move_score.resize(max<size_t>(node.move_score.size(), move + 1));

        auto move_score = node.move_score[move];

        if (move_score.count == 0) {
            return {};
        }

        distribution scores = 
            {move_score.sum, move_score.squares, move_score.count};
        float mean = scores.mean();
        float variance = scores.variance();

        return {mean, sqrt(variance)};
    }

    void solver_t::bias(
        const vector<unsigned> &constraints, unsigned move,
        float weight
    ) {
        vector<float> node;
        importance_node.get(node, constraints);
        node.resize(max<unsigned>(move + 1, node.size()));
        node[move] += weight;
        importance_node.put(constraints, node);
    }
}
