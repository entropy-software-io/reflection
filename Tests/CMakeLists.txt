enable_testing()

set (TEST_LIST
    TypeInfo/TestCanCastTo.cpp
)
create_test_sourcelist (TEST_SOURCELIST TestsMain.cpp ${TEST_LIST})

add_executable (entropy-reflection-tests ${TEST_SOURCELIST})

target_include_directories(entropy-reflection-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(entropy-reflection-tests entropy::reflection entropy::core::common)

foreach (TEST_PATH ${TEST_LIST})
    get_filename_component (TName ${TEST_PATH} NAME_WE)
    get_filename_component (TDir ${TEST_PATH} DIRECTORY)
    
    if ("${TDir}" STREQUAL "")
        set (TDir "Default")
    endif()
      
    add_test (NAME Entropy::${TDir}::${TName} COMMAND entropy-reflection-tests ${TDir}/${TName})
endforeach ()
