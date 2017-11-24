cmake_minimum_required(VERSION 3.9)
project(BroadcastMesg C)

find_package(Doxygen)
find_package(Threads)
find_library(REALTIME_LIB rt)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wextra -Wstrict-prototypes -pedantic")

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