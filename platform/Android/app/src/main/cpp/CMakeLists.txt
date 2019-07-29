cmake_minimum_required(VERSION 3.6.0)

set(CMAKE_VERBOSE_MAKEFILE on)

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

set( lib_game_src_DIR ${PROJECT_SOURCE_DIR}/../../../../../../source )

file(GLOB MATH_H "${lib_game_src_DIR}/Math/*.h")
file(GLOB MATH_CPP "${lib_game_src_DIR}/Math/*.cpp")

add_library( native-lib

             SHARED

             native-lib.h
             native-lib.cpp

             ${MATH_H}
             ${MATH_CPP}

             ${lib_game_src_DIR}/BasePathExtraction.cpp
             ${lib_game_src_DIR}/BasePathExtraction.hpp
             ${lib_game_src_DIR}/Core-pch.cpp
             ${lib_game_src_DIR}/Core-pch.hpp
             ${lib_game_src_DIR}/EnumOptionMacro.hpp
             ${lib_game_src_DIR}/glMatrix4x4.cpp
             ${lib_game_src_DIR}/glMatrix4x4.hpp
             ${lib_game_src_DIR}/glTextureLoader.cpp
             ${lib_game_src_DIR}/glTextureLoader.hpp
             ${lib_game_src_DIR}/growable_array.h
             ${lib_game_src_DIR}/json.hpp
             ${lib_game_src_DIR}/Math
             ${lib_game_src_DIR}/MathLib.h
             ${lib_game_src_DIR}/Mathlib.inl
             ${lib_game_src_DIR}/nuklear_glfw_gl3.h
             ${lib_game_src_DIR}/nuklear.h
             ${lib_game_src_DIR}/SCApplication.cpp
             ${lib_game_src_DIR}/SCApplication.h
             ${lib_game_src_DIR}/SCAudio.cpp
             ${lib_game_src_DIR}/SCAudio.h
             ${lib_game_src_DIR}/SCBaseScene.hpp
             ${lib_game_src_DIR}/SCBonus.h
             ${lib_game_src_DIR}/SCBulletManager.h
             ${lib_game_src_DIR}/SCCamera.h
             ${lib_game_src_DIR}/SCDebugScene.cpp
             ${lib_game_src_DIR}/SCDebugScene.hpp
             ${lib_game_src_DIR}/SCDisplay.cpp
             ${lib_game_src_DIR}/SCDisplay.h
             ${lib_game_src_DIR}/SCEntity.cpp
             ${lib_game_src_DIR}/SCEntity.h
             ${lib_game_src_DIR}/SCFastAnimatedMesh.h
             ${lib_game_src_DIR}/SCFastCutoffMesh.h
             ${lib_game_src_DIR}/SCFastMesh.h
             ${lib_game_src_DIR}/SCFastOceanMesh.h
             ${lib_game_src_DIR}/SCFastWaterMesh.h
             ${lib_game_src_DIR}/SCFrustum.hpp
             ${lib_game_src_DIR}/SCGround.h
             ${lib_game_src_DIR}/SCHeightMap.h
             ${lib_game_src_DIR}/SCInputInterface.h
             ${lib_game_src_DIR}/SCMenuScene.cpp
             ${lib_game_src_DIR}/SCMenuScene.hpp
             ${lib_game_src_DIR}/SCMeshInstanceManager.h
             ${lib_game_src_DIR}/SCMobileDisplay.cpp
             ${lib_game_src_DIR}/SCMobileDisplay.h
             ${lib_game_src_DIR}/SCMobileInput.cpp
             ${lib_game_src_DIR}/SCMobileInput.h
             ${lib_game_src_DIR}/SCNewGameScene.cpp
             ${lib_game_src_DIR}/SCNewGameScene.hpp
             ${lib_game_src_DIR}/SCNonCopyable.hpp
             ${lib_game_src_DIR}/SCOpenGLRenderer.cpp
             ${lib_game_src_DIR}/SCOpenGLRenderer.h
             ${lib_game_src_DIR}/SCParticleLayer.h
             ${lib_game_src_DIR}/SCRandomMeshPoint.h
             ${lib_game_src_DIR}/SCRenderable.h
             ${lib_game_src_DIR}/SCRenderer.cpp
             ${lib_game_src_DIR}/SCRenderer.h
             ${lib_game_src_DIR}/SCRendererInterface.h
             ${lib_game_src_DIR}/SCRenderScene.cpp
             ${lib_game_src_DIR}/SCRenderScene.hpp
             ${lib_game_src_DIR}/SCSceneManager.cpp
             ${lib_game_src_DIR}/SCSceneManager.h
             ${lib_game_src_DIR}/SCSettings.cpp
             ${lib_game_src_DIR}/SCSettings.h
             ${lib_game_src_DIR}/SCSettingsScene.cpp
             ${lib_game_src_DIR}/SCSettingsScene.hpp
             ${lib_game_src_DIR}/SCSparkles.cpp
             ${lib_game_src_DIR}/SCSparkles.h
             ${lib_game_src_DIR}/SCSphere.h
             ${lib_game_src_DIR}/SCTank.cpp
             ${lib_game_src_DIR}/SCTank.h
             ${lib_game_src_DIR}/SCVegetation.h
             ${lib_game_src_DIR}/SCVehicle.cpp
             ${lib_game_src_DIR}/SCVehicle.hpp
             ${lib_game_src_DIR}/ShaderProgram.cpp
             ${lib_game_src_DIR}/ShaderProgram.hpp
             ${lib_game_src_DIR}/stb_image.h
             ${lib_game_src_DIR}/Timer/GameTimer.hpp
             ${lib_game_src_DIR}/Timer/GameTimer.cpp
             ${lib_game_src_DIR}/Timer/AndroidTimer.hpp
             ${lib_game_src_DIR}/Timer/AndroidTimer.cpp)

include_directories( ${lib_game_src_DIR} )
include_directories( ${lib_game_src_DIR}/Timer )
include_directories( ${lib_game_src_DIR}/Math )

set(source "${PROJECT_SOURCE_DIR}/../../../../../../resource")
set(destination "${PROJECT_SOURCE_DIR}/../assets")

file (GLOB_RECURSE resources RELATIVE ${source} "${source}/*.*")

foreach(resource ${resources})

    message( "File is: ${resource}" )

    configure_file( ${source}/${resource} ${destination}/${resource} COPYONLY )

endforeach()

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