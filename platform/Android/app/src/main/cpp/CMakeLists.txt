cmake_minimum_required(VERSION 3.6.0)

set( lib_mod_src_DIR ${PROJECT_SOURCE_DIR}/../../../../../../libraries/FMOD )

add_library( fmod-lib SHARED IMPORTED )
set_target_properties( fmod-lib PROPERTIES IMPORTED_LOCATION
                       ${lib_mod_src_DIR}/lib/Android/${ANDROID_ABI}/libfmod.so )

add_library( fmodL-lib SHARED IMPORTED )
set_target_properties( fmodL-lib PROPERTIES IMPORTED_LOCATION
                       ${lib_mod_src_DIR}/lib/Android/${ANDROID_ABI}/libfmodL.so )

include_directories( ${lib_mod_src_DIR}/inc )
link_directories( ${lib_mod_src_DIR}/lib/Android/${ANDROID_ABI}/libfmod.so )
link_directories( ${lib_mod_src_DIR}/lib/Android/${ANDROID_ABI}/libfmodL.so )

add_library( native-lib
             SHARED
             native-lib.h
             native-lib.cpp )

find_library( log-lib
              log )

target_link_libraries( native-lib
                       GLESv2
                       ${log-lib}
                       fmod-lib
                       fmodL-lib
                       EGL
                       -ljnigraphics
                       -landroid)

#COPY SO LIBRARIES

add_custom_command( TARGET native-lib POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${lib_mod_src_DIR}/lib/Android/${ANDROID_ABI}/libfmod.so
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libfmod.so )

add_custom_command( TARGET native-lib POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${lib_mod_src_DIR}/lib/Android/${ANDROID_ABI}/libfmodL.so
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libfmodL.so )