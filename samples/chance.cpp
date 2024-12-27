#include <cstdio>
#include <array>

#include <gcad/players.h>

using namespace std;

struct communication {
    int move;

    void update(gcad::players_t &players);
};

void communication::update(gcad::players_t &players) {
    auto player = players[0];

    move = player.choose(2);

    auto probability = array<unsigned, 2>{10, 11}[move];

    unsigned secret = rand() % 20;

    int score = (secret < probability) ? 2 : 1;

    player.score(score);
}

int main() {
    gcad::players_t players(1);

    array<unsigned, 2> histogram{0};

    for (auto iteration = 0u; iteration < 10; iteration++) {
        for (auto iteration = 0u; iteration < 100; iteration++) {
            communication game;
            game.update(players);
            histogram[game.move]++;
            players.restart();
        }

        printf("%lu/%lu\n", histogram[0], histogram[1]);
    }
}