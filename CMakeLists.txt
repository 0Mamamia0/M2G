cmake_minimum_required(VERSION 3.22)
project(m2g)

set(CMAKE_CXX_STANDARD 20)
option(ENABLE_BUILD_SHARED "Enable build shared lib for java-m2g" OFF)
option(M2G_USE_JAMVM "Use JamVm" OFF)


if(MSVC)
    add_compile_options(/W0 /permissive)
else()
    add_compile_options(-w)
endif()

add_subdirectory(stb)
aux_source_directory(shared SHARED_SRC)
aux_source_directory(m2g M2G_SRC)
aux_source_directory(java-m2g JAVA_M2G_SRC)

aux_source_directory(m2g-cairo M2G_CAIRO_SRC)
aux_source_directory(java-m2g-cairo JAVA_M2G_CAIRO_SRC)


set(JAVA_M2G_EXT_SRC
        java-m2g-ext/java-Objects.cpp
        java-m2g-ext/java-Typeface.cpp
        java-m2g-ext/M2G_OnLoad.cpp

)



add_library(m2g ${M2G_SRC} ${SHARED_SRC})
add_library(java-m2g ${JAVA_M2G_SRC} ${JAVA_M2G_EXT_SRC})
add_library(m2g-cairo ${M2G_CAIRO_SRC} ${SHARED_SRC})
add_library(java-m2g-cairo ${JAVA_M2G_CAIRO_SRC} ${JAVA_M2G_EXT_SRC})


target_include_directories(m2g PRIVATE include)
target_include_directories(m2g PUBLIC stb m2g shared)
target_include_directories(java-m2g PRIVATE include java-m2g-ext)
target_include_directories(java-m2g PUBLIC stb m2g)

target_include_directories(m2g-cairo PRIVATE include)
target_include_directories(m2g-cairo PUBLIC stb m2g-cairo shared)
target_include_directories(java-m2g-cairo PRIVATE include)
target_include_directories(java-m2g-cairo PUBLIC stb m2g-cairo shared java-m2g-ext)

target_compile_definitions(m2g PRIVATE -D_USE_MATH_DEFINES)
target_compile_definitions(m2g-cairo PRIVATE -D_USE_MATH_DEFINES)


target_link_libraries(java-m2g PRIVATE m2g)




if(M2G_USE_JAMVM)
    target_compile_definitions(java-m2g PRIVATE -DUSE_JAMVM)
    target_include_directories(java-m2g PUBLIC jni/jam )
else()
    target_include_directories(java-m2g PUBLIC jni/jvm)
endif()


if(ENABLE_BUILD_SHARED)
    add_library(java-m2g-shared SHARED java-m2g-ext/JNI_OnLoad.cpp)
    target_compile_definitions(java-m2g-shared PUBLIC  -DM2G_BUILD_SHARED )
    target_link_libraries(java-m2g-shared java-m2g m2g stb_truetype stb_image)

    add_library(java-m2g-cairo-shared SHARED java-m2g-ext/JNI_OnLoad.cpp)
    target_link_libraries(java-m2g-cairo-shared java-m2g-cairo cairo freetype m2g-cairo stb_image stb_truetype)
    target_compile_definitions(java-m2g-cairo-shared PRIVATE -DM2G_BUILD_SHARED )
endif()


