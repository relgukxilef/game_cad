add_library(
    GameCAD STATIC
    solver.cpp
    players.cpp
    players2.cpp
)

target_compile_features(GameCAD PUBLIC cxx_std_20)
