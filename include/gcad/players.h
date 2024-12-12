#include <vector>
#include <map>
#include <random>
#include <functional>

namespace gcad {

    struct player_ptr {
        struct players_t *players;
        unsigned index;

        unsigned choose(unsigned maximum);
        void see(unsigned value);
        void score(unsigned value);
    };

    struct players_t {
        struct edge_t {
            // TODO: prefix of output could be compressed to auto-increment int
            std::vector<unsigned> output;
            unsigned input; // one node per choice
            bool operator<(const edge_t &other) const {
                return 
                    std::tie(output, input) < 
                    std::tie(other.output, other.input);
            }
            bool operator<(const std::vector<unsigned> &output) const {
                return this->output < output;
            }
        };
        struct node_t {
            unsigned count = 0, sum = 0, squares = 0;
        };
        // TODO: nodes could be an unordered_set
        std::map<edge_t, node_t, std::less<>> nodes; // shared between players
        std::vector<std::vector<edge_t>> moves{{}}; // one per player
        std::vector<std::vector<unsigned>> output{{}};
        std::vector<bool> expanding{false,};

        node_t root;

        std::random_device random;

        player_ptr operator[](unsigned index);
        void restart(); 
        // TODO: maybe store state during a game in separate class
    };

}
