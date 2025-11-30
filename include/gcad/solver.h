#include <vector>
#include <span>
#include <random>

namespace gcad {
    //! \brief Mean and deviation of an estimated value.
    struct statistics {
        // TODO: maybe use Welford method for more accurate streaming variance
        float mean = 0;
        float deviation = 0;
    };

    struct solution_t {
        float bias, plausibility;
        unsigned move;
    };

    template<class K, class V, class H>
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
            const std::vector<unsigned> &information, unsigned maximum,
            uint64_t mask = ~0
        );
        solution_t choose(
            const std::vector<unsigned> &information, 
            const std::vector<unsigned> &constraints, unsigned maximum,
            uint64_t mask = ~0
        );
        solution_t random(
            const std::vector<unsigned> &constraints, unsigned maximum
        );
        void score(
            const std::vector<unsigned> &information, unsigned move, 
            float value, float weight = 1.0f, bool leaf = false
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
            bool leaf = false;
        };

        struct node {
            std::vector<score_t> move_score;
        };

        cache_t<std::vector<unsigned>, node, hash> information_node;
        // Given a player index and that players observations and moves, 
        // bias-corrected
        cache_t<std::vector<unsigned>, std::vector<float>, hash> 
            importance_node;
        std::minstd_rand generator;
    };
}
