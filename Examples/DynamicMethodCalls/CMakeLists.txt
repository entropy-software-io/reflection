set (SRC_FILES 
     Main.cpp)

add_executable(entropy-dynamic-method-calls-example ${SRC_FILES})

target_link_libraries(entropy-dynamic-method-calls-example entropy::core::common entropy::reflection)
