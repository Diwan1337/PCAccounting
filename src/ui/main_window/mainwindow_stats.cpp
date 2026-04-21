#include "mainwindow.h"

#include "ui/tabs/StatsTabWidget.h"

void MainWindow::refreshStats()
{
    if (statsTab)
        statsTab->refresh();
}
