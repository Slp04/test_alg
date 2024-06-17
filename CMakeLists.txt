cmake_minimum_required(VERSION 3.5)

project(vkr VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

#set(UI_FILES mainwindow.ui startwindow.ui)

#qt_wrap_ui(UI_HEADERS ${UI_FILES})

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        hexeditor.cpp
        hexeditor.h
        startwidget.cpp
        startwidget.h
        reswidget.cpp
        reswidget.h
        reswidget.ui
        mainwindow.ui
        startwidget.ui
        #${UI_HEADERS}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(vkr
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

else()
    if(ANDROID)
        add_library(vkr SHARED
            ${PROJECT_SOURCES}
        )

    else()
        add_executable(vkr
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(vkr PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(vkr PROPERTIES
    WIN32_EXECUTABLE TRUE
)

install(TARGETS vkr
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(vkr)
endif()
