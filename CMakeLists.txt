cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Set project name and version
project(ScrollOfTheUndead VERSION 0.1)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

## If you want to link SFML statically
# set(SFML_STATIC_LIBRARIES TRUE)

# Enable debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug 
    CACHE STRING 
      "Choose the type of build (Debug or Release)" FORCE)
endif()

# Export compile commands for completion engines (optional)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set option to control setting the resource path variable
option(USE_INSTALL_RESOURCE_PATH "Set resource path to install location" OFF)

if(NOT USE_INSTALL_RESOURCE_PATH)
  set(RESOURCE_PATH "${PROJECT_SOURCE_DIR}/Resources/")
else()
  set(RESOURCE_PATH "${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME}/Resources/")
endif()

# Generate config.h
configure_file(include/config.h.in config.h)

# Find SFML shared libraries
find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

# Compile executable
add_executable(ScrollOfTheUndead 
    src/Anim_Base.cpp 
    src/Anim_Directional.cpp 
    src/Character.cpp 
    src/Enemy.cpp 
    src/EntityBase.cpp 
    src/EntityManager.cpp 
    src/EventManager.cpp 
    src/Game.cpp 
    src/Main.cpp 
    src/Map.cpp 
    src/Player.cpp 
    src/SpriteSheet.cpp 
    src/State_ChooseMap.cpp
    src/State_Game.cpp
    src/State_GameOver.cpp
    src/State_Intro.cpp
    src/State_LevelCompleted.cpp
    src/State_MainMenu.cpp
    src/StateManager.cpp
    src/State_Paused.cpp
    src/State_YesNoMenu.cpp
    src/Window.cpp )

# Set include directory search paths
target_include_directories(ScrollOfTheUndead 
  PRIVATE
    "${PROJECT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
  )

# Set executable RPATH
set_target_properties(ScrollOfTheUndead
  PROPERTIES
      INSTALL_RPATH
        "${CMAKE_INSTALL_PREFIX}/lib"
  )

# Link executable to required SFML modules
target_link_libraries(ScrollOfTheUndead sfml-graphics sfml-audio)

# Set executable install location
install(TARGETS ScrollOfTheUndead DESTINATION bin)

# Set install location for resources
install(DIRECTORY Resources
  DESTINATION "${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME}"
)

