cmake_minimum_required(VERSION 3.31.4)

project(GLFWTest VERSION 1.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

if(UNIX)
  message("This is a ${CMAKE_SYSTEM_NAME} system")
  add_definitions(-Dcmakemacro="Unix")
elseif(WIN32)
  message("This is windows")
  add_definitions(-Dcmakemacro="Windows")
endif()


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/source)
set(
  SOURCES 
  ${SOURCE_DIR}/Engine.cpp 
  ${SOURCE_DIR}/Win.cpp
  ${SOURCE_DIR}/main.cpp
  ${SOURCE_DIR}/glad.c
  ${SOURCE_DIR}/Mathril.cpp
  ${SOURCE_DIR}/Camera.cpp
  ${SOURCE_DIR}/Mesh.cpp
  ${SOURCE_DIR}/Object.cpp
  ${SOURCE_DIR}/ObjectRenderer.cpp
  ${SOURCE_DIR}/Projection.cpp
  ${SOURCE_DIR}/ShaderProgram.cpp
  ${SOURCE_DIR}/Texture.cpp
  ${SOURCE_DIR}/Light.cpp
  ${SOURCE_DIR}/GeoMesh.cpp
  ${SOURCE_DIR}/GeoTerrain.cpp
  ${SOURCE_DIR}/GeoRenderer.cpp
  ${SOURCE_DIR}/GuiMesh.cpp
  ${SOURCE_DIR}/GuiRenderer.cpp
  ${SOURCE_DIR}/ShadowFBO.cpp
  ${SOURCE_DIR}/CascadedShadow.cpp
) 

set(INC_DIR ${CMAKE_SOURCE_DIR}/include)
set(
  HEADER_FILES 
  ${INC_DIR}/glad/glad.h
  ${INC_DIR}/GLFW/glfw3.h
  ${INC_DIR}/GLFW/glfw3native.h
  ${INC_DIR}/KHR/khrplatform.h
  ${INC_DIR}/Engine.h
  ${INC_DIR}/Win.h
  ${INC_DIR}/Mathril.h
  ${INC_DIR}/Camera.h
  ${INC_DIR}/Mesh.h
  ${INC_DIR}/Object.h
  ${INC_DIR}/ObjectRenderer.h
  ${INC_DIR}/Projection.h
  ${INC_DIR}/ShaderProgram.h
  ${INC_DIR}/Texture.h
  ${INC_DIR}/Light.h
  ${INC_DIR}/GeoMesh.h
  ${INC_DIR}/GeoTerrain.h
  ${INC_DIR}/GeoRenderer.h
  ${INC_DIR}/GuiMesh.h
  ${INC_DIR}/GuiRenderer.h
  ${INC_DIR}/ShadowFBO.h
  ${INC_DIR}/CascadedShadow.h
)

# can use this to pass config settings from cmake to source tree
# configure_file(${INC_DIR}/Config.h.in ${INC_DIR}/Config.h)


add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER_FILES})

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_BINARY_DIR}/compile_commands.json
          ${CMAKE_SOURCE_DIR}/compile_commands.json
)


link_directories(${CMAKE_SOURCE_DIR}/library)

#OpenGL
find_package(OpenGL REQUIRED)

#GLFW: NOTE: find_package does not works for libraries that are not installed 
set(GLFW_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include/GLFW")
include_directories(${GLFW_INCLUDE_DIRS})
set(GLFW_LIBRARIES "${CMAKE_SOURCE_DIR}/library/glfw3.lib")

# Put all libraries into a variable
set(LIBS ${GLFW_LIBRARIES} ${OPENGL_LIBRARY})

# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})

target_include_directories(${PROJECT_NAME} PUBLIC ${INC_DIR} ${GLFW_INCLUDE_DIRS})

# Copy Directories to bin
add_custom_target(copy-runtime-files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/inventory ${CMAKE_BINARY_DIR}/bin/inventory
    DEPENDS ${PROJECT_NAME})

# use following command
# cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..
