cmake_minimum_required(VERSION 3.6)
set(CORE_SOURCE_FILES Network.cpp Strategy.cpp Game.cpp Drawing.cpp Update.cpp Experiment.cpp Input.cpp Console.cpp)
add_library(core ${CORE_SOURCE_FILES})

if (WINBUILD)
    add_definitions(-DWINBUILD=1)
endif (WINBUILD)

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

if(ImageMagick_FOUND)
    target_link_libraries(core ${ImageMagick_LIBRARIES})
endif()
if (Boost_FOUND)
    target_link_libraries(core ${Boost_LIBRARIES})
endif()


find_path(PDC_INCLUDES curses.h)
find_library(PDC_LIBS pdcurses REQUIRED)

message(STATUS "PDCurses_INCLUDES: ${PDC_INCLUDES}")
message(STATUS "PDCurses_LIBRARIES: ${PDC_LIBS}")
target_include_directories(core PRIVATE ${PDC_INCLUDES})
target_link_libraries(core ${PDC_LIBS})