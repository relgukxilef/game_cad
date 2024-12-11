
namespace gcad {

    struct player_ptr {
        struct players_t *players;
        unsigned index;

        unsigned choose(unsigned maximum);
        void see(unsigned value);
        void score(unsigned value);
    };

    struct players_t {
        player_ptr operator[](unsigned index);
    };

}