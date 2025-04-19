set (SRC_FILES 
     Main.cpp)

add_executable(entropy-transform-data-structures-example ${SRC_FILES})

target_link_libraries(entropy-transform-data-structures-example entropy::reflection)
