add_library(swe STATIC
  swedate.c swehouse.c swejpl.c swemmoon.c swemplan.c sweph.c
  swephlib.c swecl.c swehel.c
)

target_include_directories(swe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
