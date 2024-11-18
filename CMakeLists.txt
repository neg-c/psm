cmake_minimum_required(VERSION 3.26)
project(
  psm
  VERSION 1.0.0
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(NoInSourceBuilds)

add_subdirectory(src bin)

include(Package)
include(Install)
