cmake_minimum_required(VERSION 3.17)
project(engine)

set(CMAKE_CXX_STANDARD 14)

add_executable(engine main.cpp tablegame_engine.cpp tablegame_engine.h Board.h game.cpp game.h player.cpp player.h Board.cpp)