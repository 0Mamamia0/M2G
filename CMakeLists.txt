cmake_minimum_required(VERSION 3.22)
project(m2g)

set(CMAKE_CXX_STANDARD 17)

option(ENABLE_BUILD_SHARED "Enable a shared lib for java-m2g" OFF)
option(ENABLE_JAMVM "Use JamVm" ON)
aux_source_directory(m2g M2G_SRC)
aux_source_directory(java-m2g JAVA_M2G_SRC)

add_library(m2g ${M2G_SRC})



include_directories(m2g)
target_include_directories(m2g PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(m2g PRIVATE -D_USE_MATH_DEFINES)



if(${ENABLE_BUILD_SHARED})
    add_library(java-m2g SHARED ${JAVA_M2G_SRC})
    target_include_directories(java-m2g PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_definitions(java-m2g PRIVATE -DSTB_IMAGE_IMPLEMENTATION)
    target_link_libraries(java-m2g m2g)
else()
    add_library(java-m2g ${JAVA_M2G_SRC})
    target_include_directories(java-m2g PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_definitions(java-m2g PRIVATE -DSTB_IMAGE_IMPLEMENTATION)
endif()

if(ENABLE_JAMVM)
    target_compile_definitions(java-m2g PRIVATE -DUSE_JAMVM)
    target_include_directories(java-m2g PUBLIC jni/jam include ${CMAKE_CURRENT_SOURCE_DIR})
else()
    target_include_directories(java-m2g PUBLIC jni/jvm include ${CMAKE_CURRENT_SOURCE_DIR})
endif()
