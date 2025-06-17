cmake_minimum_required(VERSION 3.14)
project(parabola-swisseph LANGUAGES C CXX)

# -----------------------
# 1. Build Swiss Ephemeris
# -----------------------
add_library(swe STATIC
  swedate.c swehouse.c swejpl.c swemmoon.c swemplan.c sweph.c
  swephlib.c swecl.c swehel.c
)

target_include_directories(swe PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# -----------------------
# 2. Build Your Wrapper
# -----------------------
add_library(parabola_wrapper STATIC
  ${CMAKE_SOURCE_DIR}/parabola_wrapper.cpp
)

target_include_directories(parabola_wrapper PUBLIC
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(parabola_wrapper PRIVATE swe)

include_directories(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/headerspatch")

# -----------------------
# 3. Build the Tuner Executable
# -----------------------
add_executable(parabola_tuner
  ${CMAKE_SOURCE_DIR}/parabola_wrapper.cpp
)

target_link_libraries(parabola_tuner PRIVATE parabola_wrapper swe)

# -----------------------
# 4. Dependencies
# -----------------------
add_dependencies(parabola_wrapper swe)
add_dependencies(parabola_tuner parabola_wrapper)
