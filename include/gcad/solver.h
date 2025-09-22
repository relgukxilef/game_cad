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
    
    struct solver_t {
        // TODO: take span as argument instead of vector
        solution_t choose(
            std::span<const unsigned> information, unsigned maximum,
            uint64_t mask = ~0
        );
        solution_t choose(
            std::span<const unsigned> information, 
            std::span<const unsigned> constraints, unsigned maximum,
            uint64_t mask = ~0
        );
        void score(
            std::span<const unsigned> information, unsigned move, 
            float value, float weight = 1.0f, bool leaf = false
        );
        statistics get_statistics(
            std::span<const unsigned> information, unsigned move
        );

        void bias(
            std::span<const unsigned> constraints, unsigned move, 
            float weight
        );

        struct score_t {
            float sum = 0, squares = 0, count = 0;
            bool leaf = false;
        };

        struct node {
            std::vector<score_t> move_score;
        };

        std::unordered_map<std::uint64_t, node> information_node;
        std::unordered_map<std::uint64_t, std::vector<float>> importance_node;
        std::minstd_rand random;
    };
}
