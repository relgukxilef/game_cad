#include <cstdio>
#include <array>

#include <gcad/players.h>

struct grid_gradient {
    int x = 5, y = 5;

    void update(gcad::players_t &players);
};

void grid_gradient::update(gcad::players_t &players) {
    auto player = players[0];

    for (auto step = 0u; step < 5; step++) {
        int direction = player.choose(4);

        int *axis = &x;
        if (direction & 1)
            axis = &y;

        if (direction & 2)
            (*axis)++;
        else
            (*axis)--;

        player.see(x);
        player.see(y);
    }

    players[0].score(x);
}

int main() {
    gcad::players_t players;

    std::array<std::array<unsigned, 11>, 11> histogram{{0}};

    for (auto iteration = 0u; iteration < 100; iteration++) {
        grid_gradient game;
        game.update(players);
        histogram[game.x][game.y]++;
    }

    for (auto row : histogram) {
        for (auto cell : row) {
            printf("%u\t", cell);
        }
        printf("\n");
    }
}