#include <gcad/players2.h>

using namespace gcad;

struct guess {
    unsigned number = ~0;
    unsigned attempt = 0;

    guess(players2_t &players) {
        number = rand() % 100;
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

    for (auto iteration = 0u; iteration < 200; iteration++) {
        guess game(players);

        while (!players[0].game_over()) {
            game.update(players);
        }

        players.restart();
    }
}
