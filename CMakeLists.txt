cmake_minimum_required(VERSION 3.19)
project(PvZLiteQt LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(PvZLiteQt
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    core/GameConfig.h
    core/GameEngine.cpp
    core/GameEngine.h
    core/GameEntity.cpp
    core/GameEntity.h
    core/GridManager.cpp
    core/GridManager.h
    core/Plant.cpp
    core/Plant.h
    core/Zombie.cpp
    core/Zombie.h
    animation/AnimationManager.cpp
    animation/AnimationManager.h
    animation/EffectItem.cpp
    animation/EffectItem.h
    animation/ResourceManager.cpp
    animation/ResourceManager.h
    animation/SpriteAnimation.cpp
    animation/SpriteAnimation.h
    resources/resources.qrc
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
