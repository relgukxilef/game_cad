add_executable(
    grid_gradient
    grid_gradient.cpp
)

target_link_libraries(grid_gradient PRIVATE GameCAD)

add_executable(
    communication
    communication.cpp
)

target_link_libraries(communication PRIVATE GameCAD)

add_executable(
    chance
    chance.cpp
)

target_link_libraries(chance PRIVATE GameCAD)

add_executable(
    grid_search
    grid_search.cpp
)

target_link_libraries(grid_search PRIVATE GameCAD)

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
