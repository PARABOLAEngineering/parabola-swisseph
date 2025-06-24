cmake_minimum_required(VERSION 3.14)
project(parabolagpu LANGUAGES C CXX)

add_library(swe STATIC
  swedate.c swehouse.c swejpl.c swemmoon.c swemplan.c sweph.c
  swephlib.c swecl.c swehel.c
)

target_include_directories(swe PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(parabola_wrapper STATIC
  ${CMAKE_SOURCE_DIR}/parabola_wrapper.cpp
  ${CMAKE_SOURCE_DIR}/swevid_loader.cpp
)

target_include_directories(parabola_wrapper PUBLIC
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(parabola_wrapper PRIVATE swe)

add_executable(parabola_tuner
  ${CMAKE_SOURCE_DIR}/parabola_wrapper.cpp
)

target_link_libraries(parabola_tuner PRIVATE parabola_wrapper swe)

add_dependencies(parabola_wrapper swe)
add_dependencies(parabola_tuner parabola_wrapper)

# -----------------------
# 5. Install Rules for Swevid Loader Header
# -----------------------
install(FILES ${CMAKE_SOURCE_DIR}/swevid_loader.h
  DESTINATION include/parabola
)
