cmake_minimum_required(VERSION 3.16)
project(PCAccountingSystem)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app WIN32
    src/main.cpp
    src/ConsoleApp.cpp

    src/ui/MainWindow.cpp
    src/ui/Dialogs.cpp

    src/core/Database.cpp
    src/utils/DateUtils.cpp

    src/models/Employee.cpp
    src/models/Computer.cpp

    src/storage/StorageService.cpp
    src/storage/Serializer.cpp

    src/crypto/CryptoService.cpp

    src/services/ReportService.cpp
)

target_compile_definitions(app PRIVATE
    UNICODE
    _UNICODE
)

target_compile_options(app PRIVATE /utf-8)
