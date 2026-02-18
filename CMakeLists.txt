set(BACKEND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../projects/src)

cmake_minimum_required(VERSION 3.19)
project(PCAccountingQt LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(PCAccountingQt
    WIN32
    main.cpp
    mainwindow.cpp
    mainwindow_ui.cpp
    mainwindow_actions.cpp
    mainwindow_search.cpp
    mainwindow_refresh.cpp
    mainwindow_stats.cpp
    mainwindow.h
    EmployeesTabWidget.cpp
    EmployeesTabWidget.h
    ComputersTabWidget.cpp
    ComputersTabWidget.h
    StatsTabWidget.cpp
    StatsTabWidget.h
    EmployeeDialog.cpp
    EmployeeDialog.h
    ComputerDialog.cpp
    ComputerDialog.h

    ${BACKEND_DIR}/core/Database.cpp
    ${BACKEND_DIR}/core/ApplicationController.cpp
    ${BACKEND_DIR}/crypto/CryptoService.cpp
    ${BACKEND_DIR}/storage/Serializer.cpp
    ${BACKEND_DIR}/storage/StorageService.cpp
    ${BACKEND_DIR}/utils/DateUtils.cpp
    ${BACKEND_DIR}/models/Computer.cpp
    ${BACKEND_DIR}/models/Employee.cpp
)

target_include_directories(PCAccountingQt
    PRIVATE ${BACKEND_DIR}
)

target_link_libraries(PCAccountingQt
    PRIVATE
        Qt::Core
        Qt::Widgets
        bcrypt
)
