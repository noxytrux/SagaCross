cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on)

add_library( native-lib
             SHARED
             native-lib.h
             native-lib.cpp )

find_library( log-lib
              log )

target_link_libraries( native-lib
                       ${log-lib}
                       -ljnigraphics
                       -landroid)