/*!
\mainpage GameCAD

\section Introduction

This library implements Monte Carlo Tree Search for general game playing. It is designed to be easy to drop into existing games to control NPCs or to automatically find optimal player strategies. It is vaguely inspired by fuzzing libraries. Players try pseudo-random actions, guided by the score they receive from the game. It works for both turn-based and real-time games, games with one or multiple players, both perfect information and hidden information games, zero-sum and non-zero-sum games, symmetric and asymmetric games.

The main entry point is the replay_t class. It allows recording and replaying the inputs and outputs of the game. By passing all moves and observations through it, the library can build a model of the game. Here is an example of how to use it with the Monty Hall problem.

\snippet introduction.cpp monty_hall

Next you can sample the game tree to find strategies. For a "large" game tree like the one of Tic-Tac-Toe, this can take millions of playthroughs.

\snippet introduction.cpp explore

After enough playthroughs, it will converge to optimal play.

\snippet introduction.cpp play

The replay may return stored moves or use a solver to come up with a potential moves, or a mix of both. You can insert moves to get statistics about a specific node of the game tree.

\snippet introduction.cpp statistics

You can also sample possible game states from a given players inputs and observations to calculate statistics about hidden states. As the replay also stores each players observations, fixing a player also constrains the other players to moves that lead to the same observations for the fixed player. 

\snippet introduction.cpp hidden

Similar to a fuzzer, the solver makes guesses using previous playthroughs and is allowed to make mistakes. By letting the solver play many games, it converges to the optimal strategy. This can take many tries for complex games.

\section Status

Currently I'm trying to get the library into a state where it has a mostly stable API. There are a lot of open issues.
*/

//! [monty_hall]
#include <cassert>
#include <cstdio>
#include <gcad/replay.h>

struct monty_hall {
    int price, choice, reveal;
    bool change;
    int turn = 0;

    void step(gcad::replay_t& replay) {
        gcad::player_ptr player = replay[0];

        switch (turn++) {
        case 0:
            // price is behind one of three doors
            price = replay.random(3);

            // player chooses one of the 3 doors
            choice = player.choose(3).value();

            // the host reveals one of the doors that doesn't have the price
            reveal = 3 - price - choice;
            if (price == choice)
                reveal = (price + 1 + replay.random(2)) % 3;
            assert(reveal != price);
            assert(reveal != choice);

            // the player sees which door gets revealed
            player.see(reveal);
            break;
        case 1:
            // player may change their choice of door
            change = player.choose(2).value();
            if (change)
                choice = 3 - choice - reveal;
            assert(choice != reveal);

            // if the choice was correct, give the player a point
            if (choice == price)
                player.score(1);
            else
                player.score(0);
        }
    }

    void play(gcad::replay_t& replay) {
        step(replay);
        step(replay);
    }
};
//! [monty_hall]

//! [explore]
int main() {
    // solver stores model of the game
    gcad::solver_t solver;

    for (auto i = 0; i < 100; i++) {
        // create an empty replay with 1 player and play
        gcad::replay_t replay(1, &solver);
        monty_hall game;
        game.play(replay);
    }
//! [explore]

//! [play]
    // now lets see which strategy is the better one
    {
        int changed = 0, stayed = 0;
        for (auto i = 0; i < 10; i++) {
            gcad::replay_t replay(1, &solver);
            monty_hall game;
            game.play(replay);

            if (game.change)
                changed++;
            else
                stayed++;
        }
        printf("Changed %i times, stayed %i times.\n", changed, stayed);
        assert(changed >= 7);
    }
//! [play]

//! [statistics]
    // explore the game tree
    {
        gcad::replay_t replay(1, &solver);
        // let player pick the first door
        replay[0].input(0);
        // play for one turn
        monty_hall game;
        game.step(replay);
        // calculate expected score for changing door
        gcad::statistics score = replay[0].get_expected_score(1);
        printf(
            "Changing gives %.2f\u00b1%.2f points.\n", 
            score.mean, score.deviation
        );
        assert(score.mean > 0.6);
    }
//! [statistics]

//! [hidden]
    // explore which doors the host picks
    {
        int histogram[3] = {0};
        gcad::replay_t replay(1, &solver);
        // let player pick the first door
        replay[0].input(0);
        // sample potential continuations
        for (auto i = 0; i < 100; i++) {
            gcad::replay_t hypothesis = replay[0].sample(&solver);
            // play for one turn
            monty_hall game;
            game.play(hypothesis);

            histogram[game.reveal]++;
        }

        printf("Host revealed doors ");
        for (auto door : histogram) {
            printf("%ix, ", door);
        }
        printf("\n");
    }
}
//! [hidden]
