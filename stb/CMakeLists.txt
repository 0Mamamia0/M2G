# This file handles building each task as sub-project.
#
# Each task becomes an independent project in which you can experiment.
#
# Tasks are added as separate sub-projects so as compilation errors in
# one task don't affect another task.


project(stb)


add_library(stb_image stb_image.c )
add_library(stb_truetype stb_truetype.c)