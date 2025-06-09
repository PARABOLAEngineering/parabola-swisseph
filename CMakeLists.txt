add_library(swe STATIC
  swedate.c swehouse.c swejpl.c swemmoon.c swemplan.c sweph.c
  swephlib.c swecl.c swehel.c
)

target_include_directories(swe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 1. Build your wrapper as a static library:
add_library(parabola_wrapper STATIC
    ${CMAKE_SOURCE_DIR}/parabola_wrapper.cpp
)
target_include_directories(parabola_wrapper
    PUBLIC ${CMAKE_SOURCE_DIR}           # so it can see parabola_wrapper.h
)
target_link_libraries(parabola_wrapper
    PRIVATE swe                           # link in the Swiss Ephemeris core
)

# 2. Build your tester as an executable:
add_executable(parabola_tester
    ${CMAKE_SOURCE_DIR}/parabola_tester.cpp
)
target_link_libraries(parabola_tester
    PRIVATE parabola_wrapper
            swe
)

add_executable(parabola_tuner
    ${CMAKE_SOURCE_DIR}/parabola_tester.cpp
)
target_link_libraries(parabola_tuner
    PRIVATE parabola_wrapper
            swe
)
# 3. (Optional) make sure they’re built by default
add_dependencies(parabola_wrapper swe)
add_dependencies(parabola_tester parabola_wrapper)
