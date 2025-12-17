/*!
\mainpage GameCAD

\section Introduction

This library implements Monte Carlo Tree Search for general game playing. It is designed to be easy to drop into existing games to control NPCs or to automatically find optimal player strategies. It is vaguely inspired by fuzzing libraries. Players try pseudo-random actions, guided by the score they receive from the game. It works for both turn-based and real-time games, games with one or multiple players, both perfect information and hidden information games, zero-sum and non-zero-sum games, symmetric and asymmetric games.

The main entry point is the replay_t class. It allows recording and replaying the inputs and outputs of the game. By passing all moves and observations through it, the library can build a model of the game.

\snippet introduction.cpp monty_hall

`choose` may return a stored move or use a solver to come up with a potential move, or a mix of both. It is possible to play stored moves up to a point and then continue with the solver, or to fix one players moves and use the solver for the others. As the replay also stores each players observations, fixing a player also constrains the other players to moves that lead to the same observations for the fixed player. This allows sampling possible hidden states for a given list of observations.

Similar to a fuzzer, the solver makes guesses using previous playthroughs and is allowed to make mistakes. By letting the solver play many games, it converges to the optimal strategy. This can take many tries for complex games.

\section Status

Currently I'm trying to get the library into a state where it has a mostly stable API. There are a lot of open issues.
*/

//! [monty_hall]
#include <cassert>
#include <gcad/replay.h>

void monty_hall(gcad::replay_t& replay) {
    gcad::player_ptr player = replay[0];

    // price is behind one of three doors
    int price = replay.random(3);

    // player chooses one of the 3 doors
    int choice = player.choose(3).value();

    // the host reveals one of the doors that doesn't have the price
    int reveal;
    if (price != choice)
        reveal = 3 - price - choice;
    else
        reveal = (price + 1 + replay.random(2)) % 3;
    assert(reveal != price);
    assert(reveal != choice);

    // the player sees which door gets revealed
    player.see(reveal);

    // player may change their choice of door
    bool change = player.choose(2).value();
    if (change)
        choice = 3 - choice - reveal;
    assert(choice != reveal);

    // if the choice was correct, give the player a point
    if (choice == price)
        player.score(1);
    else
        player.score(0);
}

int main() {
    // solver stores model of the game
    gcad::solver_t solver;

    // play some games
    for (auto i = 0; i < 1000; i++) {
        // create an empty replay with 1 player
        gcad::replay_t replay(1, &solver);
        monty_hall(replay);
    }
}
//! [monty_hall]
