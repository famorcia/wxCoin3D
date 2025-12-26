# wxCoin3D Porting on wxWidgets of Coin3D (a.k.a Open Inventor) examples
# Copyright (C) 2022  Fabrizio Morciano

# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
# USA

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

find_package(GLUT REQUIRED)

pkg_check_modules (COIN3D Coin REQUIRED)

find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)

include(${wxWidgets_USE_FILE})
include_directories(${COIN3D_INCLUDE_DIRS} ${GLUT_INCLUDE_DIR})

add_subdirectory(minimal)
add_subdirectory(glutDuck)
add_subdirectory(wxIvViewer)
