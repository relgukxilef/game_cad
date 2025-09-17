#include <cstdio>
#include <array>
#include <cassert>

#include <gcad/replay.h>

using namespace std;

struct chance {
    int row, column;

    void update(gcad::replay_t &players);
};

void chance::update(gcad::replay_t &players) {
    auto player = players[0];

    row = player.choose(2).value();
    player.see(row);
    column = player.choose(2).value();

    // first row has better average score (10.5 vs 10), 
    // but second row has better max score (11 vs 12)
    // this requires differentiating between noise from reward and noise from TS
    auto probability = array<unsigned, 4>{10, 11, 8, 12}[row * 2 + column];

    unsigned secret = rand() % 20;

    float score = (secret < probability) ? 2.f : 1.f;

    player.score(score);
}

int main() {
    gcad::solver_t solver;
    gcad::replay_t players(1, &solver);

    array<unsigned, 4> histogram{0};

    for (auto iteration = 0u; iteration < 20; iteration++) {
        for (auto iteration = 0u; iteration < 1000; iteration++) {
            chance game;
            game.update(players);
            histogram[game.row * 2 + game.column]++;
            players.restart();
        }

        for (auto i : histogram) {
            printf("%lu ", i / (iteration + 1));
        }
        printf("\n");
    }

    assert(histogram[3] > 10'000);
}
