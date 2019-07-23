cmake_minimum_required(VERSION 3.2)
project(FrameServer)

CMAKE_POLICY(SET CMP0020 NEW)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

FIND_PACKAGE(Qt5Widgets REQUIRED)

FILE(GLOB SOURCE_FILES "*.cpp" "*.h"
    "Views/*.cpp" "Views/*.h"
    "Interfaces/*.cpp" "Interfaces/*.h"
    "Presenters/*.cpp" "Presenters/*.h"
    "AVIEncode/*.cpp" "AVIEncode/*.h")
    

FILE(GLOB UI_FILES "Views/*.ui")

#INCLUDE_DIRECTORIES("${OCui_SOURCE_DIR}")
#INCLUDE_DIRECTORIES("${OCcore_SOURCE_DIR}")
    

