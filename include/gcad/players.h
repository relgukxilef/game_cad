#include <vector>
#include <unordered_map>
#include <random>
#include <functional>

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

    struct player_ptr {
        struct players_t *players;
        unsigned index;

        unsigned choose(unsigned maximum);
        void see(unsigned value);
        void score(unsigned value);
    };

    struct players_t {
        players_t(unsigned number_players = 1);

        player_ptr operator[](unsigned index);
        void restart(); 
        // TODO: maybe store state during a game in separate class

        // output, input, score -> count
        // output, input -> scores
        // output -> inputs
        struct score {
            unordered_map<unsigned, unsigned> score_count;
        };
        struct node {
            unordered_map<unsigned, score> move_score;
        };
        unordered_map<vector<unsigned>, node, hash> output_node;

        struct edge {
            vector<unsigned> output;
            unsigned input;
        };
        
        score root;

        // players moves and outputs for the current game
        vector<vector<edge>> moves;
        vector<vector<unsigned>> output;

        std::random_device random;
    };

}
