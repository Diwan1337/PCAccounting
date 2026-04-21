set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(BACKEND_DIR ${SRC_DIR}/backend)

cmake_minimum_required(VERSION 3.19)
project(PCAccountingQt LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(PCAccountingQt
    WIN32
    ${SRC_DIR}/app/main.cpp

    ${SRC_DIR}/ui/main_window/mainwindow.cpp
    ${SRC_DIR}/ui/main_window/mainwindow_ui.cpp
    ${SRC_DIR}/ui/main_window/mainwindow_actions.cpp
    ${SRC_DIR}/ui/main_window/mainwindow_search.cpp
    ${SRC_DIR}/ui/main_window/mainwindow_refresh.cpp
    ${SRC_DIR}/ui/main_window/mainwindow_stats.cpp
    ${SRC_DIR}/ui/main_window/mainwindow.h

    ${SRC_DIR}/ui/tabs/EmployeesTabWidget.cpp
    ${SRC_DIR}/ui/tabs/EmployeesTabWidget.h
    ${SRC_DIR}/ui/tabs/ComputersTabWidget.cpp
    ${SRC_DIR}/ui/tabs/ComputersTabWidget.h
    ${SRC_DIR}/ui/tabs/StatsTabWidget.cpp
    ${SRC_DIR}/ui/tabs/StatsTabWidget.h

    ${SRC_DIR}/ui/dialogs/EmployeeDialog.cpp
    ${SRC_DIR}/ui/dialogs/EmployeeDialog.h
    ${SRC_DIR}/ui/dialogs/ComputerDialog.cpp
    ${SRC_DIR}/ui/dialogs/ComputerDialog.h

    ${BACKEND_DIR}/core/Database.cpp
    ${BACKEND_DIR}/core/ApplicationController.cpp
    ${BACKEND_DIR}/crypto/CryptoService.cpp
    ${BACKEND_DIR}/storage/Serializer.cpp
    ${BACKEND_DIR}/storage/StorageService.cpp
    ${BACKEND_DIR}/utils/DateUtils.cpp

)

target_include_directories(PCAccountingQt
    PRIVATE
        ${SRC_DIR}
        ${BACKEND_DIR}
)

target_link_libraries(PCAccountingQt
    PRIVATE
        Qt::Core
        Qt::Widgets
        bcrypt
)
