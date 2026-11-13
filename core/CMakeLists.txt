add_library(core STATIC Matrix.cpp
        Vector.cpp
        Vector.h
        Solver.h
        Solver.cpp
        Fraction.h
        Fraction.cpp)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
