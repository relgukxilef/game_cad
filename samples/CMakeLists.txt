enable_testing()

add_executable(
    grid_gradient
    grid_gradient.cpp
)

target_link_libraries(grid_gradient PRIVATE GameCAD)

add_test(NAME grid_gradient COMMAND grid_gradient)

add_executable(
    communication
    communication.cpp
)

target_link_libraries(communication PRIVATE GameCAD)

add_test(NAME communication COMMAND communication)

add_executable(
    chance
    chance.cpp
)

target_link_libraries(chance PRIVATE GameCAD)

add_test(NAME chance COMMAND chance)

add_executable(
    grid_search
    grid_search.cpp
)

target_link_libraries(grid_search PRIVATE GameCAD)

add_test(NAME grid_search COMMAND grid_search)

add_executable(
    guess
    guess.cpp
)

target_link_libraries(guess PRIVATE GameCAD)

add_executable(
    rock_paper_scissors
    rock_paper_scissors.cpp
)

target_link_libraries(rock_paper_scissors PRIVATE GameCAD)

add_executable(
    tic_tac_toe
    tic_tac_toe.cpp
)

target_link_libraries(tic_tac_toe PRIVATE GameCAD)
target_compile_features(tic_tac_toe PUBLIC cxx_std_20)
