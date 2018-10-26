cmake_minimum_required(VERSION 3.6)
project(blackhole)
set(CMAKE_CXX_FLAGS "-O2 -g -Wall -Wextra -pedantic --std=c++11 ${CMAKE_CXX_FLAGS}")

add_subdirectory(player)

set(PLAYERS
            kbhole
            kbhole1
            player1
            player2
            player3)

add_custom_target(run
                 COMMAND cp "$<TARGET_FILE:kbhole>" ${CMAKE_SOURCE_DIR}/bin/
                 COMMAND ./competition.sh ${PLAYERS}
                 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/
                 DEPENDS kbhole)
                 
add_custom_target(check
                 COMMAND cp "$<TARGET_FILE:kbhole>" ${CMAKE_SOURCE_DIR}/bin/
                 COMMAND ./caiaio
                 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/
                 DEPENDS kbhole)
