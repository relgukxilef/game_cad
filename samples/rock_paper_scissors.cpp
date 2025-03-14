#include <gcad/players2.h>

#include <iostream>

using namespace gcad;

const char *hands[] = {"Rock", "Paper", "Scissors"};
const char *results[] = {"Loss", "Draw", "Win"};

struct rock_paper_scissors {
    unsigned turn = 0;
    unsigned choices[2];

    void update(players2_t &players) {
        players[1 - turn].label("Waiting for other player...");
        if (auto choice = players[turn].choice(
            "Rock(0), paper(1), scissors(2)", 3
        )) {
            choices[turn] = *choice;
            turn++;
            if (turn == 2) {
                for (auto choice : choices)
                    for (auto p = 0; p < 2; p++) 
                        players[p].label(hands[choice]);
                    
                auto score = (3 + 1 + choices[0] - choices[1]) % 3;
                players[0].score(results[score], score);
                players[1].score(results[2 - score], 2 - score);
            }
        }
    }
};

int main() {
    players2_t players(2);

    for (auto iteration = 0u; iteration < 30'000; iteration++) {
        rock_paper_scissors game;

        while (!players[0].game_over() || !players[1].game_over()) {
            game.update(players);
        }

        players.restart();
    }

    auto human = players[0];

    rock_paper_scissors game;

    while (true) {
        game.update(players);
        human.print();
        if (human.game_over())
            break;
        if (!human.active())
            continue;
        unsigned input;
        cin >> input;
        human.input(input);
    }
}
