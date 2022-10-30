cmake_minimum_required(VERSION 3.5)

project(VideoPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(
    Qt${QT_VERSION_MAJOR}
    REQUIRED
    COMPONENTS
    Widgets
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Multimedia)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS OpenGLWidgets)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS OpenGL)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    XVideoWidget.cpp
    XVideoWidget.h
    XResample.cpp
    XResample.h
    XDemux.cpp
    XDemux.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(VideoPlayer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET VideoPlayer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(VideoPlayer SHARED
            ${PROJECT_SOURCES}
        )
    # Define properties for Android with Qt 5 after find_package() calls as:
    #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
else()
    add_executable(VideoPlayer
        ${PROJECT_SOURCES}
    )
endif()
endif()

# 添加ffmpeg头文件
include_directories(${PROJECT_SOURCE_DIR}/ffmpeg/include)

# 添加ffmpeg库文件
find_library(AVUTIL avutil ${PROJECT_SOURCE_DIR}/ffmpeg/lib)
find_library(AVFORMAT avformat ${PROJECT_SOURCE_DIR}/ffmpeg/lib)
find_library(SWSCALE swscale ${PROJECT_SOURCE_DIR}/ffmpeg/lib)
find_library(AVCODEC avcodec ${PROJECT_SOURCE_DIR}/ffmpeg/lib)
find_library(SWRESAMPLE swresample ${PROJECT_SOURCE_DIR}/ffmpeg/lib)


target_link_libraries(
    VideoPlayer
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    Qt${QT_VERSION_MAJOR}::OpenGL
    ${AVFORMAT}
    ${AVCODEC}
    ${AVUTIL}
    ${SWSCALE}
    ${SWRESAMPLE}
)

set_target_properties(VideoPlayer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS VideoPlayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VideoPlayer)
endif()
