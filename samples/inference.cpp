#include <gcad/players.h>

#include <cstdio>
#include <cassert>

using namespace gcad;
using namespace std;

/*
Observation matrix:
   A ~A
B  1  0
~B 0  0
*/

void inference(players_t &players, unsigned histogram[]) {
    bool a = players[0].choose(2).value();
    bool b = players[0].choose(2).value();
    players[1].see(a && b);
    bool a_hat = players[1].choose(2).value();
    players[1].score(a == a_hat);
    players[0].score(0);
    histogram[a * 2 + b]++;
}

int main() {
    solver_t solver;

    unsigned histogram[4] = {};
    
    for (auto iteration = 0u; iteration < 240; iteration++) {
        players_t players(2, &solver);
        inference(players, histogram);
    }

    // should pick all paths equally
    for (auto &i : histogram) {
        assert(i < 90);
        i = 0;
    }

    players_t players(2, &solver);
    players[1].see(0);
    for (auto iteration = 0u; iteration < 240; iteration++) {
        players_t replay = players[1].sample(&solver);
        inference(replay, histogram);
    }

    printf(
        "%u\t%u\n%u\t%u\n", 
        histogram[0], histogram[1], histogram[2], histogram[3]
    );

    // should rarely pick the (true, true) path and others equally
    assert(histogram[0] < 110);
    assert(histogram[1] < 110);
    assert(histogram[2] < 110);
    assert(histogram[3] < 30);
}
