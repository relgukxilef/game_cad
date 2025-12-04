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

    //! \brief Stores a game tree with statistics at each node, passed to 
    //! \ref replay_t::replay_t.
    //! \details Use one solver per game, shared across multiple \ref replay_t.
    struct solver_t {
        //! \brief Construct a new game tree.
        //! \details Space for a fixed number of nodes is allocated in the
        //! constructor. Nodes may be replaced with new nodes.
        //! \param log_size The number of bits used to address nodes. Space for
        //! <tt>1 << log_size</tt> nodes is allocated. If this value is too 
        //! small for a given game, it may not converge to the optimal solution.
        //! Larger values will use more memory.
        //! It is recommended that you set it as large as you can afford to.
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
