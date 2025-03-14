#include <cstdio>
#include <array>
#include <cassert>

#include <gcad/players.h>

struct grid_search {
    int x = 5, y = 5;

    void update(gcad::players_t &players);
};

void grid_search::update(gcad::players_t &players) {
    auto player = players[0];

    for (auto step = 0u; step < 5; step++) {
        player.see(x);
        player.see(y);

        int direction = player.choose(5).value();

        if (direction == 4)
            continue;

        int *axis = &x;
        if (direction & 1)
            axis = &y;

        if (direction & 2)
            (*axis)++;
        else
            (*axis)--;
    }

    players[0].score(x == 7 && y == 4);
}

int main() {
    gcad::solver_t solver;
    gcad::players_t players(1, &solver);

    std::array<std::array<unsigned, 11>, 11> histogram{{0}};

    for (auto iteration = 0u; iteration < 10'000; iteration++) {
        grid_search game;
        game.update(players);
        histogram[game.x][game.y]++;
        players.restart();
    }

    for (auto row : histogram) {
        for (auto cell : row) {
            printf("%u\t", cell);
        }
        printf("\n");
    }

    assert(histogram[7][4] > 100);
}
