##

cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)

#handling of relative paths by link_directories() is defined by CMP0015 policy
cmake_policy(SET CMP0015 NEW)
set( CMAKE_CXX_STANDARD 11 )
find_package(OpenCV REQUIRED )

# Give a name to the project.
project(Nao_AdaptiveMotion)
# This include enable you to use our CMake framework
find_package(qibuild)
find_package(OpenGL REQUIRED)
find_package(LAPACK REQUIRED)

LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib)
LINK_DIRECTORIES(/usr/lib/x86_64-linux-gnu)


INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/rnn/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/NiTE2)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/OpenNI2)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/../NAO_SDK/naoqi-sdk/include)
INCLUDE_DIRECTORIES(${ARMADILLO_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE})
#Nao linking not required. All necessary stuff is provided by qibuild
#
FILE(GLOB_RECURSE header "include/*.hpp")
add_custom_target(headers SOURCES ${header})

FILE(GLOB_RECURSE headerNN "rnn/include/*.hpp")
add_custom_target(nnH SOURCES ${headerNN})

FILE(GLOB_RECURSE srcNN "rnn/src/*.cpp")
add_custom_target(nnS SOURCES ${srcNN})

#set source files
#-----------------------------------------------------
set(_src
 src/Kinect.cpp
 src/Nao.cpp
 src/Connectom.cpp
 src/UserSkeleton.cpp
 src/main.cpp
 src/SampleWriter.cpp
 src/ObjectTracker.cpp
 src/Utilities.cpp
 rnn/rnn.hpp
)
#---------------------------------------------------------

# Create an executable named Nao_AdaptiveMotion from sources
qi_create_bin(Nao_AdaptiveMotion ${_src})

target_link_libraries( Nao_AdaptiveMotion ${OpenCV_LIBS} )

#target_link_libraries (Nao_AdaptiveMotion libNiTE2.jni.so)
# Tell CMake that movehead depends on ALCOMMON and ALPROXIES
# This will set the libraries to link movehead with,
# the include paths, and so on
qi_use_lib(Nao_AdaptiveMotion ALCOMMON ALPROXIES)

#link other libs
#--------------------------------------------------------------------
target_link_libraries(Nao_AdaptiveMotion libOpenNI2.so)
target_link_libraries(Nao_AdaptiveMotion libNiTE2.so)
target_link_libraries(Nao_AdaptiveMotion ${OPENGL_LIBRARIES})
target_link_libraries(Nao_AdaptiveMotion libGLEW.so)
target_link_libraries(Nao_AdaptiveMotion libglut.so )
target_link_libraries(Nao_AdaptiveMotion ${ARMADILLO_LIBRARIES})
target_link_libraries(Nao_AdaptiveMotion ${LAPACK_LIBRARIES})

#target_link_libraries(Nao_AdaptiveMotion ${BLAS_LIBRARIES})
#------------------------------------------------------------------------

#copy dependencies to build
#----------------------------------------------------------------------
#copy libraries to bin folder
add_custom_command(TARGET Nao_AdaptiveMotion PRE_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/lib $<TARGET_FILE_DIR:Nao_AdaptiveMotion>)

#copy config folder to build folder for debugging
add_custom_command(TARGET Nao_AdaptiveMotion PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                      ${CMAKE_SOURCE_DIR}/config ${CMAKE_SOURCE_DIR}/build-naosdk_toolchain/config)

#copy config folder to bin folder
add_custom_command(TARGET Nao_AdaptiveMotion PRE_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/config $<TARGET_FILE_DIR:Nao_AdaptiveMotion>/config)

#copy NiTE2 folder to build for debugging
add_custom_command(TARGET Nao_AdaptiveMotion PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/lib/NiTE2 ${CMAKE_SOURCE_DIR}/build-naosdk_toolchain/NiTE2)
#------------------------------------------------------------------------------------------------------------
