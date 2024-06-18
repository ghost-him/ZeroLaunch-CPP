cmake_minimum_required(VERSION 3.5)

project(QuickLaunch VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置可执行文件的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/program)

# 设置共享库的输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/program)

# 设置静态库的输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/program)




find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QuickLaunch
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ./controller/keyboardhook.h ./controller/keyboardhook.cpp
        ./ui/icon.qrc
        ./ui/searchbar.h ./ui/searchbar.cpp
        ./ui/resultitem.h ./ui/resultitem.cpp ./ui/resultitem.ui
        ./ui/resultframe.h ./ui/resultframe.cpp ./ui/resultframe.ui
        ./controller/utils.cpp ./controller/utils.h
        ./controller/initprogram.h ./controller/initprogram.cpp
        ./model/database.h ./model/database.cpp
        ./controller/controller.h ./controller/controller.cpp
        ./ui/singleapplication.h ./ui/singleapplication.cpp
        ./ui/settingwindow.h ./ui/settingwindow.cpp ./ui/settingwindow.ui
        ./ui/Icon.rc
        controller/uicontroller.h controller/uicontroller.cpp
        controller/windowhook.h controller/windowhook.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET QuickLaunch APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(QuickLaunch SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(QuickLaunch
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(QuickLaunch PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QuickLaunch)
endif()
set_target_properties(QuickLaunch PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QuickLaunch
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QuickLaunch)
endif()
