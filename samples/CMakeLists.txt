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
