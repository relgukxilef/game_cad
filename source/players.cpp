#include <gcad/players.h>

#include <cstdlib>

namespace gcad {

    player_ptr players_t::operator[](unsigned index) {
        return {this, index};
    }

    unsigned player_ptr::choose(unsigned maximum) {
        return rand() % maximum;
    }

    void player_ptr::see(unsigned value) {

    }

    void player_ptr::score(unsigned value) {

    }
    
}

