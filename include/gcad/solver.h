#include <vector>
#include <unordered_map>
#include <random>

namespace gcad {
    using namespace std;

    struct hash {
        size_t operator()(const vector<unsigned>& v) const {
            size_t hash = 0;
            for (auto e : v) {
                hash = hash * 238857403 + e;
            }
            return hash;
        }
    };

    struct solver_t {
        // TODO: take span as argument instead of vector
        unsigned choose(const vector<unsigned> &information, unsigned maximum);
        void score(
            const vector<unsigned> &information, unsigned move, unsigned value
        );

        struct score_t {
            float sum = 0, squares = 0;
            unsigned count = 0;
        };

        struct node {
            vector<score_t> move_score;
        };

        unordered_map<vector<unsigned>, node, hash> information_node;
        std::minstd_rand random;
    };
}
