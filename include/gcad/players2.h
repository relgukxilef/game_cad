#include <string_view>
#include <unordered_map>
#include <optional>

#include "players.h"

namespace gcad {
    using namespace std;

    struct group_closer_t;

    struct sample_closer_t;

    struct player2_ptr {
        bool option(string_view title);
        optional<unsigned> choice(string_view prompt, unsigned maximum);
        void label(string_view text);
        void counter(string_view text, unsigned value);
        unique_ptr<const void, group_closer_t> group(string_view title);
        void score(string_view text, unsigned value);
        bool game_over();
        void print();
        void set_human(bool human);
        void input(unsigned value);
        bool active();
        void grid(unsigned columns);
        unique_ptr<const void, sample_closer_t> sample();

        struct players2_t *players;
        unsigned index;
    };

    struct group_closer_t {
        typedef const void *pointer;
        player2_ptr player;
        unsigned depth;
        void operator()(const void *);
    };

    struct sample_closer_t {
        typedef const void *pointer;
        player2_ptr player;
        void operator()(const void *);
    };

    enum struct item_type {
        leaf, begin, end,
    };

    struct item {
        item_type type;
        string text;
    };

    struct player_info {
        bool game_over = false;
        bool human = false;
        bool active = false;
        vector<item> items;
        vector<unsigned> inputs;
        vector<unsigned> prefix_inputs, prefix_outputs;
        string prompt;
        unsigned columns = 1;
    };

    struct players2_t {
        players2_t(unsigned player_count);
        player2_ptr operator[](unsigned index);
        unsigned random(unsigned maximum);
        void restart();

        players_t players;
        unordered_map<string_view, unsigned> labels;
        vector<player_info> player_infos;
        optional<unsigned> sampling_player;
    };
}
