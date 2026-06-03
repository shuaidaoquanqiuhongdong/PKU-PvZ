cmake_minimum_required(VERSION 3.19)
project(PvZLiteQt LANGUAGES CXX)


find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)

qt_standard_project_setup()
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SOURCES
    core/*.cpp core/*.h
    animation/*.cpp animation/*.h
    ui/*.cpp ui/*.h
)

qt_add_resources(PvZLiteQt_RESOURCES resources/resources.qrc)

qt_add_executable(PvZLiteQt
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    ui/GraphicsButton.cpp
    ui/GraphicsButton.h
    ui/GameView.cpp
    ui/GameView.h
    ${PvZLiteQt_RESOURCES}
    ${SOURCES}
)

target_include_directories(PvZLiteQt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(PvZLiteQt PRIVATE PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(PvZLiteQt
    PRIVATE
        Qt::Core
        Qt::Widgets

        Qt::Multimedia

)

install(TARGETS PvZLiteQt
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET PvZLiteQt
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
