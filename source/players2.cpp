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
        if (!player.human)
            return players->players[index].choose(maximum);

        if (player.inputs.empty()) {
            player.prompt = string(prompt);
            player.active = true;
            return {};
        }

        auto input = player.inputs.back();
        player.inputs.pop_back();
        player.prompt.clear();
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
            max_width = std::max<unsigned>(max_width, item.text.size());
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
        player.items.clear();
        if (player.active)
            cout << player.prompt << " ";
    }

    void player2_ptr::set_human(bool human) {
        players->player_infos[index].human = human;
    }

    void player2_ptr::input(unsigned value) {
        auto &player = players->player_infos[index];
        player.active = false;
        player.inputs.push_back(value);
    }

    bool player2_ptr::active() {
        return players->player_infos[index].active;
    }

    void player2_ptr::grid(unsigned columns) {
        auto &player = players->player_infos[index];
        player.columns = columns;
    }

    void group_closer_t::operator()() {
        // TODO
    }
}
