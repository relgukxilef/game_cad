#include <cstdint>
#include <bit>
#include <iostream>

#include <gcad/players2.h>

using namespace gcad;

const char *results[] = {"Loss", "Draw", "Win"};

struct tic_tac_toe {
    uint16_t marks[2] = {0};

    void update(players2_t &players) {
        uint16_t board = 0b111'111'111;
        uint16_t
            vertical = 0b001'001'001,
            horizontal = 0b111,
            diagonal_0 = 0b001'010'100,
            diagonal_1 = 0b100'010'001;

        int score = 1; // 1 = draw
        for (int player = 0; player < 2; player++) {
            auto p = marks[player];
            for (int i = 0; i < 3; i++) {
                if (
                    (((p >> i) & vertical) == vertical) |
                    (((p >> i * 3) & horizontal) == horizontal)
                ) {
                    score = 2 * player;
                }
            }
            if (
                ((p & diagonal_0) == diagonal_0) |
                ((p & diagonal_1) == diagonal_1)
            ) {
                score = 2 * player;
            }
        }
        
        uint16_t occupied = marks[0] | marks[1];
        auto player = std::popcount(occupied) % 2;

        if (score != 1 || !(board & ~occupied)) {
            players[0].score(results[2 - score], 2 - score);
            players[1].score(results[score], score);
            return;
        }

        if (auto choice = players[player].choice("Cell index", 9)) {
            if (occupied & (1 << *choice)) {
                players[player].grid(1);
                players[player].label("Invalid move");
                return;
            }
            marks[player] |= (1 << *choice);

            players[player].grid(3);
            uint16_t m[2] = {marks[0], marks[1]};
            for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 3; x++) {
                    if (m[0] & 1) {
                        players[1 - player].label("o");
                    } else if (m[1] & 1) {
                        players[1 - player].label("x");
                    } else {
                        players[1 - player].label("_");
                    }
                    m[0] >>= 1;
                    m[1] >>= 1;
                }
            }
        }
    }
};

int main() {
    // sketch of how I want to use the classes
    solver_t solver;

    for (auto iteration = 0u; iteration < 100; iteration++) {
        tic_tac_toe game;
        players2_t players(2, &solver);

        while (!players[0].game_over() || !players[1].game_over()) {
            game.update(players);
        }
    }

    players2_t players(2);
    auto human = players[0];
    auto computer = players[1];

    tic_tac_toe game;

    while (true) {
        game.update(players);
        human.print();
        
        if (human.game_over())
            break;
        
        if (human.active()) {
            unsigned input;
            cin >> input;
            human.input(input);
            continue;
        }

        if (!computer.active())
            continue;

        const unsigned size = 1'000;
        for (auto iteration = 0u; iteration < size; iteration++) {
            players2_t hypothetical_players = computer.sample(&solver);
            tic_tac_toe hypothetical;

            while (!hypothetical_players[1].game_over()) {
                hypothetical.update(hypothetical_players);
            }

            cout << iteration * 100 / size << "%\r";
        }
        computer.input(
            solver.choose(
                players.players.current.players[computer.index].output, 9
            )
        );
        cout << endl;
    }
}
