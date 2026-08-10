#include <gcad/replay.h>
#include <gcad/iterators.h>

#include <cstdio>
#include <utility>

using namespace std;

void play(gcad::replay_t& replay) {
    struct player {
        gcad::player_ptr replay;
        int money = 2;
        int card;
    };
    player players[2];

    // deal cards
    int deck[] = {1, 2, 3};
    int pot = 0;
    for (int i = 0; i < 2; i++) {
        auto& player = players[i];
        player.replay = replay[i];
        swap(deck[2 - i], deck[replay.random(3 - i)]);
        auto card = deck[2 - i];
        const char* cards[]{"draw jack", "draw queen", "draw king"};
        replay.set_event_name(cards[card - 1]);
        player.replay.see(card);
        player.card = card;
        player.money--;
        pot++;
    }

    bool end = false;
    bool bet = false, checked = false;

    while (!end) {
        auto& player = players[0];
        if (!bet) {
            auto choice = player.replay.choose(2).value();
            replay.set_event_name(choice ? "check " : "bet ");
            replay.see_all(choice);
            if (choice) {
                // check
                if (checked)
                    end = true;
                checked = true;
            } else {
                // bet
                player.money--;
                pot++;
                bet = true;
            }
        } else {
            auto choice = player.replay.choose(2).value();
            replay.set_event_name(choice ? "fold " : "call ");
            replay.see_all(choice);
            if (choice) {
                // fold
                player.card = 0;
            } else {
                // call
                player.money--;
                pot++;
            }
            end = true;
        }
        swap(players[0], players[1]);
    }

    if (players[0].card > players[1].card) {
        players[0].money += pot;
    } else {
        players[1].money += pot;
    }

    for (auto& player : players) {
        player.replay.score(float(player.money));
    }
}

int main() {
    gcad::solver_t solver;
    for (int i = 0; i < 1000; i++) {
        gcad::replay_t replay(2, &solver);
        play(replay);
    }

    for (gcad::replay_t& replay : gcad::tree({2, &solver})) {
        unsigned path_size = replay.event_size();
        play(replay);

        for (unsigned i = 0; i + 1 < path_size; i++) {
            auto event = replay.get_event(i);
            if (event.index + 1 == event.size)
                fputs(" ", stdout);
            else
                fputs("\xB3", stdout);
        }
        auto event = replay.get_event(path_size - 1);
        if (event.index + 1 == event.size)
            fputs("\xC0", stdout);
        else
            fputs("\xC3", stdout);
        printf(
            "%s: %.2f\n", event.name.c_str(),
            replay.get_expected_score(
                path_size - 1, event.index
            ).mean
        );
    }
}
