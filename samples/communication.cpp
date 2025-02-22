#include <cstdio>
#include <array>
#include <cassert>

#include <gcad/players.h>

struct communication {
    int secret, color, guess;

    void update(gcad::players_t &players);
};

void communication::update(gcad::players_t &players) {
    auto communicator = players[0];
    auto guesser = players[1];

    // players know their role
    communicator.see(0);
    guesser.see(1);

    secret = rand() % 2;

    communicator.see(secret);

    color = communicator.choose(2);

    guesser.see(color);

    guess = guesser.choose(2);

    int score = (secret == guess) ? 1 : 0;

    guesser.score(score);
    communicator.score(score);
}

int main() {
    using namespace std;
    gcad::players_t players(2);

    array<array<array<unsigned, 2>, 2>, 2> histogram{{{0}}};

    for (auto iteration = 0u; iteration < 10'000; iteration++) {
        communication game;
        game.update(players);
        histogram[game.secret][game.color][game.guess]++;
        players.restart();
    }

    for (auto &secret : histogram) {
        printf("secret %llu\n", std::distance(&histogram[0], &secret));
        for (auto &color : secret) {
            printf("\tcolor %llu\n", std::distance(&secret[0], &color));
            for (auto &guess : color) {
                printf("\t\tguess %llu: ", std::distance(&color[0], &guess));
                printf("%u\n", guess);
            }
        }
        printf("\n");
    }

    assert(histogram[0][0][0] + histogram[0][1][0] > 2'500);
    assert(histogram[1][0][1] + histogram[1][1][1] > 2'500);
}
