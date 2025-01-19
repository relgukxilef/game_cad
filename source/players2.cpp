#include <gcad/players2.h>
#include <iostream>

namespace gcad {
    players2_t::players2_t(unsigned player_count) : players(player_count + 1) {
        player_infos.resize(player_count + 1);
    }

    player2_ptr players2_t::operator[](unsigned index) {
        return {this, index + 1};
    }

    unsigned players2_t::random(unsigned maximum) {
        return players[0].choose(maximum);
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
        player.prompt = string(prompt);
        if (!player.human)
            return optional<unsigned>(players->players[index].choose(maximum));

        if (player.inputs.empty())
            return {};

        auto input = player.inputs.back();
        player.inputs.pop_back();
        return input;
    }

    void player2_ptr::label(string_view text) {
        auto &labels = players->labels;
        auto value = 
            labels.insert({text, (unsigned)labels.size()}).first->second;
        players->players[index].see(value);
        players->player_infos[index].items.push_back({
            item_type::leaf, string(text)}
        );
    }

    void player2_ptr::counter(string_view text, unsigned value) {
        // TODO
    }

    void player2_ptr::score(string_view text, unsigned value) {
        players->player_infos[index].items.push_back({
            item_type::leaf, string(text)}
        );
        players->players[index].score(value);
        players->player_infos[index].game_over = true;
    }

    bool player2_ptr::game_over() {
        return players->player_infos[index].game_over;
    }

    void player2_ptr::print() {
        auto &player = players->player_infos[index];
        for (auto &item : player.items) {
            cout << item.text << endl;
        }
        player.items.clear();
        cout << player.prompt << " ";
    }

    void player2_ptr::set_human(bool human) {
        players->player_infos[index].human = human;
    }

    void player2_ptr::input(unsigned value) {
        players->player_infos[index].inputs.push_back(value);
    }

    void group_closer_t::operator()() {
        // TODO
    }
}
