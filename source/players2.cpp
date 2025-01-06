#include <gcad/players2.h>

namespace gcad {
    players2_t::players2_t(unsigned player_count) : players(player_count) {
        game_over.resize(player_count);
    }

    player2_ptr players2_t::operator[](unsigned index){
        return {this, index};
    }

    void players2_t::restart() {
        for (auto player : game_over) {
            player = false;
        }
        players.restart();
    }

    bool player2_ptr::option(string_view title)
    {
        // TODO
        return false;
    }

    optional<unsigned> player2_ptr::choice(
        string_view prompt, unsigned maximum
    ) {
        // TODO
        return optional<unsigned>(players->players[index].choose(maximum));
    }

    void player2_ptr::label(string_view text) {
        auto &labels = players->labels;
        auto value = 
            labels.insert({text, (unsigned)labels.size()}).first->second;
        players->players[index].see(value);
    }

    void player2_ptr::counter(string_view text, unsigned value) {
        // TODO
    }

    void player2_ptr::score(string_view text, unsigned value) {
        players->players[index].score(value);
        players->game_over[index] = true;
    }

    bool player2_ptr::game_over() {
        return players->game_over[index];
    }
    
    void group_closer_t::operator()() {
        // TODO
    }
}
