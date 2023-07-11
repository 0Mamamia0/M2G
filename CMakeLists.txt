cmake_minimum_required(VERSION 3.22)
project(m2g)

set(CMAKE_CXX_STANDARD 20)

option(ENABLE_BUILD_SHARED "Enable a shared lib for java-m2g" OFF)
option(M2G_USE_JAMVM "Use JamVm" OFF)
option(M2G_USE_CASIRO "Use cairo graphics backend" ON)
option(M2G_STB_TRUETYPE_IMPL "Use stb_truetype_impl" ON)
option(M2G_STB_IMAGE_IMPL "Use stb_image_impl" ON)

include_directories(m2g java-m2g java-m2g-cairo m2g-cairo stb include ${SWITCH_INCLUDE_DIR}/freetype2)

aux_source_directory(stb STB_SRC)
aux_source_directory(m2g M2G_SRC)
aux_source_directory(java-m2g JAVA_M2G_SRC)
aux_source_directory(m2g-cairo M2G_CAIRO_SRC)
aux_source_directory(java-m2g-cairo JAVA_M2G_CAIRO_SRC)

add_library(m2g ${M2G_SRC} ${STB_SRC})
add_library(java-m2g ${JAVA_M2G_SRC})
add_library(m2g-cairo ${M2G_CAIRO_SRC} ${STB_SRC})
add_library(java-m2g-cairo ${JAVA_M2G_CAIRO_SRC})



#target_compile_definitions(m2g PRIVATE -D_USE_MATH_DEFINES)
if(M2G_STB_IMAGE_IMPL)
    target_compile_definitions(m2g PRIVATE -DM2G_USE_STB_IMAGE_IMPL)
    target_compile_definitions(m2g-cairo PRIVATE -DM2G_USE_STB_IMAGE_IMPL)
endif ()

if(M2G_STB_TRUETYPE_IMPL)
    target_compile_definitions(m2g-cairo PRIVATE -DM2G_USE_STB_TRUETYPE_IMPL)
endif()



if(M2G_USE_JAMVM)
    target_compile_definitions(java-m2g PRIVATE -DUSE_JAMVM)
    target_include_directories(java-m2g PUBLIC jni/jam )
else()
    target_include_directories(java-m2g PUBLIC jni/jvm)
endif()


if(ENABLE_BUILD_SHARED)
    add_library(java-m2g-shared SHARED java-m2g/JNI_OnLoad.cpp)
    target_link_libraries(java-m2g-shared java-m2g)
    target_compile_definitions(java-m2g-shared PRIVATE -DM2G_BUILD_SHARED)

#    add_library(java-m2g-cairo-shared SHARED )
endif()


