
add_library ( MapleUI STATIC
              window.cpp
              )

set_target_properties ( MapleUI PROPERTIES 
                        ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
                        )

target_include_directories ( MapleUI
                             PRIVATE ${PROJECT_SOURCE_DIR}/include
                                     ${PROJECT_SOURCE_DIR}/include/MapleUI
                                     ${PROJECT_SOURCE_DIR}/dependencies/glfw/include
                                     ${PROJECT_SOURCE_DIR}/dependencies/glad/include
                             )

target_link_libraries ( MapleUI
                        PRIVATE glfw
                                glad
                        )

