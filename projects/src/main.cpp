#define USE_GUI 1   // 0 - консоль, 1 - GUI
#pragma execution_character_set("utf-8")

#if USE_GUI

#include "ui/MainWindow.h"
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE,
                   LPSTR,
                   int nCmdShow)
{
    MainWindow app(hInstance);
    return app.run(nCmdShow);
}

#else

int main() {
    return runConsole();  // функция из ConsoleApp.cpp
}

#endif
