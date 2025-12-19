cmake_minimum_required(VERSION 3.20)
project(md_editor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets WebEngineWidgets WebChannel)

add_executable(md_editor
    src/main.cpp
    src/services/RecentHistory.cpp
    src/services/TextStatistics.cpp
    src/services/FileService.cpp
    src/ui/MenuBuilder.cpp
    src/ui/StatsPanel.cpp
    src/ui/DialogHelper.cpp
    src/ui/NotebookPane.cpp
    src/services/NotebookManager.cpp
    src/ui/EditorPane.cpp
    src/ui/MainWindow.cpp
    src/ui/EditorBridge.cpp
    src/theme/Theme.cpp
    src/resources/resources.qrc
)

if (WIN32)
    set_target_properties(md_editor PROPERTIES WIN32_EXECUTABLE TRUE)

    # 允许通过 -DAPP_ICON=... 覆盖图标路径，默认使用仓库中的 icon.ico
    set(DEFAULT_APP_ICON "${CMAKE_SOURCE_DIR}/src/resources/icon.ico")
    if (DEFINED APP_ICON AND APP_ICON)
        set(APP_ICON_PATH "${APP_ICON}")
    elseif (EXISTS "${DEFAULT_APP_ICON}")
        set(APP_ICON_PATH "${DEFAULT_APP_ICON}")
    else()
        set(APP_ICON_PATH "")
    endif()

    if (APP_ICON_PATH)
        configure_file(
            "${CMAKE_SOURCE_DIR}/cmake/app_icon.rc.in"
            "${CMAKE_BINARY_DIR}/app_icon.rc"
            @ONLY
        )
        target_sources(md_editor PRIVATE "${CMAKE_BINARY_DIR}/app_icon.rc")
        set(CPACK_NSIS_MUI_ICON "${APP_ICON_PATH}")
        set(CPACK_NSIS_MUI_UNIICON "${APP_ICON_PATH}")
        set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\md_editor.exe")
        set(CPACK_NSIS_DISPLAY_NAME "Md Editor")
    endif()
endif()

target_include_directories(md_editor PRIVATE src)

target_link_libraries(md_editor
    Qt5::Widgets
    Qt5::WebEngineWidgets
    Qt5::WebChannel
)

install(TARGETS md_editor DESTINATION bin)
install(FILES src/resources/icon.svg DESTINATION share/icons/hicolor/scalable/apps RENAME md-editor.svg)
install(FILES src/resources/md-editor.desktop DESTINATION share/applications)

set(CPACK_PACKAGE_NAME "md-editor")
set(CPACK_PACKAGE_VENDOR "Md Editor Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Typora style Markdown/PlainText editor built with Qt5")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "Md Editor Team <fzf54122@163.com>")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Md Editor Team")
set(CPACK_DEBIAN_PACKAGE_SECTION "editors")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "qtbase5-dev (>= 5.15), qtwebengine5-dev (>= 5.15)")

if (WIN32)
    set(CPACK_GENERATOR "NSIS")
endif()

include(CPack)
