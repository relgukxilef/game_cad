#include <vector>
#include <optional>

#include "solver.h"

namespace gcad {
    struct player_t;

    struct replay_t;

    //! A player in a replay, returned by \ref replay_t::operator[].
    struct player_ptr {

        //! \brief Let the player make a choice between a number of options.
        //! \details The move is either read from the replay or calculated by
        //! the solver. 
        //! The sample is an estimate of optimal play, calculated from previous 
        //! playthroughs.
        //! If the replay contains no move for the current player,
        //! but does contain observations for other players, it will try to
        //! return moves from the set of subtrees leading to those 
        //! observations. This, too, is merely an estimate and it will
        //! occasionally return moves leading to contradicting observations.
        //! \param maximum The number of options. The returned value will be
        //! smaller than this.
        //! \param mask An optional bit mask of valid options. The function 
        //! will only return a value n if <tt>(1 << n) & mask</tt> is not 0.
        //! \return The estimated optimal move under constraints, or 
        //! \c std::nullopt if there is no move for this player in the replay
        //! and no solver was specified.
        //! \snippet snippets/functions.cpp choose
        // TODO: maybe choose should return unsigned
        std::optional<unsigned> choose(unsigned maximum, uint64_t mask = ~0);

        //! \brief Allow a player to make an observation.
        //! \details Players make decisions based on their observations. Two
        //! playthroughs are indistinguishable to a player, if it has the same
        //! series of observations.
        void see(unsigned value);

        //! \brief Mark the end of the game for this player, revealing their 
        //! score.
        //! \details The solver selects moves based on which give the highest
        //! expected score. This function may only be called once per player and
        //! playthrough. It is not necessary to end the game for all players at
        //! once.
        //! \param value The goodness of the players actions. Higher is better.
        void score(float value);

        //! \brief Create a new replay, constrained to the set of games that, to
        //! this player, are indistinguishable from this one.
        //! \details This can be used to sample the believe-space. 
        //! E.g. to estimate the likelihood of an opponent holding a specific
        //! card in a card game, one can take a number of samples, e.g. 100, 
        //! and count how often the opponent did have the card.
        // TODO: rename sample to fork
        replay_t sample(solver_t *solver);
        
        void resize(unsigned size);

        //! \brief Add a move from this player to the replay.
        //! \details This can be used to insert human moves or to restrict the
        //! sampling to a specific subtree.
        //! \bug Right now it is only valid to add a move when it's that players
        //! turn. See https://github.com/relgukxilef/game_cad/issues/20.
        void input(unsigned value);

        //! \brief Estimated the expected score for performing the given move at
        //! the current turn.
        statistics get_expected_score(unsigned choice);

        replay_t *players;
        unsigned index;
    };

    //! \brief Stores a full or partial list of moves and observations of a 
    //! playthrough.
    //! \details It allows replaying games and sampling the game tree or 
    //! subtrees of it.
    struct replay_t {
        //! \brief Construct a new replay for the given number of players and 
        //! given solver. \snippet snippets/functions.cpp construct
        replay_t(unsigned number_players = 1, solver_t *solver = nullptr);

        replay_t(replay_t &);
        ~replay_t();
        replay_t &operator=(const replay_t &);

        //! Access the information of the player with the given index.
        player_ptr operator[](unsigned index);
        void restart(); // TODO: remove

        //! Number of players.
        unsigned size();

        //! Let all players make the same observation.
        void see_all(unsigned value);

        //! \brief Read the next random event from the replay or sample it.
        //! \details If the replay is constrained, it will attempt to only 
        //! return values that don't contradict the constraints.
        //! \snippet snippets/functions.cpp random
        //! \param maximum The exclusive maximum value.
        unsigned random(unsigned maximum);

        //! \brief Whether the current state contradicts the constraints of the
        //! replay.
        //! \details Due to imperfect knowledge of the game tree, 
        //! \ref player_ptr::choose and \ref random can return values that 
        //! violate a constraint of the replay. 
        //! If a contradiction is detected, this function will return true. 
        //! This can be used to perform rejection sampling. 
        //! How likely this is depends on the complexity of the game
        //! and how many playthroughs have already been played.
        bool rejected();

        // TODO: maybe move to separate struct
        unsigned current_player = 0;
        unsigned current_choice = 0;
        bool constrained = false;
        unsigned constrained_player = 0;
        bool contradiction = false;
        std::vector<player_t> players;
        std::vector<unsigned> assumed_moves;
        std::vector<float> assumed_moves_weights;
        std::vector<unsigned> random_events;
        unsigned current_random_event = 0;

        solver_t *solver;
    };
}
