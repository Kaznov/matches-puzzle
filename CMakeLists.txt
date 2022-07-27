cmake_minimum_required(VERSION 3.12)
project("matches_puzzle" LANGUAGES C)
set(CMAKE_C_STANDARD_REQUIRED 99)

include(cmake/CompilerWarnings.cmake)
include(cmake/PreventInSourceBuilds.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/StandardProjectSettings.cmake)

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

add_library(project_sanitizers INTERFACE)
set_project_sanitizers(project_sanitizers)

add_executable(matches_puzzle
    main.c)

target_link_libraries(matches_puzzle PRIVATE project_warnings)
target_link_libraries(matches_puzzle PRIVATE project_sanitizers)