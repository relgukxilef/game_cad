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
