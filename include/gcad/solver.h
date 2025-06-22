#include <vector>
#include <span>
#include <unordered_map>
#include <random>

namespace gcad {
    using namespace std;

    struct statistics {
        float mean, deviation;
    };

    struct hash {
        size_t operator()(span<const unsigned> v) const {
            size_t hash = 0;
            for (auto e : v) {
                hash = hash * 238857403 + e;
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
        std::minstd_rand random;
    };
}
