
add_subdirectory ( glfw )

add_library ( glad STATIC
              glad/src/gl.c
              )

target_include_directories ( glad
                             PRIVATE glad/include
                             )