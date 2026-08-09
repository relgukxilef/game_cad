#include <gcad/replay.h>
#include <assert.h>

using namespace std;

void play(gcad::replay_t& replay) {

}

int main() {
//! [construct]
gcad::solver_t solver;
gcad::replay_t replay(2, &solver);
//! [construct]

//! [random]
unsigned int dice_roll = replay.random(6);
//! [random]
assert(dice_roll < 6);

//! [choose]
unsigned int door = replay[0].choose(2).value();

unsigned int cards = 0b11101;
unsigned int trick = replay[1].choose(5, cards).value();
cards &= ~(1 << trick);
//! [choose]
assert(door < 2);
assert((1 << trick) & 0b11101);

//! [see]
unsigned int card = 2;
replay[0].see(card);
//! [see]

//! [score]
bool victory = true;
if (victory) {
    replay[0].score(1);
} else {
    replay[0].score(0);
}
//! [score]

//! [sample]
gcad::replay_t hypothesis;
for (int i = 0; i < 100; i++) {
    hypothesis = replay[0].sample(&solver);
    play(hypothesis);
}
//! [sample]

replay = {2, &solver};
//! [input]
replay[0].input(1);
replay[0].input(2);
replay[1].input(2);
//! [input]
replay.see_all(1);
assert(replay[0].choose(3) == 1);
replay.see_all(1);
assert(replay[0].choose(3) == 2);
replay.see_all(1);
assert(replay[1].choose(3) == 2);
replay[0].score(0);
replay[1].score(0);

}
