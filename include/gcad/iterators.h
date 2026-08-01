#pragma once

#include "replay.h"

namespace gcad {
    struct end_t {};

    struct tree_iterator_t {
        replay_t replay;
        int depth, current_depth;
        replay_t& operator*() {
            return replay;
        }
        bool operator!=(end_t) const {
            return !replay.events.empty();
        }
        void operator++();
    };

    struct tree_t {
        // TODO: is it necessary to have a copy of replay here?
        replay_t replay;
        int depth;
        tree_iterator_t begin();
        end_t end() {
            return {};
        }
    };

    inline tree_t tree(replay_t replay, int depth = -1) {
        return tree_t{replay, depth};
    }
}
