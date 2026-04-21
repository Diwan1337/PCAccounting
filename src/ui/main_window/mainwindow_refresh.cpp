#include "mainwindow.h"

#include "ui/tabs/EmployeesTabWidget.h"
#include "ui/tabs/ComputersTabWidget.h"

void MainWindow::refreshEmployees()
{
    if (employeesTab)
        employeesTab->refresh();
}

void MainWindow::refreshComputers()
{
    if (computersTab)
        computersTab->refresh();
}
