cmake_minimum_required(VERSION 3.20)
project(wxCoin3D)

include(FindPkgConfig)
include(FindOpenGL)

if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "GL is required")
endif()
if(NOT OPENGL_GLU_FOUND)
    message(FATAL_ERROR "GLU is required")
endif()

pkg_check_modules (COIN3D Coin REQUIRED)

find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)

include(${wxWidgets_USE_FILE} ${COIN3D_INCLUDE_DIRS})

add_subdirectory(wxGLWithCoin3D)