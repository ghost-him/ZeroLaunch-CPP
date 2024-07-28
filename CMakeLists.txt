cmake_minimum_required(VERSION 3.5)

project(ZeroLaunch VERSION 0.1 LANGUAGES CXX)

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
find_library(PROPSYS_LIB NAMES propsys HINTS "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.26100.0\\um\\x64")

set(PROJECT_SOURCES
        main.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ZeroLaunch
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
        model/chineseconvertpinyin.h model/chineseconvertpinyin.cpp
        controller/uwpapp.h controller/uwpapp.cpp
        ui/spinboxdelegate.h ui/spinboxdelegate.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ZeroLaunch APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ZeroLaunch SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ZeroLaunch
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ZeroLaunch PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(PROPSYS_LIB)
    target_link_libraries(ZeroLaunch PRIVATE ${PROPSYS_LIB})
else()
    message(FATAL_ERROR "Could not find propsys.lib")
endif()


message(${CMAKE_SOURCE_DIR}/model/pinyin.json)
message(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_custom_command(
    OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/pinyin.json
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/model/pinyin.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    DEPENDS ${CMAKE_SOURCE_DIR}/model/pinyin.json
    COMMENT "Copying pinyin.json to output directory"
)

add_custom_target(copy_pinyin_file ALL
    DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/pinyin.json
)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ZeroLaunch)
endif()
set_target_properties(ZeroLaunch PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ZeroLaunch
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ZeroLaunch)
endif()
