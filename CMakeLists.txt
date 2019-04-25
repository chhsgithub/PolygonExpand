cmake_minimum_required (VERSION 2.8)


project (PolygonExpand)

set(OpenCV_DIR "C:/depends/opencv/build/")
find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)

include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

