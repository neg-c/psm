cmake_minimum_required(VERSION 3.27)
project(
  psm
  VERSION 0.0.1
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(NoInSourceBuilds)

add_subdirectory(src bin)

include(Package)
include(Version)
include(Install)
