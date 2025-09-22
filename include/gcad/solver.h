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

    template<class K, class V, class H = std::hash<K>>
    struct cache_t : private H {
        cache_t(unsigned log_size = 12);
        template<class T> void get(V&, const T&) const;
        template<class T> void put(const T&, const V&);
        std::vector<K> keys;
        std::vector<V> values;
        std::size_t log_size;
    };

    struct hash {
        size_t operator()(std::span<const unsigned> v) const {
            size_t hash = 1;
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
        solver_t(unsigned log_size = 12);
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

        cache_t<std::uint64_t, node> information_node;
        cache_t<std::uint64_t, std::vector<float>> importance_node;
        std::minstd_rand random;
    };
}
