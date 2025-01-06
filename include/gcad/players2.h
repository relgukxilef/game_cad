#include <string_view>
#include <unordered_map>
#include <optional>

#include "players.h"

namespace gcad {
    using namespace std;

    struct group_closer_t {
        typedef unsigned pointer;
        void operator()();
    };

    struct player2_ptr {
        bool option(string_view title);
        optional<unsigned> choice(string_view prompt, unsigned maximum);
        void label(string_view text);
        void counter(string_view text, unsigned value);
        unique_ptr<void, group_closer_t> group(string_view title);
        void score(string_view text, unsigned value);
        bool game_over();

        struct players2_t *players;
        unsigned index;
    };

    struct players2_t {
        players2_t(unsigned player_count);
        player2_ptr operator[](unsigned index);
        void restart();

        players_t players;
        unordered_map<string_view, unsigned> labels;
        vector<bool> game_over;
    };
}
