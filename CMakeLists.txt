cmake_minimum_required(VERSION 3.25)
project(m2g)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(m2g M2G_SRC)
aux_source_directory(java-m2g JAVA_M2G_SRC)

add_library(m2g ${M2G_SRC})
target_include_directories(m2g PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(m2g PRIVATE -D_USE_MATH_DEFINES)

add_library(java-m2g SHARED ${JAVA_M2G_SRC})
target_include_directories(java-m2g PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(java-m2g PRIVATE -DSTB_IMAGE_IMPLEMENTATION)
target_link_libraries(java-m2g m2g)