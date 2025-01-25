
cmake_minimum_required ( VERSION 3.21 )

set ( CMAKE_CXX_STANDARD 20 )
set ( CMAKE_CXX_STANDARD_REQUIRED True )

project ( MapleUI )

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_subdirectory ( dependencies )

add_subdirectory ( src )

option ( MAPLE_BUILD_TESTS "" ON )
if ( MAPLE_BUILD_TESTS )
    add_subdirectory ( tests )
endif ()

