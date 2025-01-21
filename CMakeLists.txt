cmake_minimum_required(VERSION 3.27)
project(
  psm
  VERSION 0.1.0
  DESCRIPTION "Color Space conversion library"
  HOMEPAGE_URL "https://github.com/neg-c/psm"
  LANGUAGES CXX)

set(PROJECT_VERSION_SUFFIX "alpha")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(NoInSourceBuilds)

# Module options
option(PSM_BUILD_ADJUST_CHANNELS "Build the adjust_channels module" ON)
option(PSM_BUILD_ORGB "Build the oRGB module" ON)
option(PSM_BUILD_ADOBE_RGB "Build the AdobeRGB module" ON)

add_subdirectory(src bin)

include(Package)
include(Version)
include(Install)
