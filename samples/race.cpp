#include <cstdio>
#include <array>
#include <cassert>
#include <algorithm>
#include <cmath>

#include <gcad/replay.h>

using namespace std;

struct vec2 {
    int x, y;

    vec2(int x, int y) : x(x), y(y) {}
    vec2(int x) : x(x), y(x) {}
    vec2() : x(0), y(0) {}

    vec2 operator+(const vec2 &other) const {
        return vec2(x + other.x, y + other.y);
    }
    vec2 operator-(const vec2 &other) const {
        return vec2(x - other.x, y - other.y);
    }
    vec2 operator*(int scalar) const {
        return vec2(x * scalar, y * scalar);
    }
    vec2 operator*(const vec2 &other) const {
        return vec2(x * other.x, y * other.y);
    }
    vec2 operator/(int scalar) const {
        return vec2(x / scalar, y / scalar);
    }
    bool operator==(const vec2 &other) const {
        return x == other.x && y == other.y;
    }
};
vec2 apply(const auto f, vec2 a, vec2 b) {
    return vec2(f(a.x, b.x), f(a.y, b.y));
}
vec2 clamp(vec2 a, vec2 maximum) {
    return apply([](int a, int b) { return min(max(a, 0), b); }, a, maximum);
}

const bool course[] = {
    0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 0, 0, 0,
    0, 1, 0, 1, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 1, 0, 1, 0,
    0, 0, 0, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0,
};
const vec2 checkpoints[2] = {
    vec2(5, 5), vec2(1, 1)
};

struct grid_gradient {
    void update(gcad::replay_t &players);
    array<vec2, 100> path{0};
    unsigned time;
};

void grid_gradient::update(gcad::replay_t &players) {
    auto player = players[0];
    vec2 position = checkpoints[1] * 16 + 8, velocity;
    vec2 cell = position / 16;

    fill(path.begin(), path.end(), vec2());

    int next_checkpoint = 0;
    time = 100;
    for (; time > 0; time--) {
        player.see(position.x);
        player.see(position.y);

        int direction = player.choose(5).value();

        if (direction != 4) {
            int *axis = &velocity.x;
            if (direction & 1)
                axis = &velocity.y;

            if (direction & 2)
                (*axis)+=2;
            else
                (*axis)-=2;
        }
        
        cell = clamp((position + velocity) / 16, vec2(6));
        if (course[cell.x * 7 + cell.y]) {
            position = position + velocity;
        } else {
            velocity = vec2();
        }

        path[time - 1] = position;

        if (cell == checkpoints[next_checkpoint]) {
            next_checkpoint++;
            if (next_checkpoint == 2) {
                break;
            }
        }
    }

    player.score(time);
}

int main() {
    gcad::solver_t solver;
    gcad::replay_t players(1, &solver);

    grid_gradient game;
    for (auto iteration = 0u; iteration < 200'000; iteration++) {
        players.restart();
        game.update(players);
        if (iteration % 1000 == 0) {
            printf("Time: %u\n", game.time);
        }
    }

    for (auto path : game.path) {
        printf("%d;%d\n", path.x, path.y);
    }
}
