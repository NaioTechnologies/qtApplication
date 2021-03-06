#==================================================================================================
#
#	CMake file for building ozHAL
#
#==================================================================================================


#--------------------------------------------------------------------------------------------------
#
#	CMake compatibility issues
#
cmake_minimum_required( VERSION 2.8.11 )


#--------------------------------------------------------------------------------------------------
#
#	Project name
#
project( Visioz )


#--------------------------------------------------------------------------------------------------
#
#   Module finding
#
set( CMAKE_MODULE_PATH  ${PROJECT_SOURCE_DIR}/cmake/modules )
find_package( VITALS	0.4.0	REQUIRED )
find_package( ROBBIE	0.3.0	REQUIRED )
find_package( OpenCV	2.4.5	REQUIRED )
find_package( Qt4		REQUIRED )

#find_package(Qt5Declarative)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_definitions(${QT_DEFINITIONS})

#--------------------------------------------------------------------------------------------------
#
#   Source & Include directories
#
set( INCLUDE_DIR	${PROJECT_SOURCE_DIR} )
set( SOURCE_DIR		${PROJECT_SOURCE_DIR} )


#--------------------------------------------------------------------------------------------------
#
#   Additional include directories
#
include_directories(
	${INCLUDE_DIR}
)

INCLUDE(${QT_USE_FILE})


#--------------------------------------------------------------------------------------------------
#
#	Set sources to compile
#
set( EXECUTABLE_SOURCES
	${SOURCE_DIR}/main.cpp
	${SOURCE_DIR}/mainwindow.cpp
	${SOURCE_DIR}/SaladDetector.cpp
	${SOURCE_DIR}/visionthread.cpp
	${SOURCE_DIR}/ObjectTracker.cpp
	
)

set( HEADERS
	${SOURCE_DIR}/mainwindow.h
	${SOURCE_DIR}/SaladDetector.h
	${SOURCE_DIR}/visionthread.h
	${SOURCE_DIR}/ObjectTracker.h
	
)

set( VISIOZ_UI
	${SOURCE_DIR}/mainwindow.ui
)





#--------------------------------------------------------------------------------------------------
#
#	Warning Options / C++ Language Options / Code Generation Options
#
add_definitions(
	-Wall
	-W
	-Wextra
	-pedantic
	-Winline
	# -Weffc++
	-std=c++1y
	-Wc++11-compat
	-Wconversion
	-Wconversion-null
	-Wsign-conversion
	-Wctor-dtor-privacy
	-Wdelete-non-virtual-dtor
	-Wnarrowing
	-Wnon-virtual-dtor
	-Wreorder
	-Wshadow
	-Wold-style-cast
	-Woverloaded-virtual
	-Wno-pmf-conversions
	-Wsign-promo
	-Wcast-align
	-Wfloat-equal
	-Wformat=2
	-Wmissing-declarations
	-Woverlength-strings
	-Wunreachable-code
	-fconstexpr-depth=10
	-fno-elide-constructors
	-ffor-scope
	-fno-gnu-keywords
	-ftemplate-backtrace-limit=10
	-fuse-cxa-atexit
	-fexceptions
	-fsigned-char
	-fdiagnostics-color
	-fPIE
)


#--------------------------------------------------------------------------------------------------
#
# Set default cmake build type to Debug (None Debug Release RelWithDebInfo)
#
if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Debug" )
endif()


if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	add_definitions(
		-fno-optimize-sibling-calls
        -fno-omit-frame-pointer

		-fsanitize=address
		-fsanitize=undefined
		#-fsanitize=thread
	)
endif()


#--------------------------------------------------------------------------------------------------
#
#   Add builds definitions
#
if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	add_definitions( -O1 -DDEBUG -g )
elseif( CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
	add_definitions( -O3 -DDEBUG -march=native -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 )
elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
	add_definitions( -O3 -march=atom -msse -msse2 -mssse3 -msse4.1 -msse4.2 -ffast-math -ftree-loop-if-convert -funroll-loops -mfpmath=sse )
endif()

#--------------------------------------------------------------------------------------------------
#
#   Executable creation
#

qt4_wrap_cpp(HEADERS_MOC ${HEADERS})

ADD_EXECUTABLE(${PROJECT_NAME} ${EXECUTABLE_SOURCES})


#--------------------------------------------------------------------------------------------------
#
#   Library includes
#
target_include_directories( ${PROJECT_NAME} SYSTEM PUBLIC
	${VITALS_INCLUDE_DIRS}
	${ROBBIE_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}

)


#--------------------------------------------------------------------------------------------------
#
#   Executable linking
#
target_link_libraries( ${PROJECT_NAME}
	${VITALS_LIBRARIES}
	${ROBBIE_LIBRARIES}
	${OpenCV_LIBRARIES}
	${QT_LIBRARIES}
		
)

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	target_link_libraries( ${PROJECT_NAME}
		-lasan
		-lubsan
		#-ltsan
	)
endif()

#--------------------------------------------------------------------------------------------------
#
#   Executable properties
#
set_target_properties( ${PROJECT_NAME} PROPERTIES

	# allow creating static and shared libs without conflicts
	CLEAN_DIRECT_OUTPUT 1

	# Executable name
	OUTPUT_NAME ${PROJECT_NAME}-1.0.2.5.6.8-r4587423
)


#--------------------------------------------------------------------------------------------------
#
#   Copying needed files
#
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE} )
file( COPY ${PROJECT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )


