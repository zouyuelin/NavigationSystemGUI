cmake_minimum_required(VERSION 3.5)

project(NavigationSystem LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-std=c++14 -O3 -pthread -Wall")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt5 COMPONENTS Widgets COMPONENTS Widgets Core Gui REQUIRED)
find_package(OpenCV 4.3.0  REQUIRED)

#find_package(VTK 7.1 REQUIRED )

#VTK
set(VTK_INCLUDE_DIRS "/usr/local/include/vtk-7.1/")
FILE(GLOB VTK_LIBRARIES "/usr/local/lib/libvtk*")

#qwt
set(QWT_INCLUDE_DIRS "/usr/include/qwt/")
FILE(GLOB QWT_LIBRARIES "/usr/lib/libqwt*")

message(STATUS "${VTK_INCLUDE_DIRS}")
message(STATUS "${VTK_LIBRARIES}")

include_directories(${VTK_INCLUDE_DIRS} ${QWT_INCLUDE_DIRS})

add_executable(NavigationSystem
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    telnet.h
    telnet.cpp
    detect.cpp
    detect.h
    vtkrender.cpp
    vtkrender.h
    )

target_link_libraries(NavigationSystem ${OpenCV_LIBS} Qt5::Core Qt5::Widgets Qt5::Gui ${VTK_LIBRARIES} ${QWT_LIBRARIES})
