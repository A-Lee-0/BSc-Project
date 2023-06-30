cmake_minimum_required(VERSION 3.6)
set(SOURCE_FILES Game.cpp Drawing.cpp Experiment.cpp)
add_library(core ${SOURCE_FILES})

find_package(ImageMagick REQUIRED COMPONENTS Magick++ MagickCore MagickWand)
if(ImageMagick_FOUND)
    message(STATUS "ImageMagick_INCLUDE_DIRS: ${ImageMagick_INCLUDE_DIRS}")
    message(STATUS "ImageMagick_LIBARRIES: ${ImageMagick_LIBRARIES}")
    message(STATUS "ImageMagick_VERSION_STRING: ${ImageMagick_VERSION_STRING}")

    include_directories(${ImageMagick_INCLUDE_DIRS})
    target_compile_options(core PUBLIC -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16)
endif()

find_package(Boost REQUIRED COMPONENTS random filesystem)
if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
endif()

get_target_property(OUT core LINK_LIBRARIES)
message(STATUS ${OUT})

if(ImageMagick_FOUND)
    message(ImageMagick Found)
    target_link_libraries(core ${ImageMagick_LIBRARIES})
endif()
if (Boost_FOUND)
    message(Boost Found)
    target_link_libraries(core ${Boost_LIBRARIES})
endif()