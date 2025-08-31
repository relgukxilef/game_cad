#include <gcad/players.h>

#include <iostream>

using namespace gcad;
using namespace std;

/*
Observation matrix:
   A ~A
B  1  0
~B 0  0
*/

void inference(players_t &players) {
    bool a = players[0].choose(2).value();
    bool b = players[0].choose(2).value();
    players[1].see(a && b);
    bool a_hat = players[1].choose(2).value();
    players[1].score(a == a_hat);
    players[0].score(0);
}

int main() {
    solver_t solver;
    
    for (auto iteration = 0u; iteration < 10; iteration++) {
        players_t players(2, &solver);
        inference(players);
    }

    cout << "sample" << endl;

    players_t players(2, &solver);
    players[1].see(0);
    for (auto iteration = 0u; iteration < 100; iteration++) {
        players_t replay = players[1].sample(&solver);
        inference(replay);
    }

}
