cmake_minimum_required(VERSION 3.5)
set(CMAKE_TOOLCHAIN_FILE C:/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(LethalCompanyModpackInstaller VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Concurrent)

# Overall Libraries Include/Linking
include_directories(C:/vcpkg/installed/x64-windows/include)
link_directories(C:/vcpkg/installed/x64-windows/lib)

# Include zlib
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/zlib-1.3/include)
#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/zlib-1.3)

# Include libzip
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/libzip-1.10.1/include)
#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/libzip-1.10.1)

# Set UI build headers
FILE(GLOB UIs *.ui)
qt6_wrap_ui(UI_HEADERS ${UIs})

# Project Sources
set(PROJECT_SOURCES
        src/main.cpp

        # Implementation
        src/mainwindow.h src/mainwindow.cpp
        src/installer.h src/installer.cpp
        src/downloader.h src/downloader.cpp
        src/manager.h src/manager.cpp
        src/ziphandler.h src/ziphandler.cpp
        src/appexceptions.h src/appexceptions.cpp
        src/userdatahandler.h src/userdatahandler.cpp
        src/logger.h src/logger.cpp

        # UI
        src/mainwindow.ui
        src/stylesheets/stylesheet.qss
        src/stylesheets/stylesheet.vars

        # Other
        src/assets/icon.ico
        src/assets/icon.rc
        vcpkg.json
)

# Add missing libraries
#configure_file(src/assets/icon.ico ${CMAKE_CURRENT_BINARY_DIR}/icon.ico COPYONLY)

# Move Styling To Binary Folder]
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/styling)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/styling/fonts)
configure_file(src/fonts/dos.ttf ${CMAKE_CURRENT_BINARY_DIR}/styling/fonts/dos.ttf COPYONLY)
configure_file(src/stylesheets/stylesheet.qss ${CMAKE_CURRENT_BINARY_DIR}/styling/stylesheet.qss COPYONLY)
configure_file(src/stylesheets/stylesheet.vars ${CMAKE_CURRENT_BINARY_DIR}/styling/stylesheet.vars COPYONLY)

# App Icon
configure_file(src/assets/icon.ico ${CMAKE_CURRENT_BINARY_DIR}/icon.ico COPYONLY)
configure_file(src/assets/icon.rc ${CMAKE_CURRENT_BINARY_DIR}/icon.rc COPYONLY)
set(app_icon_resource_windows "${CMAKE_CURRENT_BINARY_DIR}/icon.rc")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

#target_sources(LethalCompanyModpackInstaller
#  PRIVATE
#    src/assets/icon.rc
#)
qt_add_executable(LethalCompanyModpackInstaller
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${app_icon_resouce_windows}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET LethalCompanyModpackInstaller APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(LethalCompanyModpackInstaller SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(LethalCompanyModpackInstaller
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(LethalCompanyModpackInstaller
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Concurrent
        zip.lib
        zlib.lib
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.LethalCompanyModpackInstaller)
endif()
set_target_properties(LethalCompanyModpackInstaller PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE

)

include(GNUInstallDirs)
install(TARGETS LethalCompanyModpackInstaller
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(LethalCompanyModpackInstaller)
endif()
