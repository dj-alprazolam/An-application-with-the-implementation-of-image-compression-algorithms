cmake_minimum_required(VERSION 3.16)

project(app_compressor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(OpenCV_DIR "C:/opencv/build/x64/vc16" CACHE PATH "OpenCV MSVC build")
find_package(OpenCV REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

add_executable(${PROJECT_NAME} ${SOURCES} "src/gui/mainwindow.h" "src/gui/mainwindow.cpp" "src/algorithm/rle.h" "src/algorithm/compressionalgorithm.h" "src/algorithm/rle.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets ${OpenCV_LIBS})

if(MSVC)
    add_compile_options(/utf-8)
endif()