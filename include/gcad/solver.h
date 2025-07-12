#include <vector>
#include <span>
#include <unordered_map>
#include <random>

namespace gcad {
    using namespace std;

    struct statistics {
        // TODO: maybe use Welford method for more accurate streaming variance
        float mean = 0, deviation = 0;
    };

    struct importance {
        // bias-corrected
        float failures = 0, successes = 0;
    };

    struct hash {
        size_t operator()(span<const unsigned> v) const {
            size_t hash = 0;
            for (auto e : v) {
                hash = (hash + e) * 13552659750178043939ull;
            }
            return hash;
        }
        size_t operator()(const vector<unsigned>& v) const {
            return operator()(span<const unsigned>(v.data(), v.size()));
        }
    };

    struct solver_t {
        // TODO: take span as argument instead of vector
        unsigned choose(const vector<unsigned> &information, unsigned maximum);
        unsigned choose(
            const vector<unsigned> &information, 
            const vector<unsigned> &constraints, unsigned maximum
        );
        void score(
            const vector<unsigned> &information, unsigned move, unsigned value
        );
        statistics get_statistics(
            const vector<unsigned> &information, unsigned move
        );

        struct score_t {
            float sum = 0, squares = 0;
            unsigned count = 0;
        };

        struct node {
            vector<score_t> move_score;
        };

        unordered_map<vector<unsigned>, node, hash> information_node;
        // Given a player index and that players observations and moves, 
        unordered_map<vector<unsigned>, vector<importance>, hash> 
            importance_node;
        std::minstd_rand random;
    };
}
