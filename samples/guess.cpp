#include <gcad/players2.h>

#include <iostream>

using namespace gcad;

struct guess {
    unsigned number = ~0;
    unsigned attempt = 0;

    guess(players2_t &players) {
        number = players.random(100);
    }

    void update(players2_t &players) {
        auto player = players[0];

        if (auto guess = player.choice("Guess the number", 100)) {
            if (*guess < number) {
                player.label("Guess too low");
            } else if (*guess > number) {
                player.label("Guess too high");
            } else {
                player.score("Correct", 1);
                return;
            }
            attempt++;
            if (attempt == 10) {
                player.score("No attempts left", 0);
            }
        }
    }
};

int main() {
    players2_t players(1);

    for (auto iteration = 0u; iteration < 1'000; iteration++) {
        guess game(players);

        while (!players[0].game_over()) {
            game.update(players);
        }

        players.restart();
    }

    printf("wins: %u\n", players.players.root.score_count[1]);

    players[0].set_human(true);

    guess game(players);

    while (true) {
        game.update(players);
        players[0].print();
        if (players[0].game_over())
            break;
        if (!players[0].active())
            continue;
        unsigned input;
        cin >> input;
        players[0].input(input);
    }
}
