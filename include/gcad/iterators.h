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
        solver_t* solver;
        unsigned size;
        int depth;
        tree_iterator_t begin();
        end_t end() {
            return {};
        }
    };

    //! \brief Create an iterable object that produces all possible replays.
    //! For this to work, the returned replay needs to be played to the end 
    //! before advancing the iterator.
    //! \param replay The replay to take the number of players and solver from.
    //! \param depth The maximum number of events to expand. Set to -1 for 
    //! unlimited, which is only useful for small game trees.
    inline tree_t tree(replay_t replay, int depth = -1) {
        return tree_t{replay.solver, replay.size(), depth};
    }
}
