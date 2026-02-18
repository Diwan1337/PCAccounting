#include "mainwindow.h"

MainWindow::MainWindow(ApplicationController* controller,
                       QWidget* parent)
    : QMainWindow(parent),
    controller(controller)
{
    setupUi();
}
