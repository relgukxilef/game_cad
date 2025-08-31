#include <vector>
#include <span>
#include <unordered_map>
#include <random>

namespace gcad {
    struct statistics {
        // TODO: maybe use Welford method for more accurate streaming variance
        float mean = 0, deviation = 0;
    };

    struct solution_t {
        float bias, plausibility;
        unsigned move;
    };
    

    struct hash {
        size_t operator()(std::span<const unsigned> v) const {
            size_t hash = 0;
            for (auto e : v) {
                hash = (hash + e) * 13552659750178043939ull;
            }
            return hash;
        }
        size_t operator()(const std::vector<unsigned>& v) const {
            return operator()(std::span<const unsigned>(v.data(), v.size()));
        }
    };

    struct solver_t {
        // TODO: take span as argument instead of vector
        solution_t choose(
            const std::vector<unsigned> &information, unsigned maximum,
            uint64_t mask = ~0
        );
        solution_t choose(
            const std::vector<unsigned> &information, 
            const std::vector<unsigned> &constraints, unsigned maximum,
            uint64_t mask = ~0
        );
        void score(
            const std::vector<unsigned> &information, unsigned move, 
            unsigned value, float weight = 1.0f
        );
        statistics get_statistics(
            const std::vector<unsigned> &information, unsigned move
        );

        void bias(
            const std::vector<unsigned> &constraints, unsigned move, 
            float weight
        );

        struct score_t {
            float sum = 0, squares = 0, count = 0;
        };

        struct node {
            std::vector<score_t> move_score;
        };

        std::unordered_map<std::vector<unsigned>, node, hash> information_node;
        // Given a player index and that players observations and moves, 
        // bias-corrected
        std::unordered_map<std::vector<unsigned>, std::vector<float>, hash> 
            importance_node;
        std::minstd_rand random;
    };
}
