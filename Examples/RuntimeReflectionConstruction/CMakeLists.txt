set (SRC_FILES 
     Main.cpp)

add_executable(entropy-runtime-reflection-construction-example ${SRC_FILES})

target_link_libraries(entropy-runtime-reflection-construction-example entropy::core::common entropy::reflection)
