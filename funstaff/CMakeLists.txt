#cmake_minimum_required(VERSION 2.6)
project(funstaff)
option(ENABLE_QT_STATIC "static llink to qt" ON)
option(ENABLE_TOOLS "enable building tools" OFF)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/funstaff.rc.in
		${CMAKE_CURRENT_SOURCE_DIR}/src/funstaff.rc @ONLY NEWLINE_STYLE LF)

set(CMAKE_AUTOMOC ON)
	set(QtComponent Core Gui OpenGL Widgets Multimedia)

if(WIN32)
	if(MSVC AND ENABLE_QT_STATIC)
		list(APPEND QtComponent
			UiPlugin ThemeSupport FontDatabaseSupport EventDispatcherSupport PlatformCompositorSupport WindowsUIAutomationSupport)
	endif()
endif()

find_package(Qt5 5.12.1 COMPONENTS ${QtComponent} REQUIRED)
link_libraries(Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia)

#set(CMAKE_AUTORCC OFF)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#add_definitions(-DQT_NO_KEYWORDS)

if(WIN32 AND MSVC AND ENABLE_QT_STATIC)
	# For static Windows builds, we also need to pull in the Qt5 Platform
	# support library, which is not exposed to CMake properly, unfortunately
	get_target_property(QT_LIB_DIR "${Qt5Widgets_LIBRARIES}" LOCATION)
	get_filename_component(QT_LIB_DIR "${QT_LIB_DIR}" PATH)

	link_libraries(
		${Qt5HB_LIBRARIES}
		${Qt5PNG_LIBRARIES}
		${Qt5QTPCRE_LIBRARIES}
		${Qt5QTFREETYPE_LIBRARIES}
		Qt5::QWindowsIntegrationPlugin
		Qt5::QWindowsVistaStylePlugin
		Qt5::QICOPlugin
		Qt5::QWindowsAudioPlugin
		Qt5::EventDispatcherSupport
		Qt5::OpenGL
		Qt5::FontDatabaseSupport
		Qt5::ThemeSupport
		Qt5::PlatformCompositorSupport
		Qt5::WindowsUIAutomationSupport
		Qt5::QGifPlugin
	)

	if (WIN32)
		link_libraries(Qt5::DSServicePlugin)
	endif()

	add_definitions(-DSTATIC_QT5)
	add_definitions(-DQT_STATICPLUGIN)
endif()


qt5_wrap_ui(UI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/funstaff.ui)

set_property(SOURCE ${UI_SRCS} PROPERTY SKIP_AUTOMOC ON)

qt5_add_resources(RC_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/funstaff.qrc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
#include_directories(../third_party/WinReg)

set(funstaff_SRCS
	${UI_SRCS}
	${RC_SRCS}

	include/funstaff/funstaff.hpp

	src/funstaff.rc
	src/main.cpp
	src/funstaff.cpp
)

if (WIN32)
	link_libraries(version.lib)
endif()

add_executable(funstaff WIN32
	${funstaff_SRCS}
)