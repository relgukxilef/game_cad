#include <gcad/players.h>

namespace gcad {

    player_ptr players_t::operator[](unsigned index) {
        return {this, index};
    }

    unsigned player_ptr::choose(unsigned maximum) {
        return 0;
    }

    void player_ptr::see(unsigned value) {

    }

    void player_ptr::score(unsigned value) {
        
    }
    
}

