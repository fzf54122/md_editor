cmake_minimum_required(VERSION 3.20)
project(md_editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets WebEngineWidgets WebChannel)

add_executable(md_editor
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/EditorBridge.cpp
    src/theme/Theme.cpp
    src/resources/resources.qrc
)

target_include_directories(md_editor PRIVATE src)

target_link_libraries(md_editor
    Qt5::Widgets
    Qt5::WebEngineWidgets
    Qt5::WebChannel
)
