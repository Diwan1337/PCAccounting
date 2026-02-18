#include "mainwindow.h"

#include "StatsTabWidget.h"

void MainWindow::refreshStats()
{
    if (statsTab)
        statsTab->refresh();
}
