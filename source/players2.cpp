#include <gcad/players2.h>
#include <iostream>
#include <cassert>

namespace gcad {
    // TODO: order functions to match the header
    players2_t::players2_t(unsigned player_count) : players(player_count + 1) {
        player_infos.resize(player_count + 1);
    }

    player2_ptr players2_t::operator[](unsigned index) {
        return {this, index + 1};
    }

    unsigned players2_t::random(unsigned maximum) {
        return *player2_ptr{this, 0}.choice("", maximum);
    }

    void players2_t::restart() {
        for (auto &player : player_infos) {
            player.game_over = false;
            player.items.clear();
        }
        players.restart();
    }

    bool player2_ptr::option(string_view title) {
        // TODO
        return false;
    }

    optional<unsigned> player2_ptr::choice(
        string_view prompt, unsigned maximum
    ) {
        // TODO: make actions visible to the random player
        auto &player = players->player_infos[index];

        if (!player.human || players->sampling) {
            return players->players[index].choose(maximum);
        }

        if (auto move = players->players[index].replay(maximum)) {
            return *move;
        }

        player.prompt.assign(prompt);
        player.active = true;
        return {};
    }

    void player2_ptr::label(string_view text) {
        auto &labels = players->labels;
        auto value = 
            labels.insert({text, (unsigned)labels.size()}).first->second;
        players->players[index].see(value);
        auto &player = players->player_infos[index];
        
        player.items.push_back({
            item_type::leaf, string(text)}
        );
    }

    void player2_ptr::counter(string_view text, unsigned value) {
        // TODO
    }

    void player2_ptr::score(string_view text, unsigned value) {
        auto &player = players->player_infos[index];
        player.items.push_back({item_type::leaf, string(text)});
        player.game_over = true;
        players->players[index].score(value);
    }

    bool player2_ptr::game_over() {
        return players->player_infos[index].game_over;
    }

    void player2_ptr::print() {
        auto &player = players->player_infos[index];
        unsigned max_width = 0;
        for (auto &item : player.items) {
            max_width = std::max(max_width, (unsigned)item.text.size());
        }
        cout.setf(ios::left, ios::adjustfield);
        unsigned index = 0;
        for (auto &item : player.items) {
            cout.width(max_width);
            cout << item.text;
            index++;
            if (index == player.columns) {
                cout << endl;
                index = 0;
            }
        }
        if (player.active)
            cout << player.prompt << " ";
        player.items.clear();
    }

    void player2_ptr::set_human(bool human) {
        players->player_infos[index].human = human;
    }

    void player2_ptr::input(unsigned value) {
        auto &player = players->player_infos[index];
        player.active = false;
        players->players.filter.players[index].moves.push_back({
            players->players.current_game.players[index].output, value
        });
    }

    bool player2_ptr::active() {
        return players->player_infos[index].active;
    }

    void player2_ptr::grid(unsigned columns) {
        auto &player = players->player_infos[index];
        player.columns = columns;
    }

    const int dummy = 0;

    unique_ptr<const void, sample_closer_t> player2_ptr::sample() {
        players->players.filter.players[index] = 
            players->players.current_game.players[index];
        auto original = players->players.current_game;
        players->restart();
        players->sampling = true;
        return {&dummy, {*this, original}};
    }

    void group_closer_t::operator()(const void *) {
        // TODO
    }

    void sample_closer_t::operator()(const void *) {
        player.players->players.current_game = std::move(original);
        player.players->players.filter.players[player.index] = {};
        player.players->sampling = false;
    }
}
