cmake_minimum_required(VERSION 3.9)

set(PROJECT_NAME BroadcastMesg)
project(${PROJECT_NAME})

find_package(Doxygen)
find_package(Threads)
find_library(REALTIME_LIB rt)

set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "" FORCE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Werror -Wextra -Wstrict -Wwrite-strings -fno-common -g -O3 -std=gnu11")

add_executable(sender sender.c sharedlib.c sharedlib.h)
add_executable(receiver receiver.c sharedlib.c sharedlib.h)

if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(sender "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(receiver "${CMAKE_THREAD_LIBS_INIT}")
endif()

if(REALTIME_LIB)
    target_link_libraries(sender "${REALTIME_LIB}")
    target_link_libraries(receiver "${REALTIME_LIB}")
endif()

if(DOXYGEN_FOUND)
    add_custom_target(doc
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND ${DOXYGEN_EXECUTABLE})
endif()