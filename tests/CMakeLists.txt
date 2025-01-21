
add_executable ( Test1 test1.cpp _temp_gpu_fix.cpp )

target_include_directories ( Test1
                             PRIVATE ${PROJECT_SOURCE_DIR}/include
                             )

target_link_libraries ( Test1
                        PRIVATE MapleUI
                        )

add_executable ( Test2 test2.cpp )

target_include_directories ( Test2
                             PRIVATE ${PROJECT_SOURCE_DIR}/include
                             )

target_link_libraries ( Test2
                        PRIVATE MapleUI
                        )