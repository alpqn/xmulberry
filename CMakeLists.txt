cmake_minimum_required(VERSION 3.25)

project(xmulberry VERSION 0.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "ui/")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Mulberry.cpp
    src/Setting.cpp
    src/Utils.cpp
    src/MimeApp.cpp
    include/Mulberry.hpp # For moc
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
