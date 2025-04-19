set (SRC_FILES 
     Main.cpp)

add_executable(entropy-compile-time-reflection-example ${SRC_FILES})

target_link_libraries(entropy-compile-time-reflection-example entropy::reflection)
