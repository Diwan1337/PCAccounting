set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(BACKEND_DIR ${SRC_DIR}/backend)
set(OPENSSL_ROOT_DIR "E:/OpenSSL-Win64" CACHE PATH "Path to the local OpenSSL installation")

cmake_minimum_required(VERSION 3.19)
project(PCAccountingQt LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

find_path(OPENSSL_INCLUDE_DIR
    NAMES openssl/evp.h
    PATHS "${OPENSSL_ROOT_DIR}/include"
    NO_DEFAULT_PATH
)

if(MSVC)
    set(OPENSSL_LIB_DIR "${OPENSSL_ROOT_DIR}/lib/VC/x64/MD")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(OPENSSL_LIB_DIR "${OPENSSL_ROOT_DIR}/lib/VC/x64/MDd")
    endif()
    find_library(OPENSSL_CRYPTO_LIBRARY
        NAMES libcrypto
        PATHS "${OPENSSL_LIB_DIR}"
        NO_DEFAULT_PATH
    )
else()
    find_library(OPENSSL_CRYPTO_LIBRARY
        NAMES crypto libcrypto
        PATHS "${OPENSSL_ROOT_DIR}/lib" "${OPENSSL_ROOT_DIR}/lib64"
        NO_DEFAULT_PATH
    )
endif()

if(NOT OPENSSL_INCLUDE_DIR OR NOT OPENSSL_CRYPTO_LIBRARY)
    message(FATAL_ERROR
        "OpenSSL was not found. Check OPENSSL_ROOT_DIR or install a compiler-compatible OpenSSL package."
    )
endif()

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
        ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(PCAccountingQt
    PRIVATE
        Qt::Core
        Qt::Widgets
        ${OPENSSL_CRYPTO_LIBRARY}
)

if(EXISTS "${OPENSSL_ROOT_DIR}/bin/libcrypto-4-x64.dll")
    add_custom_command(TARGET PCAccountingQt POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${OPENSSL_ROOT_DIR}/bin/libcrypto-4-x64.dll"
            $<TARGET_FILE_DIR:PCAccountingQt>
    )
endif()
