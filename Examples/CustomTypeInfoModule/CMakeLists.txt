set (SRC_FILES 
     Main.cpp)

add_executable(entropy-custom-type-info-module-example ${SRC_FILES})

target_link_libraries(entropy-custom-type-info-module-example entropy::reflection)
