#include "gcad/replay.h"

#include <gcad/iterators.h>

using namespace std;

namespace gcad {
    void tree_iterator_t::operator++() {
        // TODO: there should be a function that resizes events and removes
        // player observations and inputs
        bool end = current_depth == replay.events.size();
        replay.events.resize(current_depth);
        replay_t next_replay = {replay.size(), replay.solver};
        next_replay.events = replay.events;
        replay = std::move(next_replay);

        if (!end && current_depth != depth) {
            replay.insert_event(0);

        } else {
            while (
                !replay.events.empty() &&
                ++replay.events.back().index == replay.events.back().size
            ) {
                replay.events.pop_back();
            }
        }

        current_depth = (int)replay.events.size();
    }

    tree_iterator_t tree_t::begin() {
        replay_t current_replay = {size, solver};
        if (depth != 0)
            current_replay.insert_event(0);
        return {current_replay, depth, 1};
    }
}
