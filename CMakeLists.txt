#               MultiPlatform Qt5 | CMake Project
#            Platform: Windows/MacOS
#            Author:   Raiix
#            Date:     2019.11.8

cmake_minimum_required(VERSION 3.5)
project(MyQtAoo LANGUAGES CXX)

# Detect operation system
message(STATUS "Current Operation System is ${CMAKE_SYSTEM}")
if(WIN32)
    message(STATUS "Configuring for Windows.")
    set(CMAKE_PREFIX_PATH "C:\\pf\\Qt\\5.13.2\\mingw73_32")
endif(WIN32)
if(APPLE)
    message(STATUS "Configuring for MacOS.")
    foreach(QT_POSIBLE_PATH "/usr/local/Cellar/" "~/Applications/")
        message("Finding QT_PATH in ${QT_POSIBLE_PATH}")
        find_file(QT_PATH qt PATHS ${QT_POSIBLE_PATH})
        if(EXISTS ${QT_PATH})
            message("QT_PATH:${QT_PATH} found!")
            set(CMAKE_PREFIX_PATH "${QT_PATH}/5.13.2")
            if(NOT EXISTS ${CMAKE_PREFIX_PATH}/bin)
                set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}/clang_64)
            endif(NOT EXISTS ${CMAKE_PREFIX_PATH}/bin)
            break()
        else(EXISTS ${QT_PATH})
            message("QT_PATH not found in ${QT_POSIBLE_PATH}")
        endif(EXISTS ${QT_PATH})
    endforeach(QT_POSIBLE_PATH)
endif(APPLE)

# Detect build type
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE} ")
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(DEBUG 1)
else()
    set(RELEASE 1)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Change executable output path to output dir
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_subdirectory(RixJson)

add_executable(MyQtAoo
  main.cpp
  MainWindow.cpp
  MainWindow.h
  MainWindow.ui
        DataManager.cpp DataManager.h)

if(APPLE)
    set_target_properties(MyQtAoo PROPERTIES MACOSX_BUNDLE TRUE)
endif(APPLE)

target_link_libraries(MyQtAoo PRIVATE Qt5::Widgets RixJson)

# DLL Deployment.
set(DEPLOY_DIR ${EXECUTABLE_OUTPUT_PATH})

# Generated .bat to deploy dlls automatically for Windows
if(WIN32)
    if(DEBUG)
        set(DEPLOY_CONFIGURE debug)
    elseif(RELEASE)
        set(DEPLOY_CONFIGURE release)
    endif(DEBUG)
    set(DEPLOY_FUNC ${PROJECT_BINARY_DIR}/dlls_deployment.bat)
    file(GENERATE
            OUTPUT ${DEPLOY_FUNC}
            CONTENT
            "
echo off

:: Translate unix path to windows path
set \"WIN_DEPLOY_DIR=${DEPLOY_DIR}\"
set \"WIN_DEPLOY_DIR=%WIN_DEPLOY_DIR:/=\\%\"

:: Translate unix path to windows path
:: set \"WIN_PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}\"
:: set \"WIN_PROJECT_BINARY_DIR=%WIN_PROJECT_BINARY_DIR:/=\\%\"

:: Translate unix path to windows path
set \"WIN_QT_PATH=${CMAKE_PREFIX_PATH}\"
set \"WIN_QT_PATH=%WIN_QT_PATH:/=\\%\"

:: Add QT path to PATH
set PATH=%PATH%;%WIN_QT_PATH%\\bin

:: Create output dir if it not exists
if not exist %WIN_DEPLOY_DIR% (md  %WIN_DEPLOY_DIR%)

:: Use windeployqt.exe to copy the dependencies
windeployqt.exe %WIN_DEPLOY_DIR%\\MyQtAoo.exe -force -${DEPLOY_CONFIGURE}
            "
            )

    # By adding a target indicating deploy dir and func to be cleaned
    add_custom_target(
            DeployDLLs ALL
            DEPENDS MyQtAoo
            COMMAND ${DEPLOY_FUNC}
            BYPRODUCTS ${DEPLOY_DIR} ${DEPLOY_FUNC}
            COMMENT "[${CMAKE_SYSTEM_PROCESSOR}] Deploying dlls for ${CMAKE_SYSTEM_NAME} platform."
    )
endif(WIN32)

# Directly execute command to deploy shared libs automatically for MacOS
if(APPLE)
    if(DEBUG)
        set(DEPLOY_CONFIGURE -use-debug-libs)
    elseif(RELEASE)
        set(DEPLOY_CONFIGURE " ")
    endif(DEBUG)

    # By adding a target indicating deploy dir and func to be cleaned
    add_custom_target(
            DeployDLLs ALL
            DEPENDS MyQtAoo
            COMMAND echo
            BYPRODUCTS ${DEPLOY_DIR}
            COMMENT "[${CMAKE_SYSTEM_PROCESSOR}] Deploying dlls for ${CMAKE_SYSTEM_NAME} platform."
    )

    if(RELEASE)
        add_custom_command(
                TARGET DeployDLLs
                POST_BUILD
                COMMAND ${CMAKE_PREFIX_PATH}/bin/macdeployqt ${DEPLOY_DIR}/MyQtAoo.app -always-overwrite -dmg
        )
    endif(RELEASE)
endif(APPLE)


