cmake_minimum_required(VERSION 3.19)
project(PvZLiteQt LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

file(GLOB_RECURSE SOURCES
    core/*.cpp core/*.h
    animation/*.cpp animation/*.h
)

qt_add_executable(PvZLiteQt
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    resources/resources.qrc
    ${SOURCES}
)

target_link_libraries(PvZLiteQt
    PRIVATE
        Qt::Core
        Qt::Widgets
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
