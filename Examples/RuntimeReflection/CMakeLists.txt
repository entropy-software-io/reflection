set (SRC_FILES 
     Main.cpp)

add_executable(entropy-runtime-reflection-example ${SRC_FILES})

target_link_libraries(entropy-runtime-reflection-example entropy::reflection)
