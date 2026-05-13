cmake_minimum_required(VERSION 3.16)
project(ps5-rental-arm-cpp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets Gui)

# Заглушить предупреждения об устаревшем OpenGL API на macOS
if(APPLE)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/Client.cpp
    src/ConsoleDevice.cpp
    src/Game.cpp
    src/RentalOrder.cpp
    src/RentalManager.cpp
    src/ConsoleOpenGLWidget.cpp
    src/RentalContract.cpp
    src/ReturnCheckResult.cpp
    src/ReturnMediaRecord.cpp
    src/ReturnBoxSimulator.cpp
)

set(HEADERS
    src/MainWindow.h
    src/Client.h
    src/ConsoleDevice.h
    src/Game.h
    src/RentalOrder.h
    src/RentalManager.h
    src/ConsoleOpenGLWidget.h
    src/RentalContract.h
    src/ReturnCheckResult.h
    src/ReturnMediaRecord.h
    src/ReturnBoxSimulator.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Gui
)
